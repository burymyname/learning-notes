# 简介和概述

## 内核的任务

- 硬件与软件之间的中间层，充当底层驱动程序

## 实现策略

- 两种范型：
    - 微内核：只有最基本的功能直接由中央内核实现，其他功能都委托给独立进程，这些进程通过明确定义的通信接口与中心内核通信。
    - 宏内核：内核的全部代码被打包到一个文件中，包括所有子系统(内存管理，文件系统，设备驱动)。内核的每个函数都可以访问内核的其他部分。(有什么问题？可能会导致源码中复杂的嵌套)
    - 目前 Linux 采用的是宏内核范型，但是引入模块热拔插和动态装载。

## 内核的组成部分

![kernel components](../../images/linux_kernel_1.png)

### 进程 进程切换 调度

- 进程切换：撤销进程的 CPU 资源之前，需要先将与进程相关的状态都保存，并将进程置于空闲状态。重新激活进程的时候，再将保存的状态恢复。

- 调度：决定如何在现有进程之间分配 CPU 时间

### UNIX 进程

- Linux 对进程采用一种**层次系统**，每个进程都依赖于一个父进程。
- 内核启动 `init` 程序作为第一个进程，该进程负责进一步的系统初始化操作。因此 `init` 是进程树的根，所有进程都直接或者间接起源于该进程。
- `pstree` 以树的形式列出进程分支。(当前列出来树根不是 `init` 而是 `systemd`，经过查阅，目前广泛被各大 Linux 发行版采用的 `Init` 进程是 `Systemd`。`Systemd` 作为系统启动和管理的一整套解决方案，取代了 `init`，成为系统的 1 号进程，其他的进程都是其子进程)

<br>

- UNIX 中创建新进程的两种机制：
    - `fork` 创建当前进程的一个副本，父子进程之间只有 `PID` 不同。linux 采用 COW ，将内存的复制延迟到写入数据的时候，只读访问时可以共用同一页内存，可以提高效率。
    - `exec` 将一个新的程序加载到当前进程的内存中执行，将内存内容替换为新的数据，然后开始执行新程序。

<br>

- 线程：
    - UNIX 进程，又称重量级进程。
    - 线程又称轻量级进程。
    - `clone` 创建线程

<br>

- 命名空间：
    - 以前的全局资源，现在具有不同分组，每个命名空间可以包含一个特定的 PID 集合，或者可以提供文件系统的不同视图。
    - 用处：对于虚拟机，通过为容器的命名空间来建立系统的多个视图，容器彼此分离。

### 地址空间和特权级别

- 虚拟地址空间：与实际物理内存容量无关，由 CPU 字长决定
- Linux 将虚拟地址空间分为两部分：内核空间和用户空间
- 用户空间：`0 ~ TASK_SIZE`，内核空间：`TASK_SIZE ~ 2^32 或 2^64`

<br>

- 特权级别
    - Linux 只使用两种不同状态：核心态和用户态。区别在于对内核空间的访问
    - 用户态禁止访问内核空间
    - 从用户态到核心态切换需要通过系统调用。内核可以访问虚拟地址空间的用户部分
    - 内核还可以由异步硬件中断激活，然后在中断上下文进行。与在进程上下文运行的区别值，中断上下文中运行不能访问虚拟地址空间中的用户空间部分。(因为硬件中断随时可能发生，所以进程与硬中断的原因无关，内核无权访问用户空间)
    - `ps fax` 查看进程，可以识别内核线程

<br>

- 虚拟和物理地址空间
    - 虚拟地址被分为等长的单位**页**，然后映射到物理内存页上。

### 页表

- 页表：将虚拟地址空间映射到物理地址空间的数据结构，使用数组，对虚拟地址空间中的每一页，都分配一个数组项。
- IA-32 下页大小为 4K，虚拟地址空间有 4G，需要包含 100 万项的数组。且每个进程有自己的页表。
- 多级分页：将虚拟地址划分为多个部分，Linux 采用 4 级页表

<br>

