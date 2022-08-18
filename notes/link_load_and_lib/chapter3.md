## 第三章 目标文件

目标文件从结构上说，是还没有链接的可执行文件。所以内部有一些符号和地址没有调整。

### 目标文件的格式

- Windows 下 PE 格式，Linux 下 ELF
- COFF(Common File Format)：上述两种都是该格式的变种
- 动态链接库：Windows 下的 `.dll` 和 Linux 下的 `.so`
- 静态链接库：Windows 下的 `.lib` 和 Linux 下的 `.a`
- 动态链接库和静态链接库都是可执行文件格式

- ELF 格式文件类型

| ELF 文件类型 | 说明 | 实例 |
| --- | --- | --- |
| 可重定位文件 | 包含数据和代码，可以被链接为可执行文件或共享目标文件，静态链接库也属于这类 | `.o`, `.obj` |
| 可执行文件 | 直接可以执行的程序 | `.exe`, 无扩展名 |
| 共享目标文件 | 包含代码和数据，链接器可以使用其与其他可重定位文件和共享目标文件链接，产生新的目标文件，或者是动态链接器可以将几个共享目标文件与可执行文件结合，作为进程映像的一部分运行 | `.so`, `.dll` |
| 核心转储文件(core dump) | 进程以外终止的时候，系统将进程地址空间的内容和一些其他信息保存下来，没有代码段| core dump 文件|

### 目标文件是是什么样的

- section/segment：节/段
- 代码段(code section)：`.code`, `.text`
- 数据段(data section)：`.data` 

- 段表(Section table)：描述文件中各个段的数组，段表描述了文件中各个段在文件中的偏移位置和段的属性等
- 初始化的全局变量和局部静态变量在 `.data` 段
- 未初始化的全局变量和局部静态变了在 `.bss` 段，未分配内存，只预留了位置
- BSS(Block Started by Symbol)：用于定义符号并且为该符号预留给定数量的未初始化空间

- 为什么要分数据和代码在不同的段？  
    - 代码只读，数据可读可写，可以设置权限防止恶意改写程序指令
    - 有利于提高cache命中率
    - 多个进程可以共享指令这样的只读数据

### 挖掘SimpleSection.o

- 只编译不链接
    ```
    $ gcc -c SimpleSection.c
    ```

- `objdump` 查看内部结构
    ```
    $ objdump -h SimpleSection.o # 查看各个段基本信息
    ```

#### 代码段
- `objdump -s` 将所有段内容以 hex 打出来
- `objdump -d` 将所有包含指令的段反汇编

#### 数据段和只读数据段
- `.data` 段保存的是已经初始化的全局静态变量和局部静态变量
- `.rodata` 段保存的是只读数据，一般是程序中的只读变量，例如const，字符串常量

有时候编译器会把字符串常量放在 `.data` 段中

#### BSS段
- `.bss` 段存放是的未初始化的全局变量和局部静态变量
- 编译单元内部可见的静态变量是存放在 `.bss` 段中的

- 如果一个静态变量初始化为0，则会被放在 `.bss` 段中，其余会放在 `.data` 中，因为未初始化默认是0，在这里会被优化

#### 其他段
- 如果要将一个二进制文件作为目标文件中的一个段：
    ```
    $ objcopy -I binary -o elf32-i386 -B i386 image.jpg image.o
    ```

- 自定义段  
指定编译器将变量或者代码放到指定的段中
    ```
    __attribute__((section("FOO"))) int global = 42;
    __attribute__((section("BAR"))) void foo()
    ```

### ELF 文件结构描述

- 头部是 ELF 文件头，包含描述整个文件的基本属性。紧接是 ELF 文件各个段(Section)，后面是段表(Section Header Table)

