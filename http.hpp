#ifndef WH_HTTP_HPP
#define WH_HTTP_HPP
#include"Mutex/Mutex.hpp"
#include<string>
using namespace std;

#define DEBUG_TEST
#ifdef DEBUG_TEST
#define DEBUG_GETLINE
#define DEBUG_PARSEMSG
#endif

class Http{
public:
    Http();

public:
    //报文请求方法
    enum METHOD{GET,POST,HEAD,PUT,DELETE,TRACE,OPTIONS,CONNECT,PATH};
    //主状态机的状态
    enum MAINSTATE{CHECK_STATE_REQUESTLINE,CHECK_STATE_HEADER,CHECK_STATE_CONTENT};
    //报文解析结果
    enum PARSESTATE{NO_REQUEST,GET_REQUEST,BAD_REQUEST,NO_RESOURCE,FORBIDDEN_REQUEST,
    FILE_REQUEST,INTERNAL_ERROR,CLSOED_CONNECTIOn};
    //从状态机状态
    enum LINESTATE{LINE_OK,LINE_BAD,LINE_OPEN,MSG_EMPTY};
    
    //接收消息
    void getMessage(string msg);

private:
    //请求方法
    METHOD method_;
    //主状态机状态
    MAINSTATE mainState_;
    //报文解析结果
    PARSESTATE parseRes_;

private:
    //负责保护parseMsg的锁
    Mutex parseMutex_;
    //解析消息
    static void* parseMsg(void*);

    //获取一行
    LINESTATE getLine(string& msg);

    //负责保护requestMsg的锁
    Mutex msgMutex_;
    //请求报文
    string requestMsg_ GUARDED_BY(msgMutex_);
    //指向请求报文新一行的index
    unsigned int lineStart_;
    //指向请求报文待解析部分开头的index
    unsigned int lineCur_;
};

#endif