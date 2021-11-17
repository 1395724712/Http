#include"Mutex.hpp"
#include"http.hpp"
#include<iostream>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
using namespace std;

int main(){
    
    int fd = open("./requestMsg",O_RDONLY);
    char txt[1000];
    read(fd,txt,1000);
    string requestStr(txt,1000);

    // cout<<requestStr<<endl;

    for(int i=0;i<requestStr.size();i++){
        if(requestStr[i]==' '&&i<requestStr.size()-1&&requestStr[i+1]==' ')
            requestStr.replace(requestStr.begin()+i,requestStr.begin()+i+2,"\r");
    }

    // for(int i)
    

    // cout<<"what"<<"\r"<<"is"<<endl;

    Http httpParser;
    int pos = 0;
    for(int i=0;i<10;i++){
        string subStr = requestStr.substr(pos,100);
        pos+=100;
        httpParser.getMessage(subStr);
    }

    cout<<"running success"<<endl;
    return 0;
}