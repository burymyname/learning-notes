# Linux network API

- socket 地址 API
- socket 基础 API
- 网络信息 API

## socket 地址 API

### 主机字节序 网络字节序

- 大端：高放低
- 小端：高放高
- 现代多采用小端，小端又被称为主机字节序
- 网络字节序是大端

- 主机序和网络序转换函数
    ```
    #include <netinet/in.h>
    htonl
    htons
    ntohl
    ntohs
    ```
### 通用 socket 地址


## 接收连接

## 发起连接

## 关闭连接

## 数据读写

### TCP 数据读写

- 对文件的读写 `read` 和 `write` 同样适用于 socket。
- 也有专门的接口：
    ```
    ssize_t recv()
    ssize_t send()
    ```



### UDP 数据读写

- 用于 UDP 的系统调用
    ```
    ssize_t recvfrom()
    ssize_t sendto()
    ```

### 通用数据读写函数

- 通用的读写系统调用
    ```
    recvmsg()
    sendmsg()
    ```

## 带外标记

- sockatmark

## 地址信息函数

- 获取本端 socket 地址：`getsockname`
- 获取远端 socket 地址：`getpeername`

## socket 选项

- 专门用于读取和设置 socket 文件描述符属性的方法
    ```
    int getsocketopt
    int setsocketopt
    ```

### SO_REUSEADDR

- TCP 中 TIME_WAIT 状态，服务器可以通过设置 socket SO_REUSEADDR 选项强制使用被处于 TIME_WAIT 状态的连接占用的 socket 地址

### SO_RCVBUF 和 SO_SNDBUF

- SO_RCVBUF 和 SO_SNDBUF 分别表示 TCP 接收和发送缓冲区的大小

- 通过设置选项设置缓冲区大小时，系统会将其值加倍，并不得小于某个最小值

### SO_RCVLOWAT 和 SO_SNDLOWAT

- 表示 TCP 接收和发送缓冲区的低水位标记，一般被 I/O 复用系统调用，用来判断 socket 是否可读或可写

- 默认均为 1 byte

### SO_LINGER

- 用于控制 close 系统调用在关闭 TCP 连接时的行为，默认用 close 系统调用来关闭一个 socket 时，close 立即返回，TCP 模块负责把 socket 对应的发送缓冲区残留数据发送给对方

- `linger` 结构体

## 网络信息 API

- ip 地址和端口号不方便记忆，调用网络信息 API 实现主机名到 IP 地址的转换

### getthostbyname gethostbyaddr

- 根据主机名获取主机完整信息
- 根据 ip 地址获取主机完整信息

### getservbyname getservbyport

- 根据名称获取服务完整信息
- 根据端口号获取服务完整信息

### getaddrinfo

- 通过主机名获取 IP 地址和端口号

