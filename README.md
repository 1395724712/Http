# Http
Keywords:轮子，http状态机，GET、POST

## 1、 HTTP状态机
我希望这个项目可以成为一个即插即用的HTTP状态机，从设计开始一步一步、一点点完成一个能够用于webserver的http状态机

## 2、todo:
- [x] 测试Mutex动态库的运行
- [x] 测试子文件夹下有.git和CMakeLists.txt的运行
> 子文件夹下有CMakeLists.txt的情况，不需要设置那么多
- [ ] 导入日志项目
> 太烂了，不导了吧
- [ ] 设计解析请求部分的总体框架

## 3、请求解析部分的总体设计

### 3.1、 唯一的公有功能函数
`void getMessage(string msg)`
本函数负责将输入的信息加入到已有信息的末尾，然后**别的线程**进行消息解析，本函数返回
* 为什么不在本线程进行解析
> 会影响消息接收，

### 3.2、 消息解析
`void parseMsg()`
本函数但当主状态机的工作，同时还要负责在解析出结果后，调用相应的负责应答的业务函数。
* 进入本函数后，先加锁
> 确保同一时刻只有一个`parseMsg()`在处理，确保状态机正常运行
* 然后检查`requestMsg`是否为空
> 如果为空，说明解析已经被其他线程的`parseMsg()`完成，直接返回
* 主状态机负责解析请求行和请求首部信息
> * `parseRequestLine`负责解析请求行的工作,要求从中获取`method_`,`uri_`,`httpVersion_`
> * `parseRequestHeader`负责解析请求首部的内容，要求从中获取`KeepConnection_`,
> 对于Http版本号，暂时不做否决处理，只保留
> 对于请求首部的其他信息，现在即便知道了也不知道怎么处理，所以暂时不保留

#### 3.2.1、解析请求行`parseRequestLine`
* 它负责什么工作？
> 解析请求行
* 它返回什么？
> 解析状态PRASESTATE:如果成功返回请求类型`GET_REQUEST`、`POST_REQUEST`；如果请求类型暂时不接受，就返回`NO_REQUEST`;
> 如果请求存在语病，就返回`BAD_REQUEST`；对于其余的状态码，暂时不做考虑

#### 3.2.2、解析请求头部`parseRequestHead`
* 它负责什么工作？
> 从请求头部中读取**我们感兴趣的信息**
* 它返回什么？
> 解析状态PRASESTATE：如果运行正常就根据method返回；如果出错就返回`BAD_REQUEST`

### 3.3、 获取一行
`LINE_STATE getLine(string& line)`
将每个`\r\n`之间的内容交给`line`
* 维护一个index指向**每一个请求行开头**
> `lineStart_`永远只指向请求行的开头，由getMessage负责初始化
* 维护一个index指向当前的位置
> `lineCur_`指向当前的位置

### 3.1 关键成员变量

* string requestMsg_
> 收到的请求

* Mutex msgMutex_
> 保护msg

* Mutex parseMutex_;
> 保护同一时间只有一个parseMsg在允许（也许应该使用条件变量）

### 3.2 关键成员函数
* `LINESTATUS getline(string& line)`
> 从msg中获取一行

* `HTTPSTAUS parseMsg()`
> 解析请求msg

* `void getmsg(const string& s)`
> 将收到的数据s,添加到msg后