#### 文件头
- `readelf -h` 读取 ELF 文件头
- ELF 文件头内容
    - Magic：7F 45 4C 46
    - 文件机器字节长度
    - 数据存储方式
    - 版本
    - 运行平台
    - ABI版本
    - ELF重定位类型
    - 硬件平台版本
    - 入口地址
    - 程序头入口和长度
    - 段表位置和长度
    - 段的数量

    以32位版本文件头结构为例
    ```
    template<int size>
    struct Ehdr_data
    {
    unsigned char e_ident[EI_NIDENT]; // 16
    Elf_Half e_type;    // 2
    Elf_Half e_machine; // 2
    Elf_Word e_version; // 4
    typename Elf_types<size>::Elf_Addr e_entry; // 4/8
    typename Elf_types<size>::Elf_Off e_phoff;  // 4/8
    typename Elf_types<size>::Elf_Off e_shoff;  // 4/8
    Elf_Word e_flags;       // 4
    Elf_Half e_ehsize;      // 2 elf header size 
    Elf_Half e_phentsize;   // 2 prog header entry size
    Elf_Half e_phnum;       // 2 prog header entry num
    Elf_Half e_shentsize;   // 2 sect header entry size
    Elf_Half e_shnum;       // 2 sect header num
    Elf_Half e_shstrndx;    // 2 sect header string table index
    };

    ```

| 成员 | readelf 输出与含义 |
| --- | --- |
| e_indet | 对应 Magic 到 ABI Version，包括 Magic，字长类型，大小端，ELF版本，ABI版本 |
| e_type | REL (Relocatable file) ELF 文件类型 |
| e_machine | Advanced Micro Devices X86-64 ELF 文件的CPU平台属性，相关常量以 EM 开头
| e_version | Version: 0x1 ELF 版本号，一般为1 |
| e_entry | Entry point address: 0x0 入口地址，规定 ELF 程序的入口虚拟地址，可重定位文件一般没有入口地址，所以为0 |
| e_phoff | Start of program headers: 0 (bytes into file) 段表偏移量 |
| e_shoff | Start of section headers: 239640 (bytes into file) 节表偏移量 |
| e_flag | Flags: 0x0 ELF 标志位，用来标识一些 ELF 文件平台相关的属性 |
| e_ehsize | Size of this header: 64 (bytes) ELF 文件头本身大小 |
| e_phentsize | Size of program headers: 0 (bytes) 文件段表描述符的大小 |
| e_phnum | Number of program headers: 0 段表中段描述符个数 |
| e_shentsize | Size of section headers: 64 (bytes) 节表描述符的大小 |
| e_shnum | Number of section headers: 29 节描述符个数 |
| e_shstrndx | Section header string table index: 1 节表字符串表所在的节的index |

- ELF Magic：前16字节对应 `e_ident` 成员，用来标识 ELF 文件的平台属性。
    ```
    Magic:   |7f 45 4c 46| 02| 01| 01| 00 00 00 00 00 00 00 00 00
    Class:                             ELF64
    Data:                              2's complement, little endian
    Version:                           1 (current)
    OS/ABI:                            UNIX - System V
    ABI Version:                       0
    ```
    
    - 0 - 3 字节是 ELF 文件必须的标识码，`0x7F 0x45 0x4C 0x46`，是第 0 个对应 DEL 控制符，后面 3 字节对应 `ELF` 字符
    - 接下来第 4 字节是标识 ELF 文件类，0x01 表示 32 位，0x02 表示 64 位。
    - 第 5 字节是 ELF 文件大小端
    - 第 6 字节是 ELF 文件主版本号，一般是 1，因为后面再也没更新过
    - 后面的 9 byte 没有定义，一般填 0。

- 文件类型：e_type，一般有 3 种
    - `ET_REL 1` 可重定位文件，.o
    - `ET_EXEC 2` 可执行文件
    - `ET_DYN 3` 共享目标文件，.so

- 机器类型：e_machine，表示 ELF 文件的平台属性
    - `EM_M32 1` 
    - `EM_SPARC 2`
    - `EM_386 3` Intel x86
    - `EM_68K 4`
    - `EM_860 5`

#### 节表
- 节表：Section Header Table 编译链接靠节表来定位和访问各个节的属性
- `readelf -S xxx` 读取节表内容
- ELF 节表第一个元素是无效的描述符，类型为 NULL
