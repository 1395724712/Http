# Http
Keywords:轮子，http状态机

## 1、 HTTP状态机
我希望这个项目可以成为一个即插即用的HTTP状态机，从设计开始一步一步、一点点完成一个能够用于webserver的http状态机

## 2、todo:
- [x] 测试Mutex动态库的运行
- [x] 测试子文件夹下有.git和CMakeLists.txt的运行
> 子文件夹下有CMakeLists.txt的情况，不需要设置那么多
- [ ] 导入日志项目
> 太烂了，不倒了吧
- [ ] 设计解析请求部分的总体框架

## 3、请求解析部分的总体设计

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