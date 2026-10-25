#include<stdio.h>
#include"transmission.h"
#include<string.h>
#include<sys/socket.h>
#include<errno.h>

int send_all(int fd, char *buf, size_t size){
    size_t used=0;
    while(used<size){
        ssize_t n=send(fd,buf+used,size-used,0);
        if(n<=0){
            if(errno==EAGAIN)continue;
            else return -1;
        }
        used+=n;
    }
    return 0;
}

ssize_t recv_line(int fd, char *buf, size_t size){
    size_t used=0;
    if(size<2){
        printf("接收缓存不足\n");
        return -1;
    }
    while(used+1<size){
        char ch;
        ssize_t n=recv(fd,&ch,1,0);
        if(n<0){
            if(errno==EINTR)continue;
            else return -1;
        }
        if(n==0){
            return -1;
        }
        if(ch=='\n'){
            buf[used]='\0';
            return used;
        }
        buf[used]=ch;
        used++;
    }
    buf[used]='\0';
    return -1;
}