#include"http.hpp"
#include<functional>
#include<assert.h>
#include<iostream>
#include<algorithm>
using namespace std;

Http::Http():
mainState_(CHECK_REQUEST_REQUESTLINE),//主状态机的初始状态是解析请求行
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
    while(lineState == LINE_OK){
        
        switch(This->mainState_){
            case CHECK_REQUEST_REQUESTLINE:
                {
                    PARSESTATE parseRes = This->parseRequestLine(line);
                }
                break;
            case CHECK_REQUEST_HEADER:
                {
                    PARSESTATE parseRes = This->parseRequestHeader(line);
                }
                break;
            case CHECK_REQUEST_CONTENT:
                break;
            defualt:
                break;
        }

        line = "";
        lineState = This->getLine(line);
    }
}

Http::PARSESTATE Http::parseRequestLine(const string& line){
    //0 本函数的工作
    //0.1、获取请求方法、uri、http版本号
    //0.2、本函数负责修改mianState_的状态
    unsigned int startIdx = 0;
    unsigned int endIdx = 0;

    //0.3 消除开始位置的空格
    while(line[startIdx]==' '||line[startIdx]=='\t')
        startIdx++;
    endIdx = startIdx;

    //1、获取请求方法
    PARSESTATE returnRes;
    while(endIdx<line.size()&&line[endIdx]!=' '&&line[endIdx]!='\t')
        endIdx++;
    string method = line.substr(startIdx,endIdx-startIdx);
    //避免不规范，转换为大写
    transform(method.begin(),method.end(),method.begin(),::toupper);
    if(method=="GET")
    {
        method_ = GET;
        returnRes = GET_REQUEST;
    }
    else if(method == "POST"){
        method_ = POST;
        returnRes = POST_REQUEST;
    }
    else
        return NO_REQUEST;

    //2、获取uri
    startIdx = endIdx+1;
    while(startIdx<line.size()&&(line[startIdx]==' '||line[startIdx]=='\t'))
        startIdx++;
    endIdx = startIdx+1;

    while(endIdx<line.size()&&line[endIdx]!=' '&&line[endIdx]!='\t')
        endIdx++;
    startIdx = endIdx;

    while(startIdx>=0&&line[startIdx]!='/')
        startIdx--;
    startIdx++;
    if(startIdx==0)
        return BAD_REQUEST;
    uri_ = line.substr(startIdx,endIdx - startIdx);

    //3、获取版本号
    startIdx = endIdx+1;
    while(startIdx<line.size()&&(line[startIdx]==' '||line[startIdx]=='\t'))
        startIdx++;
    endIdx = startIdx+1;
    while(endIdx<line.size()&&line[endIdx]!=' '&&line[endIdx]!='\t')
        endIdx++;
    httpVersion_ = line.substr(startIdx,endIdx-startIdx);

    //4、切换主状态机状态
    mainState_ = CHECK_REQUEST_HEADER;

#ifdef DEBUG_PARSE_REQLINE
    cout<<"Method: "<<method<<endl;
    cout<<"URI: "<<uri_<<endl;
    cout<<"HTTP Version: "<<httpVersion_<<endl;
#endif
    return returnRes;
}

Http::PARSESTATE Http::parseRequestHeader(const string& line){
    //0 负责的工作
    //0.1 从请求头部中获取我们需要的信息
    //0.2 如果读如的是空行，主状态机转为CHECK_REQUEST_CONTENT

    //1、 获取字段名
    int startIdx =0;
    int endIdx =0;
    while(startIdx<line.size()&&(line[startIdx]==' '||line[startIdx]=='\t'))
        startIdx++;
    if(startIdx == line.size())
        return BAD_REQUEST;

    endIdx = startIdx;
    while(endIdx<line.size()&&line[endIdx]!=':')
        endIdx++;
    if(endIdx==line.size())
        return BAD_REQUEST;

    string segmentName = line.substr(startIdx,endIdx-startIdx);
    transform(segmentName.begin(),segmentName.end(),segmentName.begin(),::toupper);
    
    //2、获取字段值
    startIdx = endIdx+1;
    while(startIdx<line.size()&&(line[startIdx]==' '||line[startIdx]=='\t'))
        startIdx++;
    if(startIdx==line.size())
        return BAD_REQUEST;
    
    endIdx = line.size()-1;
    while(line[endIdx]==' '||line[endIdx]=='\t')
        endIdx--;
    if(endIdx==startIdx-1)
        return BAD_REQUEST;
    
    string segmentValue = line.substr(startIdx,endIdx-startIdx+1);
    transform(segmentValue.begin(),segmentValue.end(),segmentValue.begin(),::toupper);

    if(segmentName == "CONNECTION")
        if(segmentValue=="KEEP-ALIVE")
            keepConnection_ = true;
        else
            keepConnection_ = false;
#ifdef DEBUG_PARSE_REQHEADER
    cout<<"Segment Name: "<<segmentName<<endl;
    cout<<"Segment Value: "<<segmentValue<<endl;
#endif

}