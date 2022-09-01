## 第五章 Windows PE

### PE

- 目标文件仍然是 COFF，可执行文件为 PE


### PE 前身：COFF

- COFF 文件结构
 - 映像头(Image Header)
 - 段表(Section Table)
 - 段(Section) ...
 - 符号表

- PE文件又叫映像文件


### 链接指示信息

### 调试信息

### 符号表

### WIndows下的ELF：PE

- PE 是 COFF 的扩展，主要变化有两个：
    - 文件开始部分不是COFF文件头，是DOS MZ 可执行文件格式的文件头和桩代码
    - COFF文件头结构 `IMAGE_FILE_HEADER` 部分扩展为 `IMAGE_NT_HEADERS`，这个结构包含了原来的Image Header以及新增的PE扩展头部结构

