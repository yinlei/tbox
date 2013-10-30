/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdio.h>
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_DEMO_FILE_READ_MAXN 			(65536)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */
typedef struct __tb_demo_context_t
{
	// the sock
	tb_handle_t 		sock;

	// the file
	tb_handle_t 		file;

	// the size
	tb_hize_t 			size;

}tb_demo_context_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_context_exit(tb_aicp_t* aicp, tb_demo_context_t* context)
{
	if (context)
	{
		if (context->file) 
		{
			tb_aicp_delo(aicp, context->file);
			tb_file_exit(context->file);
		}
		if (context->sock) 
		{
			tb_aicp_delo(aicp, context->sock);
			tb_socket_close(context->sock);
		}
		tb_free(context);
	}
}
static tb_bool_t tb_demo_file_read_func(tb_aicp_t* aicp, tb_aice_t const* aice);
static tb_bool_t tb_demo_sock_send_func(tb_aicp_t* aicp, tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aicp && aice && aice->code == TB_AICE_CODE_SEND, tb_false);

	// the context
	tb_demo_context_t* context = (tb_demo_context_t*)aice->data;
	tb_assert_and_check_return_val(context, tb_false);

	// ok?
	if (aice->state == TB_AICE_STATE_OK)
	{
		// trace
//		tb_print("send[%p]: real: %lu, size: %lu", aice->handle, aice->u.send.real, aice->u.send.size);

		// continue?
		if (aice->u.send.real < aice->u.send.size)
		{
			// post send to client
			if (!tb_aicp_send(aicp, aice->handle, aice->u.send.data + aice->u.send.real, aice->u.send.size - aice->u.send.real, tb_demo_sock_send_func, context)) return tb_false;
		}
		// ok? 
		else 
		{
			// post read from file
			if (!tb_aicp_read(aicp, context->file, context->size, TB_DEMO_FILE_READ_MAXN, tb_demo_file_read_func, context)) return tb_false;
		}
	}
	// closed or failed?
	else
	{
		if (aice->state == TB_AICE_STATE_CLOSED)
			tb_print("send[%p]: closed", aice->handle);
		else tb_print("send[%p]: failed: %lu", aice->handle, aice->state);
		tb_demo_context_exit(aicp, context);
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_demo_file_read_func(tb_aicp_t* aicp, tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aicp && aice && aice->code == TB_AICE_CODE_READ, tb_false);

	// the context
	tb_demo_context_t* context = (tb_demo_context_t*)aice->data;
	tb_assert_and_check_return_val(context, tb_false);

	// ok?
	if (aice->state == TB_AICE_STATE_OK)
	{
		// trace
//		tb_print("read[%p]: real: %lu, size: %lu", aice->handle, aice->u.read.real, aice->u.read.size);

		// has data?
		if (aice->u.read.real)
		{
			// post send to client
			if (!tb_aicp_send(aicp, context->sock, aice->u.read.data, aice->u.read.real, tb_demo_sock_send_func, context)) return tb_false;

			// save size
			context->size += aice->u.read.real;
		}
		// no data?
		else 
		{	
			// post read from file
			if (!tb_aicp_read(aicp, context->file, context->size, TB_DEMO_FILE_READ_MAXN, tb_demo_file_read_func, context)) return tb_false;
		}
	}
	// closed or failed?
	else
	{
		if (aice->state == TB_AICE_STATE_CLOSED)
			tb_print("read[%p]: closed", aice->handle);
		else tb_print("read[%p]: failed: %lu", aice->handle, aice->state);
		tb_demo_context_exit(aicp, context);
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_demo_sock_acpt_func(tb_aicp_t* aicp, tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aicp && aice && aice->code == TB_AICE_CODE_ACPT, tb_false);

	// the file path
	tb_char_t const* path = (tb_char_t const*)aice->data;
	tb_assert_and_check_return_val(path, tb_false);

	// trace
	tb_print("acpt[%p]: %p", aice->handle, aice->u.acpt.sock);

	// acpt ok?
	if (aice->state == TB_AICE_STATE_OK)
	{
		// done
		tb_bool_t 			ok = tb_false;
		tb_demo_context_t* 	context = tb_null;
		do
		{
			// check
			tb_assert_and_check_break(aice->u.acpt.sock);

			// make context
			context = tb_malloc0(sizeof(tb_demo_context_t));
			tb_assert_and_check_break(context);

			// init context
			context->sock = aice->u.acpt.sock;
			context->file = tb_file_init(path, TB_FILE_MODE_RO | TB_FILE_MODE_AICP);
			tb_assert_and_check_break(context->file);

			// addo sock
			if (!tb_aicp_addo(aicp, context->sock, TB_AICO_TYPE_SOCK)) break;

			// addo file
			if (!tb_aicp_addo(aicp, context->file, TB_AICO_TYPE_FILE)) break;

			// post read from file
			if (!tb_aicp_read(aicp, context->file, context->size, TB_DEMO_FILE_READ_MAXN, tb_demo_file_read_func, context)) break;

			// ok
			ok = tb_true;

		} while (0);

		// failed?
		if (!ok)
		{
			// exit context
			if (context) tb_demo_context_exit(aicp, context);

			// exit loop
			return tb_false;
		}
		else
		{
			// continue it
			if (!tb_aicp_acpt(aicp, aice->handle, tb_demo_sock_acpt_func, path)) return tb_false;
		}
	}
	// failed?
	else
	{
		// exit loop
		tb_print("acpt[%p]: failed: %lu", aice->handle, aice->state);
		return tb_false;
	}

	// ok
	return tb_true;
}
static tb_pointer_t tb_demo_loop_thread(tb_pointer_t data)
{
	// aicp
	tb_handle_t aicp = data;
	tb_size_t 	self = tb_thread_self();

	// trace
	tb_print("[loop: %lu]: init", self);

	// loop aicp
	if (aicp) tb_aicp_loop(aicp, -1);
	
	// trace
	tb_print("[loop: %lu]: exit", self);

	// exit
	tb_thread_return(tb_null);
	return tb_null;
}
/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// check
	tb_assert_and_check_return_val(argv[1], 0);

	// init tbox
	if (!tb_init(malloc(10 * 1024 * 1024), 10 * 1024 * 1024)) return 0;

	// init
	tb_handle_t 	sock = tb_null;
	tb_handle_t 	aicp = tb_null;
	tb_handle_t 	loop[16] = {0};

	// open sock
	sock = tb_socket_open(TB_SOCKET_TYPE_TCP);
	tb_assert_and_check_goto(sock, end);

	// bind port
	if (!tb_socket_bind(sock, tb_null, 9090)) goto end;

	// listen sock
	if (!tb_socket_listen(sock)) goto end;

	// init aicp
	aicp = tb_aicp_init(16);
	tb_assert_and_check_goto(aicp, end);

	// addo sock
	if (!tb_aicp_addo(aicp, sock, TB_AICO_TYPE_SOCK)) goto end;

	// post acpt
	if (!tb_aicp_acpt(aicp, sock, tb_demo_sock_acpt_func, argv[1])) goto end;

	// done loop
	loop[0] = tb_thread_init(tb_null, tb_demo_loop_thread, aicp, 0);
	loop[1] = tb_thread_init(tb_null, tb_demo_loop_thread, aicp, 0);
	loop[2] = tb_thread_init(tb_null, tb_demo_loop_thread, aicp, 0);
	loop[3] = tb_thread_init(tb_null, tb_demo_loop_thread, aicp, 0);

	// wait exit
	getchar();

end:

	// trace
	tb_print("end");

	// kill aicp
	if (aicp) tb_aicp_kill(aicp);
	
	// wait exit
	{
		// exit loop
		tb_handle_t* l = loop;
		for (; *l; l++)
		{
			if (!tb_thread_wait(*l, -1))
				tb_thread_kill(*l);
			tb_thread_exit(*l);
		}
	}

	// exit aicp
	if (aicp) tb_aicp_exit(aicp);

	// exit sock
	if (sock) tb_socket_close(sock);

	// exit tbox
	tb_exit();
	return 0;
}