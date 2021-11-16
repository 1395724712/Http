#include"http.hpp"
#include<functional>
#include<assert.h>
#include<iostream>
using namespace std;
Http::LINESTATE Http::getLine(string &line){
    //0 加锁
    //可不可以不加锁
    MutexLock lockGuard(msgMutex_);

    //1、如果lineStart_大于requestMsg的长度，说明此时无待处理报文
    assert(lineStart_<=requestMsg.size());
    assert(lineCur_<=requestMsg.size());
    if(lineStart_==requestMsg.size())
        return MSG_EMPTY;

    //2、寻找\r\n
    int lineEnd = -1;
    while(lineCur_<requestMsg.size()){
        if(requestMsg[lineCur_]=='\r'){
            //2.1、当前位置是\r,检查下一个位置是不是\n
            if(lineCur_==requestMsg.size()-1)
                return LINE_OPEN;//当前位置是最后一个
            else if(requestMsg[lineCur_]=='\n'){
                lineEnd = lineCur_-1;//当前位置的下个位置是\n，返回一行
                break;
            }
            else
                return LINE_BAD;
        }
        else if(requestMsg[lineCur_]=='\n'){
            //2.2、当前位置是\n,检查上一个是不是\r
            if(requestMsg[lineCur_-1]=='\r'){
                lineEnd = lineCur_-2;
                break;
            }
            else
                return LINE_BAD;
        }
        //什么都不是就继续吧
        lineCur_++;
    }

    line = requestMsg.substr(lineStart_,lineEnd - lineStart_+1);
#ifdef DEBUG_GETLINE
    cout<<line<<endl;
#endif
    lineStart_ = lineEnd + 2;
    lineCur_ = lineEnd +2;
}

void Http::getMessage(string msg){
    {
        MutexLock lockGuard(msgMutex_);
        requestMsg += msg;
    }
    
    //todo:多线程部分尝试C++11的新特性
    pthread_t pidId;
    pthread_create(&pidId,nullptr,Http::parseMsg,this);
}