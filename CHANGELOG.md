# Changelog

## master (unreleased)

### Bugs fixed

* Fix stdout compatibility issue for vs2015

## v1.5.2

### New features

* Add smallest configure option
* Add process operation interfaces

### Changes

* Improve envirnoment interfaces
* Modify xmake.lua for supporting xmake v2.x

### Bugs fixed

* Fix ltimer bug
* Fix asio memory leaks bug
* Fix asio httpd response bug on linux
* Fix path bug for windows

## v1.5.1

### New features

* Add automaticlly check libc interfaces
* Support custom allocator 
* Add trace for allocator in the debug mode
* Add `static_pool` module
* Add stream interfaces for reading all data to string
* Add adler32 hash algorithm
* Add `tb_memmem` interface
* Add regex module with pcre, pcre2 or posix regex 

### Changes

* Optimize stream and support read/write character device file
* Modify `tb_init` api and support allocator arguments
* Improve memory manager and use the allocator mode
* Redefine `assert` and will abort for debug mode 

### Bugs fixed

* Fix some bugs for android
* Fix seek bug for stream

# 更新日志

## master (开发中)

### Bugs修复

* 修复stdout在vs2015以上版本的兼容性问题

## v1.5.2

### 新特性

* 增加smallest参数配置选项，实现一键配置最小化编译，禁用所有扩展模块和依赖库
* 增加进程创建和控制接口

### 改进

* 增强环境变量设置接口
* 修改xmake.lua支持最新版xmake v2.x, 简化编译配置

### Bugs修复

* 修复ltimer定时器不准问题
* 修复asio部分内存泄露问题
* 修复asio/httpd在linux下keepalive模式，响应很慢问题
* 修复windows下路径处理的一些bug

## v1.5.1

### 新特性

* 自动检测所有系统libc接口，优先使用系统版本
* 支持自定义内存分配器，并且能够在debug模式下，获取每次分配的代码位置信息，用于自定义追踪
* 增加轻量级`static_pool`来维护整块buffer的内存分配，适合局部管理部分内存，pool虽然也能维护，但是底层基于`large_pool`，比较重量级，适合全局管理内存
* 增加stream快速读取全部数据到string的接口
* 增加adler32 hash算法
* 增加`tb_memmem`接口
* 采用pcre/pcre2/posix regex实现正则表达式库

### 改进

* 优化stream，支持对字符设备文件的读写
* 修改`tb_init`接口，增加allocator自定义内存分配器参数，实现用户的侵入式内存管理
* 重构内存管理，完全采用分配器allocator模式，可以灵活切换内存管理，支持原生系统内存、静态buffer内存、内存池等各种分配方式
* 重定义assert，debug模式遇到assert直接abort执行

### Bugs修复

* 修复android下的一些bug
* 修复stream的seek问题

## v1.5.0

### 新特性

* 增加跨平台环境变量操作接口

### 改进

* 重建整个编译架构，采用xmake跨平台自动构建工具进行构建。。
* 优化.pkg的依赖包机制，支持依赖库和接口的自动检测，针对libc、libm优先使用自动检测到的系统库接口实现，如果当前平台没有实现则使用tbox的自己实现版本，使得最大化性能和跨平台性。。
* 完善和优化路径操作，增加相对路径、绝对路径的相互转换

### Bugs修复

* 修复strlcpy等一些libc接口的实现bug

## v1.4.8

### 新特性

* 新增路径操作接口，支持相对路径、绝对路径相互转换

### 改进

* 重建整个makefile架构，采用*.pkg依赖包模式模块化对第三方库的依赖，降低耦合
* 默认编译配置可以自动探测当前平台支持的依赖包，注：所有依赖包都是可选的，如果要最小化编译，可以完全禁用
* 编译生成的所有库和头文件，也都安装成独立*.pkg格式，方便集成到其他开发平台，也方便copy
* 增强object路径解析接口，支持json, xml宏路径解析，并增加实用json解析工具：jcat
* 实现通用ipaddr结构，统一接口，全面支持ipv6/ipv4，stream/http的url也完全支持ipv6格式解析
* 重命名hash为`hash_map`，并新增`hash_set`容器

## v1.4.7

### 改进

* 增强fixed16定点类型的接口，优化部分接口性能，调试模式下增加更多的溢出检测
* 优化整数平方根的实现，增加对64位整数平方根的快速计算

### Bugs修复

* 修复string空字符串bug
* 修复windows下asio的一些bug
* 修复一些编译问题

## v1.4.7_rc1

### 新特性

* 增加asio模块，支持各种异步socket/file操作，支持异步dns、ssl（依赖polarssl/openssl）、http
* 增加http cookie支持，完善http客户端协议
* 增加sql数据库模块，依赖sqlite3/mysql
* 增加object模块
* 新增min/max heap容器，新增`list_entry`、`single_list_entry`等外置轻量链表实现，和`list`、`single_list`不同的是，不需要维护内部内存，而且更加灵活，新增bloom_filter
* 新增remove、walk、count、for等常用算法支持
* 新增线程池、定时器、信号量、自旋锁、atomic64等常用系统操作
* 新增http服务器、http爬虫、http下载器等实用性demo

### 改进

* 重构stream模块，并新增`async_stream`、`async_transfer`、`transfer_pool`等新特性。
* 优化和完善libc、libm的接口
* 重构整个内存管理架构，完善内存检测的支持，优化内存使用和效率

### Bugs修复

* 修复和优化xml解析模块
