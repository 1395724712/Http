#include"http.hpp"
#include<functional>
Http::LINESTATE Http::getLine(string &line){
    //0、 先加锁
    MutexLock lock(msgMutex_);

    //1、如果msgItr指向请求消息最后一个元素的下一个，说明此时无待处理报文
    if(msgItr_>=requestMsg.end())
        return MSG_EMPTY;
    
    //2、寻找\r\n
    string::iterator nailItr = msgItr_;
    
    //2.1、 先找\r
    while(nailItr<requestMsg.end()&&*nailItr!='\r')
        nailItr++;

    //没找到'\r',或者找到了，但已经到末尾了
    if(nailItr==requestMsg.end()||nailItr+1==requestMsg.end())
        return LINE_OPEN;
    
    //找到了，但是之后不是\n
    if(*(nailItr+1)!='\n')
        return LINE_BAD;
    
    //3、返回line
    nailItr++;
    line = requestMsg.substr(msgItr_ - requestMsg.begin(),nailItr-2-msgItr_+1);
    msgItr_ = nailItr+1;
    return LINE_OK;
}

void Http::getMessage(string msg){
    {
        MutexLock lockGuard(msgMutex_);
        requestMsg += msg;
    }

    if(requestMsg.size()==msg.size())
        msgItr_ = requestMsg.begin();
    
    //todo:多线程部分尝试C++11的新特性
    // pthread_t pidId;
    // std::function<void(void*)> parseMsgThread = std::bind(parseMsg,this,std::placeholders::_1);
    // pthread_create(&pidId,nullptr,this->parseMsg,nullptr);
}