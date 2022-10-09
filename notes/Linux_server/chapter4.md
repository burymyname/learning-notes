# 实例 访问 web 服务器

- 通过访问 web 服务器，分析一个完整的 TCP/IP 实例

## 代理服务器

### http 代理服务器工作原理

- 分类：
    - 正向代理：客户端自己设置代理服务器地址
    - 反向代理：设置在服务端，用来接收连接请求，转发给内部网络服务器，对外表现为真实服务器。
    - 透明代理：只能设置在网关上，一种特殊的正向代理

- TODO：代理图

## 访问 DNS 服务器

- 读取 `/etc/resolv.conf` 文件获取 DNS 服务器 IP 地址，然后 UDP 模块将 DNS 查询报文封装为 UDP 数据报，同时把源端口和目标端口加入 UDP 数据报头部

- UDP 调用 IP 服务，IP 将 UDP 数据报封装为 IP 数据报，把源 IP 地址和 DNS 服务器 IP 地址放入数据报头部。查询路由选择如何发送 IP 数据报

- 如果 ARP 缓存中没有对应的缓存项，则发起 ARP 广播查询 IP 地址。

## 本地名称查询

- 用域名访问主机，需要 DNS 服务器获取 IP 地址
- 通过主机访问本地局域网上机器，可以通过本地静态文件来获取机器 IP 地址，在 `/etc/hosts` 文件中

- 可以通过修改 `/etc/host.conf` 文件来自定义系统解析主机名的方法和顺序。一般是先访问本地 host 文件，再访问 DNS 服务

## HTTP 通信

### HTTP 请求

- http 请求内容
    ```
    GET http://www.baidu.com/index.html HTTP/1.0
    User-Agent: Wget/1.12 (linux-gnu)
    Host: www.baidu.com
    Connection: close
    ```
- 第一行是请求行，GET 是请求方法
    - GET：申请获取资源，不对服务器产生任何其他影响
    - HEAD：和 GET 类似，仅要求返回头部信息
    - POST：客户端提交数据的方法，可能会影响服务器
    - PUT：上传某个文件
    - DELETE：删除某个资源
    - TRACE：要求服务器返回原始 http 请求的内容，可以查看中间服务器对请求额影响。
    - OPTIONS：查看服务器对某个特定 URL 都支持哪些请求方法，可以把 URL 设置为 `*` 从而获得所有支持的请求方法
    - CONNECT：用于某些代理服务器，可以把请求的连接转化为一个安全隧道
    - PATCH：对某个资源做部分修改

- 安全方法：只获取资源，不修改服务器内容。HEAD GET OPTIONS TRACE

- 等幂：多次连续的重复的请求和一次请求效果一样。安全方法 + PUT DELETE

- url：http://www.baidu.com/index.html
- scheme：`http`，表示获取目标资源需要使用的应用层协议
- host：`www.baidu.com`，指定资源所在目标主机
- file：`index.html` 指示资源文件的名称

- http 请求头部：一个请求可以包含多个字段，一个头部字段用一行表示，`name: value`，顺序任意

- 一个 TCP 连接只能为一个 http 请求服务，当处理完客户的一个 http 请求后，web 服务器就主动将连接关闭了。

- 短连接：同一个用户的多个连续的 http 请求不能共用一个 TCP 连接
- 长连接：多个请求可以使用同一个 TCP 连接。
- `Connection` 字段。如果传输完后关闭则设为 `close`，保持一段时间等待后续连接 `keep-alive`

- 头部字段之后必须空行，表示头部字段结束，头部字段以回车和换行结束，空行必须只包含一个回车换行

- 消息体可选，非空需要包含字段 `Content-Length`


### http 应答

- 状态行：第一行，指示服务器使用协议版本号，`200 OK` 是状态码和状态信息

- 状态码：
    - 1xx 信息
        - 100 Continue 服务器收到了客户端请求行和头部信息，告诉客户端继续发送数据部分。
    - 2xx 成功
        - 200 OK 请求成功
    - 3xx 重定向
        - 301 Moved Permanently 资源被转移，请求将被重定向
        - 302 Found 通知客户端，资源能在其他地方被找到，需要用 GET 访问
        - 304 Not Modified 
        - 307 Temporary Redirect 通知客户端资源能在其他地方找到，与 302 不同点在于客户端可以用和原始请求相同的方法来访问目标资源
    - 4xx 客户端错误
        - 400 Bad Request 通用客户请求错误
        - 403 Forbidden 访问禁止，通常是没有权限
        - 404 Not Found 资源没找到
        - 407 Proxy Auth Required 客户需要先获取代理服务器认证
    - 5xx 服务器错误
        - 500 Internal Server Error 通用服务器错误
        - 503 Service Unavailable 暂时无法访问服务器


- Cookie：http 是无状态协议，后续请求如果需要用到之前的请求信息必须重传。需要 cookie 保持 http 连接状态，可以用 cookie 区分不同用户
