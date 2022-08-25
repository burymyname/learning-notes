## 第四章 静态链接

- 链接：两个目标文件如何链接成一个可执行文件。

### 空间和地址分配

- 多个目标文件的各个段如何合并？

#### 按序叠加
- 问题：输出文件会有成百上千的零碎的段，浪费空间。且因为需要对齐，会存在大量的内存碎片。

#### 相似段合并
- `.bss` 段在目标文件和可执行文件中不占文件空间，但是在装载的时候占用地址空间。
- 地址和空间：有两层含义，1）可执行文件的空间。2）装载后的虚拟地址空间。
- 对于有实际数据的段，例如代码段和数据段，两者都要分配。但对于 `.bss` 段，分配空间只局限于虚拟地址空间。

- 两步链接：
    - 空间和地址分配  
    扫描所有输入目标文件，获取各个段的长度、属性和位置，并将输入目标文件中的符号表中的符号定义和符号引用收集，统一放到全局符号表(GOT表)。  
    链接器能获取所有目标文件的段长度，合并，计算出合并后段的长度和位置，建立映射关系。
    - 符号解析和重定位  
    使用第一步收集到的信息，读取输入文件中段的数据，重定位信息，并进行符号解析和重定位，调整代码地址。

- `$ ld a.o b.o -e main -o ab`  
    - `-e main` 表示main函数作为程序入口，链接器默认程序入口味 `_start`
    - `-o ab` 表示链接输文件名，默认为 `a.out`

- 