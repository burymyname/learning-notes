# TCP 协议详解

本章讨论 4 个方面：
- TCP 头部信息
- TCP 状态转移过程：状态机
- TCP 数据流：交互数据流和成块数据流
- TCP 数据流控制：内核需要对 TCP 数据流进程控制，包括超时重传和拥塞控制

## TCP 服务特点

- TCP：面向连接，字节流，可靠传输

面向连接

- 通信双方必须建立连接，然后再开始传输，全双工信道，完成后需要断开连接。
- 一对一，UDP适合广播多播
- TCP 发出的报文个数和应用执行的写操作数之间没有固定数量关系
- 发送和读取先放入缓冲区，可以一次读写，也可以分多次读写，取决于缓冲区大小。
- UDP是没有发送和接受的缓冲区的，必须及时读取，否则会丢包，且没有足够额应用程序缓冲区来读取，会被截断。

可靠

- 超时重传：发送报文后需要得到接收方的应答，如果在定时内没有收到，会重新发送报文。

## TCP 头部结构

### TCP 固定头部结构

- 头部结构
    ```
    +-------------------------+----------------------+
    |     16 bit src port     |   16 bit dest port   |
    +-------------------------+----------------------+
    |                 32 bit seq num                 |
    +------------------------------------------------+
    |                 32 bit ack num                 |
    +------------------------------------------------+
    | len | resev |U|A|P|R|S|F|  16 bit window size  |
    +-----+-------+-----------+----------------------+
    |      16 bit CRC         | 16 bit emergency ptr |
    +-------------------------+----------------------+
    |           option, no more than 40 byte         |
    +------------------------------------------------+
    ```

- 16 bit 端口号：通信时，客户端源端口通常使用自动选择的临时端口，服务器使用知名固定服务端口号。定义在文件 `/etc/services` 文件中
- 32 bit 序号：在一次 TCP 通信过程中，某一个传输方向上，字节流每个字节的编号。初始化为一个随机值 ISN，后续的 TCP 报文序号值为 ISN 加上该报文段数据的第一个字节在整个字节流的偏移。
- 32 bit 确认号：对另一方发送的报文段的响应，其值为收到的 TCP 报文序号+1，即不仅需要携带自己的序号，还需要携带对方序号+1。
- 4 bit 头部长度：标识该 TCP 头部有多少个 32 bit 字，最大能有 2^4-1 = 15 * 4 = 60 byte
- 6 bit 标志位
    - URG：urgent ptr 紧急指针
    - ACK：表示确认号是否有效。
    - PSH：提示接收端应该立即从 TCP 接收缓冲区读取数据
    - RST：复位报文段，表示要求对方重新建立连接
    - SYN：同步报文段，表示请求建立连接，
    - FIN：结束报文段，表示通知对方本端关闭连接。

- 16 bit 窗口大小：TCP 流量控制的手段，指的是接收窗口，告诉对方本端 TCP 接收缓冲区还能容纳多少字节的数据，这样对方就能控制发送数据的速度。
- 16 bit 校验和：对 TCP 报文头部和数据的 CRC 校验和。
- 16 bit 紧急指针：内容是一个正的偏移量，是发送端向接收端发送紧急数据的方法。该值 + 序号字段值 = 最后一个紧急数据的下一个字节的序号，即是紧急指针相对当前序号的偏移量。

### TCP 头部选项

- TCP 头部的最后一个字段，是可变长的可选信息，最长40 bytes

- 典型结构
    ```
    +---------------+-----------------+---------------+
    | kind (1 byte) | length (1 byte) | info (n byte) |
    +---------------+-----------------+---------------+
    ```

- kind 说明选项的类型，有的 TCP 选项没有后面的两个字段
- length 表示选项的总长，包括 kind
- info 根据具体类型携带的信息

- TODO：kind 种类详解

## TCP 连接建立关闭

### 连接的建立和关闭

- 建立连接
    - 客户端 => 服务端，包含 SYN 标志，seq = ISN
    - 服务端 => 客户端，包含 SYN ACK 标志，seq = ISN，ack = 接收到的报文段 ISN + 1
    - 客户端 => 服务端，包含 ACK 标志，ack = 接收到的报文段 ISN + 1

- 关闭连接
    - 客户端 => 服务端 FIN seq
    - 服务端 => 客户端 ACK seq + 1
    - 服务端 => 客户端 FIN seq
    - 客户端 => 服务端 ACK seq + 1

### 半关闭状态

- TCP 是全双工的，通信的一端可以选择结束连接，但允许继续接收来自对方的数据，直到对方关闭连接。这种状态就是半关闭状态

- 判断对方是否关闭连接：`read` 系统调用返回 0，说明收到结束报文段。

### 连接超时

- 超时重连，会递增发送的时间间隔，到达限制的重连次数后放弃并通知。

## TCP 状态转移

- TODO：状态迁移图

### 状态转移总图

- 服务器
    - `listen` 系统调用进入 LISTEN 状态，被动等待客户端连接
    - 收到同步报文段，发送带 SYN 确认报文，进入 SYN_RCVD 状态
    - 收到客户端确认报文段，进入 ESTABLISHED 状态。
    - 客户端主动关闭连接，服务端接收到 FIN 报文，进入 CLOSE_WAIT 状态。等待服务器应用程序关闭连接
    - 客户端检测到客户端关闭后，发送一个 FIN 结束报文段关闭连接，进入 LAST_ACK 状态，等待客户端确认。

