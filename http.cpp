#include"http.hpp"
#include<functional>
#include<assert.h>
#include<iostream>
using namespace std;

Http::Http():
mainState_(CHECK_STATE_REQUESTLINE),//主状态机的初始状态是解析请求行
parseRes_(NO_REQUEST),
msgMutex_(Mutex()),
parseMutex_(Mutex()),
lineStart_(0),
lineCur_(0)
{}

Http::LINESTATE Http::getLine(string &line){
    //0 加锁
    //可不可以不加锁
    MutexLock lockGuard(msgMutex_);

    //1、如果lineStart_大于requestMsg的长度，说明此时无待处理报文
    //或在发生了LINE_BAD错误，不再进行处理
    if(lineStart_>=requestMsg_.size())
        return MSG_EMPTY;

    //2、寻找\r\n
    int lineEnd = -1;
    while(lineCur_<requestMsg_.size()){
        if(requestMsg_[lineCur_]=='\r'){
            //2.1、当前位置是\r,检查下一个位置是不是\n
            if(lineCur_==requestMsg_.size()-1)
                return LINE_OPEN;//当前位置是最后一个
            else if(requestMsg_[lineCur_+1]=='\n'){
                lineEnd = lineCur_-1;//当前位置的下个位置是\n，返回一行
                break;
            }
            else{
                lineStart_ = -1;
                return LINE_BAD;
            }
        }
        else if(requestMsg_[lineCur_]=='\n'){
            //2.2、当前位置是\n,检查上一个是不是\r
            if(requestMsg_[lineCur_-1]=='\r'){
                lineEnd = lineCur_-2;
                break;
            }
            else{
                lineCur_ = -1;
                return LINE_BAD;
            }
        }
        //什么都不是就继续吧
        lineCur_++;
    }
    if(lineEnd == -1)
        return LINE_OPEN;

    line = requestMsg_.substr(lineStart_,lineEnd - lineStart_+1);
#ifdef DEBUG_GETLINE
    cout<<line<<endl;
#endif
    lineStart_ = lineEnd + 3;
    lineCur_ = lineEnd +3;
    return LINE_OK;
}

void Http::getMessage(string msg){
    {
        MutexLock lockGuard(msgMutex_);
        requestMsg_ += msg;
    }
    
    //todo:多线程部分尝试C++11的新特性
    pthread_t pidId;
    pthread_create(&pidId,nullptr,parseMsg,this);
    pthread_detach(pidId);
}

void* Http::parseMsg(void* para){
    Http *This = (Http*)para;
    //0 开始运行前先加锁
    MutexLock lockGuard(This->parseMutex_);

    //1 然后检查requestMsg是否已经处理完毕
    string line;
    LINESTATE lineState = This->getLine(line);
    if(lineState == MSG_EMPTY)
        return nullptr;
    while(line!=""){

        line = "";
        lineState = This->getLine(line);
    }
}