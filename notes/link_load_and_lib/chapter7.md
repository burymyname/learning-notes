## 动态链接

### 为什么要动态链接

- 浪费内存和磁盘空间，共享内容可以使用一个
- 程序更新发布需要重新静态链接，太繁琐

- 动态链接：运行时链接
- 减少空间，升级方便，可扩展和兼容

- 共享对象：动态共享对象，以`.so`为扩展名，Windows中叫动态链接库，dll

### 简单例子

- module：模块，动态链接下可执行文件和共享对象都可以看作程序的一个模块。

- 引用符号，如果是静态链接，会对符号重定位，如果是动态链接，则标记为动态链接的符号，不进行重定位，等到装载时再进行。

动态链接程序运行时地址空间分布
- 除了程序，还有动态链接库，还有动态链接器
- 共享对象的最终装载地址是不确定的


### 地址无关代码

#### 固定装载地址的困扰
- 任意地址加载：不假设共享对象在进程虚拟地址空间中的位置。

#### 装载时重定位
- 编译时只使用 -shared
- 装载时重定位不能被多个进程共享

#### 地址无关代码
- 地址无关代码：把指令中需要修改的部分分离，与数据部分放在一起，指令部分保持不变，每个进程中有一个副本。
- fPIC
- 把模块内的符号分类
    1. 模块内部的函数调用跳转
    2. 模块内部的数据访问
    3. 模块外部的函数调用跳转
    4. 模块外部的数据访问
- 1.模块内部的函数调用跳转：相对地址跳转
- 2.模块内部的数据访问：PC + 偏移
- 3.模块外部的数据访问：
    - GOT：全局偏移表，数据段中建立一个指向变量的指针数组。通过GOT表项间接引用
    - 链接器装载模块的时候查找变量所在地址，填充GOT表。
- 4.模块外部的函数调用跳转：通过GOT表项间接跳转
- fPIE：地址无关可执行文件

#### 共享模块的全局变量问题

- 定义在模块内部的全局变量：当一个模块引用了一个定义在共享对象中的全局变量的时候。编译时无法确定是否跨模块调用。  
在.bss中创建一个变量副本，将使用这个变量的指令都指向可执行文件中的副本，默认定义为在其他模块的全局变量。通过GOT表实现访问。
- 如过全局变量在可执行文件中有副本，链接器就把GOT中的地址指向该副本，如果变量在共享模块中初始化，则动态链接器还要把该值复制到程序的变量副本中。

#### 数据段地址无关性

### 延迟绑定(PLT)

- 动态链接比静态链接稍慢一些，主要因为动态链接通过GOT表定位，间接寻址。以及程序执行时，动态链接器都需要进行一次链接工作。

- 延迟绑定实现：函数被第一次用到了才绑定，通过PLT实现

- PLT实现  
每个外部函数在PLT中都有一个对应的项xxx@plt，

### 动态链接相关结构

- 映射完成后，OS先启动动态链接器ld.so

#### interp 段
- interp段中保存一个字符串，可执行文件所需要的动态链接器的路径。

#### dynamic 段
- dynamic段保存了动态链接器所需要的基本信息。
- ldd xxxx 查看程序依赖于哪些共享库

#### 动态符号表
- 动态符号表：dynsym，只保存动态链接相关符号。symtab保存的是所有符号。

- 导入函数，导出函数

#### 动态链接重定位表
- rel.dyn：对数据引用的修正，修正位置在got和数据段
- rel.plt：对函数引用的修正，修正位置在got.plt

#### 动态链接时进程堆栈初始化信息
- 辅助信息数组：结构数组，类型+值。位于环境变量指针的后面


### 动态链接的步骤和实现

#### 动态链接器自举


#### 装载共享对象


#### 重定位和初始化

### 显式运行时链接
