
##  1. <a name='chapter2'></a>第二章

总结：本章详细介绍了编译的完整过程，主要包括调用编译器的过程，以及编译的具体过程

---

<!-- vscode-markdown-toc -->
* 1. [第二章](#chapter2)
	* 1.1. [被隐藏了的过程](#Complation)
		* 1.1.1. [预编译](#Preprocess)
		* 1.1.2. [编译](#Compile)
		* 1.1.3. [汇编](#Assembly)
		* 1.1.4. [链接](#Link)
	* 1.2. [编译器做了什么](#CompilationStep)
		* 1.2.1. [词法分析](#Scanner)
		* 1.2.2. [语法分析](#Parser)
		* 1.2.3. [语义分析](#SemanticAnalyzer)
		* 1.2.4. [中间语言生成](#IR)
		* 1.2.5. [目标代码生成和优化](#Optimization)
	* 1.3. [链接器年龄比编译器长](#LinkerHistory)
	* 1.4. [模块拼装：静态链接](#StaticLink)

<!-- vscode-markdown-toc-config
	numbering=true
	autoSave=true
	/vscode-markdown-toc-config -->
<!-- /vscode-markdown-toc -->

###  1.1. <a name='Complation'></a>被隐藏了的过程

- 编译过程：
    - 预处理
    - 编译
    - 汇编
    - 链接

    ```
    +------------------------------------+
    | source code(.c) + header files(.h) |
    +------------------------------------+
    |
    +---> processing(cpp)
    |
    v
    +------------------+
    | preprocessed(.i) |
    +------------------+
    |
    +---> compilation(gcc)
    |
    v
    +--------------+
    | assembly(.s) |
    +--------------+
    |
    +---> assembly(as)
    |
    v
    +-----------------+   +----------------+
    | object file(.o) | + | static lib(.a) |
    +-----------------+   +----------------+
    |
    +---> linking(ld)
    |
    v
    +-------------------+
    | execulatble(.out) |
    +-------------------+
    ```

####  1.1.1. <a name='Preprocess'></a>预编译

- C: `.c -> .i`, C++: `.cpp -> .ii`
- 命令
    ```
    $gcc -E hello.c -o hello.i
    $cpp hello.c > hello.i
    ```

- 预编译主要处理 `#` 开头的预编译指令：
    - 删除 `#define` 展开宏定义
    - 处理条件编译
    - 处理 `#include`，将被包含的文件插入到该预编译指令的位置，这个过程是递归的
    - 删除注释
    - 添加行号和文件名标识，以便编译器产生调试用的debug信息
    - 保留所有的 `#pragme` 指令，因为编译器需要使用

    经过预编译后的`.i`文件宏定义都已经展开，并且包含的文件已经被插入到文件中。

####  1.1.2. <a name='Compile'></a>编译
- 编译过程：词法分析，语法分析，语义分析，优化。生成汇编代码
    ```
    $ gcc -S hello.i -o hello.s
    ```
    编译器把预编译和编译合并为一个步骤，对于C来说，程序为 `cc1`，C++是 `cc1plus`

####  1.1.3. <a name='Assembly'></a>汇编
- 汇编：将汇编指令转为机器码
    ```
    $ as hello.s -o hello.o
    $ gcc -c hello.s -o hello.o
    ```

####  1.1.4. <a name='Link'></a>链接
- 链接：将目标文件和库链接起来，才能得到可执行文件

###  1.2. <a name='CompilationStep'></a>编译器做了什么

- 编译过程：词法分析(扫描)，语法分析，语义分析，源代码优化，代码生成，目标代码优化
    ```
    +-------------+
    | Source Code |
    +-------------+
    |
    +---> scanner
    |
    v
    +--------+
    | Tokens |
    +--------+
    |
    +---> parser
    |
    v
    +-------------+
    | Syntax Tree |
    +-------------+
    |
    +---> semantic analyzer
    |
    v
    +-----------------------+
    | Commented Syntax Tree |
    +-----------------------+
    |
    +---> source code optimizer
    |
    v
    +------------------------------+
    | intermediate respresentation |
    +------------------------------+
    |
    +---> code generator
    |
    v
    +-------------+
    | target code |
    +-------------+
    |
    +---> code optimizer
    |
    v
    +-------------------+
    | final target code |
    +-------------------+

    ```

####  1.2.1. <a name='Scanner'></a>词法分析
- 扫描器：scanner，进行词法分析，有限状态机的算法，分割源码为token
- 记号：token，主要为关键字，标识符，字面量，特殊符号
- 预处理不归编译器的范围，由预处理器处理
- `lex`可以根据制定的词法规则进行词法分析

####  1.2.2. <a name='Parser'></a>语法分析
- 语法分析器：parser，对token进行语法分析，产生语法树，采用上下文无关文法
- 语法树：以表达式为节点的树，符号和数字是最小的表达式
- `yacc`编译器编译器，只需要改变语法规则，无需重写parser

####  1.2.3. <a name='SemanticAnalyzer'></a>语义分析
- 语义分析器：semantic analyzer，只能分析静态语义
- 静态语义：在编译期可以确定的语义，包括声明和类型的匹配
- 有些类型需要做隐式转换，语义分析程序会在语法树中插入相应的转换节点

####  1.2.4. <a name='IR'></a>中间语言生成
- 源码优化是在IR层面做的
- 三地址码，P代码(P-code)

####  1.2.5. <a name='Optimization'></a>目标代码生成和优化
- 后端：包括代码生成和优化

###  1.3. <a name='LinkerHistory'></a>链接器年龄比编译器长

- 重定位：重新计算各个目标的地址，如果程序经过修改，需要重定位
- 符号：用来表示一个地址，可以是函数，也可以是变量
- 模块：由多个函数变量组成的文件
- 不同模块之间的通信：模块间的符号引用
    - 函数调用
    - 变量访问

- 链接：将编译后的模块链接到一起，产生可执行文件

###  1.4. <a name='StaticLink'></a>模块拼装：静态链接

- 链接过程包括：
    - 地址和空间分配
    - 符号决议/绑定
    - 重定位

- 重定位入口：每一个要被重定位的地方



