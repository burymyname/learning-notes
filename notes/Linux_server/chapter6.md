# 高级 I/O 函数

- 网络编程相关的 I/O 函数
    - 创建文件描述符：pipe dup/dup2
    - 读写数据：readv/writev sendfile mmap/munmap splice tee
    - 控制 I/O 行为和属性：fcntl

## pipe

- pipe 用于创建管道，实现进程间通信
    ```
    int pipe(int fd[2]);
    ```

- pipe 的参数是一个包含两个 int 类型的数组指针，成功时返回 0，并将打开的文件描述符填入参数指向的数组，失败返回 -1 并设置 errno

- `fd[2]` 分别构成管道的两端，fd[1] 只能写，fd[0] 只能读。需要双向需要两个管道。

- 默认下文件描述符都是阻塞的，如果用 read 来读取一个空的管道，则 read 会阻塞直到管道内有数据可读。类似如果用 write 来写一个满的管道，也会阻塞直到管道有足够空闲

- 管道内部传输的是字节流。管道本身有容量大小限制。

- socketpair 可以创建本地双向管道

## dup/dup2

- 输入重定向到一个文件，或者输出重定向到一个网络连接，可以用用于复制文件描述符的 dup 和 dup2
    ```
    int dup(int file_descriptor);
    int dup2(int file_descriptor_one, int file_descriptor_two);
    ```

- dup 函数创建一个新的文件描述符，与原文件描述符指向同一个文件/管道/网络连接，并且文件描述符总是取当前可用的最小整数值。

- dup2 函数与 dup 类似，不过将返回第一个不小于 two 的整数值

## readv writev

- 分散读和集中写
    - readv 函数将数据从文件描述符读取到分散的内存块
        ```
        ssize_t readv(int fd, const struct iovec* vector, int count);
        ```
    - writev 函数将多块分散的内存数据一并写入文件描述符中
        ```
        ssize_t writev(int fd, const struct iovec* vector, int count);
        ```

- iovec 是一块内存区

## sendfile

- 在内核中操作，在两个文件描述符之间直接传递数据，避免内核缓冲区和用户缓冲区之间的拷贝。
    ```
    ssize_t sendfile(int out_fd, int in_fd, off_t* offset, size_t count);
    ```

- in_fd 必须是真实的文件，out_fd 必须是一个 socket

## mmap munmap

- mmap 用于申请一段内存空间，可以作为进程间通信的共享内存，也可以将文件直接映射到其中
    ```
    void* mmap(void* start, size_t length, int prot, int flags, int fd);
    ```

- prot 设置内存段的访问权限，可以取以下几个值的按位或
    - PROT_READ
    - PROT_WRITE
    - PROT_EXEC
    - PROT_NONE

- munmap 用于释放 mmap 创建的内存。

## splice

- 在两个文件描述符之间移动数据，也是零拷贝

- fd_in 和 fd_out 至少一个为管道

## tee

- 在两个管道文件描述符之间赋值数据，零拷贝，与 splic 相同。

## fcntl