- 全局页目录(PGD)：虚拟地址的第一部分。
- 中间页目录(PMD)：第二个部分
- 页表数组(PTE)：页表的索引
- 偏移量：页内部位置。

<br>

- 对于虚拟地址中不需要的区域，不必创建中间页目录或者页表，节省内存空间。

<br>

- MMU：内存管理单元，用于转换虚拟地址到物理地址
- TLB：地址转换缓存，用于保存地址转换中出现最频繁的地址

<br>

- 与 CPU 的交互：IA-32 只使用了 2 级页表，64 位体系结构地址空间较大，需要 3,4 级的页表。

<br>

- 内存映射：内核中大量使用的抽象手段
    - 可以像普通内存一样访问。
    - 例如文件映射到内存，可以读取相应内存来访问文件。
    - 驱动程序使用内存映射，外设的 I/O 可以映射到虚拟地址空间区域。

### 物理内存的分配

- 内核分配只分配完整的页帧，将内存划分为更小的部分，由**用户空间的标准库完成**

<br>

- 伙伴系统：快速检测内存的连续区域，用户分配连续页
    - 系统中的两个伙伴是空闲的，则会合并为更大的内存块，作为下一层次上某个内存块的伙伴。
    - 内核对所有大小相同的伙伴都放置到一个同一个列表中管理。
    - 分配的时候，进行分裂
    - 释放内存的时候，检查地址，判断是否能创建一组伙伴，合并为一个更大的内存放回到伙伴列表中

<br>

- slab 缓存：内核需要在伙伴系统基础上，定义额外的内存管理层，将伙伴系统提供的页划分为更小的部分
    - 对频繁使用的对象：内核定义了只包含所需对象实例的缓存。
    - 对通常情况下小内存块的分配：定义一组slab缓存，用相同函数访问，`kmalloc`和`kfree`

<br>

- 页面交换和页面回收
    - 页面交换：通过利用磁盘作为扩展空间，增大可用的内存。
    - 缺页异常：换出的页可以通过特别的页表项标识。在进程试图访问此类页帧时，CPU则启动一个可以被内核截取的缺页异常。此时内存可以将硬盘上的数据切换到内存中。接下来用户进程可以恢复运行

### 计时

- 进程调度就需要计时
- `jiffies` 全局变量，会按恒定的时间间隔进行递增。
- 底层通常是定时器中断。

### 系统调用

- 系统调用是用户进程和内核交互的经典方法。
- 传统系统调用分组：
    - 进程管理：创建新进程，查询，调试
    - 信号：发送信号，定时器
    - 文件：具体是文件内容处理
    - 目录和文件系统：创建删除目录，变更目录信息
    - 保护机制：读取和变更UID/GID，命名空间的处理
    - 定时器函数和统计信息

<br>

- 在发出系统调用时，处理器必须改变特权级别，从用户状态切换到核心态

### 设备驱动 块设备 字符设备

- 外设可以分为两类：
    - 字符设备：连续的数据流，可以顺序读取，通常不支持随机存取。例如调制解调器。
    - 块设备：可以随机访问设备数据，可以自行确定读取数据的位置。例如磁盘。
- 块设备的驱动比字符设备复杂。

### 网卡

- Linux 用套接字抽象，支持通过文件接口处理网络连接。

### 文件系统

- VFS：虚拟文件系统，内核需要提供一个额外的软件层，将底层文件系统和应用层以及内核自身隔离。

### 模块 热拔插

- 模块用于在运行时动态地向内核添加功能。
- 对支持热插拔而言，模块在本质上是必需的

### 缓存

- 页缓存

### 链表处理

- 双向链表

### 对象管理和引用计数

- TODO

### 数据类型

- 类型定义：用 `typedef` 来定义，避免依赖体系结构。
- 如果某个变量类型是 `typedef` 而来，则不能直接访问，需要通过辅助函数

- 字节序

- pre-cpu 变量

- 访问用户空间：`__user` 内核使用该记号标识指向用户地址空间中区域的指针。

## 小结

- 本章主要是介绍了内核的全景，阐述各个部分的职责，处理问题，交互方式。