- 客户端
    - 客户端通过 `connect` 系统调用，主动与服务器建立连接，发送一个 SYN 同步报文段，进入 SYN_SENT 状态。
    - 如果端口不存在，或者被处于 TIME_WAIT 状态的连接占用，服务器发送一个复位报文段。则调用 `connect` 失败。
    - 如果端口存在，但 ACK 超时未收到，则调用 `connect` 失败。
    - 调用失败转为 CLOSED 状态，连接成功则进入 ESTABLISHED
    - 客户端主动关闭，发送一个 FIN 结束报文段，进入 FIN_WAIT_1 状态
    - 如果此时收到 ACK，则进入 FIN_WAIT_2，再收到 FIN，会进入 TIME_WAIT 状态。也可以直接收到同时带有 ACK + FIN 的报文，直接进入 TIME_WAIT 状态。 

### TIME_WAIT 状态

- 客户端收到服务器端 FIN 后，没有直接进入 CLOSED 状态，而是进入 TIME_WAIT 状态，需要等待 2 MSL (max segment life) 报文最大生存时间，才能完全关闭。

- TIME_WAIT 状态存在的原因：
    1. 可靠终止 TCP 连接
    2. 保证让迟来的 TCP 报文有足够时间识别并丢弃。否则，可能在关闭的连接上建立的新连接会收到旧连接的报文。

- 为什么是 2 MSL？
    - 能够确保两个传输方向上迟到的 TCP 报文已经被丢弃了。

## 复位报文段

- 复位报文段通知对方关闭或者重新建立连接

### 访问不存在端口

- 回复一个复位报文段，然后收到的一端关闭连接或重新连接

### 异常终止连接

- 发送复位报文段后，发送端所有等待发送的数据都丢弃。
- `socket` 选项 SO_LINGER

### 处理半打开连接

- 半打开状态：一端因为异常关闭终止了连接，但是对方没有收到结束报文，仍然维持这原来的连接。

## TCP 交互数据流

- TCP 携带应用数据按照长度分为：
     - 交互数据：对实时性要求高，例如 ssh
     - 成块数据：通常长度为最大允许数据长度，例如 ftp

- 延迟确认：不马上确认上次收到的数据，而是在在一段延时后查看本端是否有数据需要发送，如果有则和确认信息一起发送。

- Nagle 算法：双方最多只能发送一个未被确认的数据报，确认到达前不能发送其他 TCP 报文段。

## TCP 成块数据流

- TODO：

## 带外数据

- TODO:

## 超时重传

- 定时器：TCP 报文段第一次被发送时启动，如果超时时间内未收到接收方的应答，TCP 模块将重传报文并重置定时器

- `iperf` 监测网络状况

## 拥塞控制

### 概述

- 拥塞控制 4 部分：
    - 慢启动
    - 拥塞避免
    - 快速重传
    - 快速恢复

- `/proc/sys/net/ipv4/tcp_congestion_control` 文件指示当前机器使用的拥塞控制算法

- 发送窗口：发送端向网络一次连续写入的数据量，send window。拥塞控制最终受控的变量即发送窗口。所以 SWND 限定了发送端能连续发送的 TCP 报文段数量。

- 发送者最大段 SMSS：TCP 报文段的最大长度

- 接收方可以通过其接收通告窗口 RWND 来控制发送端的 SWND

- 发送端还引入了一个称为拥塞窗口 CWND 的状态变量。实际的 SWND 值是 RWND 和 CWND 较小值

### 慢启动和拥塞避免

- TCP 连接建立后， CWND 被设置为 Initial Window IW，大小为 2 ~ 4 SMSS。此时发送端最多能发生 IW 个字节数据。

- 此后发送端每收到一个接收端的一个确认，其 CWND 就按照如下增加：`CWND += min(N, SMSS)`。N 是本次确认中包含的之前未被确认的字节数。

- 慢启动：CWND 按照上式指数形式扩大，因为 TCP 开始并不知道网络实际情况，需要以试探的方式平滑增加

- 慢启动门限：slow start threshold size：当 CWND 超过 门限大小，TCP 拥塞控制进入拥塞避免阶段。

- 拥塞控制避免算法，让 CWND 按照线性增加，2 种实现：
    - 每个 RTT 时间内，按照上面的式子计算新的 CWND，不论该 RTT 时间诶发送端收到多少个确认。
    - 每收到一个对新数据的确认，按照如下式子更新 `CWND += SMSS * SMSS/CWND`

- 当拥塞发生时，如何判断拥塞发生：
    - 传输超时：TCP 重传定时器溢出
    - 接收到重复的确认报文

- 应对：
    - 传输超时：用慢启动和拥塞避免
    - 收到重复ACK：使用快速重传和快速恢复，如果该情况出现在定时器溢出后，也当成第一种处理

- 传输超时，则执行重传并如下调整：`ssthresh = max(FligntSize/2, 2*SMSS) CWND <= SMSS` 其中 `FlightSize` 是发生但是没有确认的字节数

### 快速重传 快速恢复

- 当发送端收到 3 个重复确认的报文，则认为发生拥塞
- 快速重传和快速恢复步骤：
    1. 当收到第 3 个重复 ACK 时，按照传输超时重新计算 `ssthresh`，然后立即重传丢失的报文段，并按照如下设置 CWND：`CWND = ssthresh + 3*SMSS`
    2. 每次收到一个重复的确认，设置 `CWND += SMSS`，此时发送端可以发送新的 TCP 报文
    3. 当收到新数据的确认，设置 CWND = ssthresh

