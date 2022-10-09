# I/O 复用

- Linux 下实现 I/O 复用的系统调用主要有 select poll epoll

## select

- 在一段时间内，监听用户感兴趣的文件描述符上的可读可写和异常事件。

### select API

- 原型
    ```
    include <sys/select.h>
    int select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, struct timeval* timeout)
    ```

- nfds 参数：指定被监听的文件描述符总数，文件描述符从 0 开始计数，所以该值通常为最大文件描述符 + 1
- 后面三个参数分别是可读，可写，异常文件描述符集合。
- timeout 参数是设置 select 的超时时间，是一个 timeval 结构类型的指针，精确到微秒级，但是不能完全依赖该值。如果传入 0 则立即返回，如果传入 NULL 则一直阻塞直到某个文件描述符就绪

- select 成功时返回就绪文件描述符总数，如果超时时间内没有文件就绪，返回 0，失败时返回 -1 并设置 errno

### 文件描述符就绪条件

- 可读

- 可写

### 处理带外数据


## poll

- 在指定时间内轮询一定数量的文件描述符，测试其中是否有就绪

- 原型
    ```
    #include <poll.h>
    int poll(struct pollfd* fds, nfds_t nfds, int timeout)
    ```

- fds 参数是一个 pollfd 结构类型的数组，指定文件描述符上发生的可读可写异常等事件

- event 可以自己指定

## epoll

### 内核事件表

- epoll 是 Linux 特有的 I/O 复用函数，在实现和使用上与 select poll 有很大差异。

- epoll 使用一组函数来完成任务，而不是单个函数，把用户关心的文件描述符上的事件放在内核里的一个事件表中，无须像 select 和 poll 那样每次调用重复传入描述符集合。

- epoll 需要一个额外的文件描述符来唯一标识内核中的这个事件表
```
#include <sys/epoll.h>
int epoll_create(int size);
```
该函数返回的文件描述符将用作其他所有 epoll 系统调用的第一个参数


### epoll_wait

- epoll_wait 是主要接口，在一段超时时间内等待一组文件描述符上的事件

- 返回就绪的文件描述符个数


### LT 和 ET 模式

- epoll 对文件描述符的操作有两种：
    - LT(level trigger 电平触发)模式：默认工作模式，
    - ET(edge trigger 边缘触发)模式：内核事件表注册一个 EPOLLET 事件，epoll 的高效工作模式

- LT 模式：当 epoll_wait 检测到文件描述符上有事件发生，并通知应用后，应用可以不立即处理，当下一次调用 epoll_wait 时，还会继续通知该事件，直到该事件被处理

- ET 模式：当 epoll_wait 检测到其上有事件发生并通知应用后，应用必须立即处理该事件，后续 epoll_wait 调用将不再向应用通知。

- ET 模式比 LT 模式降低了同一个 epoll 事件被重复触发的次数。

### EPOLLONESHOT 事件

- 避免出现两个线程同时操作一个 socket 的情况。
- 注册 EPOLLONESHOT 事件的文件描述符，操作系统最多触发其上注册的一个可读可写异常事件。
- 处理完后需要重置该事件，确保下一次能够触发。

