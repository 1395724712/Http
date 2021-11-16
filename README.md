# Http
Keywords:轮子，http状态机

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
* 进入本函数后，先加锁
> 确保同一时刻只有一个`parseMsg()`在处理，确保状态机正常运行
* 然后检查`requestMsg`是否为空
> 如果为空，说明解析已经被其他线程的`parseMsg()`完成，直接返回
* 然后进行解析

### 3.3、 获取一行
`LINE_STATE getLine(string& line)`
将每个`\r\n`之间的内容交给`line`
* 维护一个index指向**每一个请求行开头**
> `lineStart_`永远只指向请求行的开头，由getMessage负责初始化
* 维护一个index指向当前的位置
> `lineCur_`指向当前的位置

### 3.1 关键成员变量

* string msg
> 收到的请求

* Mutex msgMutex_
> 保护msg

* Mutex ParseMutex_;
> 保护同一时间只有一个parseMsg在允许（也许应该使用条件变量）

### 3.2 关键成员函数
* `LINESTATUS getline(string& line)`
> 从msg中获取一行

* `HTTPSTAUS parseMsg()`
> 解析请求msg

* `void getmsg(const string& s)`
> 将收到的数据s,添加到msg后