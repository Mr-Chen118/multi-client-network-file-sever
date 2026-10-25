#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<signal.h>
#include<pthread.h>
#include<arpa/inet.h>
#include"transmission.h"
#include<unistd.h>
#include<netinet/in.h>

void *read_from_server(void *arg){
    int sockfd=*((int*)arg);
    char read_buf[1024];
    while(1){
        int n=recv_line(sockfd,read_buf,sizeof(read_buf));
        if(n<0){
            printf("服务器已关闭连接\n");
            break;
        }else if(n==0)continue;
        printf("%s\n",read_buf);
        fflush(stdout);
    }
    shutdown(sockfd,SHUT_RD);
    return NULL;
}

void *write_to_server(void *arg){
    int sockfd=*((int*)arg);
    char write_buf[1024];
    while(fgets(write_buf,sizeof(write_buf),stdin)!=NULL){
        if(send_all(sockfd,write_buf,strlen(write_buf))<0){
            perror("send_all");
            break;
        }
    }
    shutdown(sockfd,SHUT_WR);
    return NULL;
}

int main(){
    signal(SIGPIPE,SIG_IGN);
    int sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0){
        perror("socket");
        return -1;
    }
    //配置服务器地址
    struct sockaddr_in server;
    memset(&server,0,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port=htons(8080);
    inet_pton(AF_INET,"127.0.0.1",&server.sin_addr);

    if(connect(sockfd,(struct sockaddr*)&server,sizeof(server))<0){
        perror("connect");
        close(sockfd);
        return -1;
    }
    pthread_t tid_r,tid_w;
    pthread_create(&tid_r,NULL,read_from_server,(void*)&sockfd);
    pthread_create(&tid_w,NULL,write_to_server,(void*)&sockfd);

    pthread_join(tid_r,NULL);
    pthread_join(tid_w,NULL);
    printf("exit\n");
    close(sockfd);
    return 0;
}