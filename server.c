#include<stdio.h>
#include<sys/socket.h>
#include<pthread.h>
#include<string.h>
#include"transmission.h"
#include<stdlib.h>
#include<unistd.h>
#include<netinet/in.h>
#include<errno.h>
 
int sockfd_addr[ MAX_CLIENT];
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;
int online_client=0;

void *handle_client( void *arg);

int sign_client(int *p){
    pthread_mutex_lock(&client_mutex);
    //把堆缓冲中的fd放到i本地变量中
    int client_fd=*p;
    free(p);
    if(MAX_CLIENT<=online_client){
        printf("服务器已满 客户端%d注册失败\n",client_fd);
        pthread_mutex_unlock(&client_mutex);
        return -1;
    }
    for(int i=0;i<MAX_CLIENT;i++){
        if(sockfd_addr[i]==0){
            pthread_t tid;
            int *t_id=malloc(sizeof(int));
            *t_id=client_fd;
            if(pthread_create(&tid,NULL,handle_client,(void*)t_id)!=0){
                perror("pthread_create");
                free(t_id);
                pthread_mutex_unlock(&client_mutex);
                return -1;
            }
            pthread_detach(tid);
            pthread_mutex_unlock(&client_mutex);
            
            sockfd_addr[i]=client_fd;
            online_client++;
            return 0;
        }
    }
    pthread_mutex_unlock(&client_mutex);
    return -1;
}

ssize_t rm_client(void* fd){
    pthread_mutex_lock(&client_mutex);
    int client_fd=*((int*)fd);
    for(int i=0;i<MAX_CLIENT;i++){
        if(client_fd==sockfd_addr[i]){
            sockfd_addr[i]=0;
            online_client--;
            printf("客户端%d已退出\n",client_fd);
            pthread_mutex_unlock(&client_mutex);
            return 0;
        }
    }
    pthread_mutex_unlock(&client_mutex);
    return -1;
}

void *send_broadcast(char *msg){
    pthread_mutex_lock(&client_mutex);
    int len=strlen(msg);
    for(int i=0;i<MAX_CLIENT;i++){
        if(sockfd_addr[i]!=0){
            int total=0;
            while(total<len){
                int ret_s = send(sockfd_addr[i],msg+total,len-total,0);
                if(ret_s<=0){
                    perror("send");
                    shutdown(sockfd_addr[i],SHUT_RDWR);
                    break;
                }
                total+=ret_s;
            }
        }
    }
    pthread_mutex_unlock(&client_mutex);
    return NULL;
}

void *handle_client(void *arg){
    int client_fd = *((int*)arg);
    free(arg);
    char *read_buf=NULL;
    char *write_buf=NULL;
    read_buf=malloc(sizeof(char)*1024);
    memset(read_buf,0,1024);
    write_buf=malloc(sizeof(char)*1024);
    memset(write_buf,0,1024);
    if(read_buf==NULL||write_buf==NULL){
        perror("malloc");
        free(read_buf);
        free(write_buf);
        return NULL;
    }
    //接收客户端发送的请求
    while(1){
        ssize_t n=recv_line(client_fd,read_buf,1024);
        if(n==0){
            continue;
        }
        if(n<0){
            printf("客户端%d读取失败\n",client_fd);
            //rm_client移除客户端
            int *p=malloc(sizeof(int));
            *p=client_fd;
            rm_client(p);
            free(p);
            close(client_fd);
            free(read_buf);
            free(write_buf);
            return NULL;
        }
        printf("客户端%d: %s\n",client_fd,read_buf);
        int ret_n=snprintf(write_buf,1024,"客户端%d: %s\n",client_fd,read_buf);
        if(ret_n<0){
            perror("snprintf");
        }
        else if(ret_n>=1024){
            fprintf(stderr,"数据被截断\n");
        }
        else{
            send_broadcast(write_buf);
        }
    }
}

int main()
{
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketfd < 0){
        perror("socketfd");
        return EXIT_FAILURE;
    }
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(socketfd,(struct sockaddr*)&addr , sizeof(addr))){
        perror("bind");
        close(socketfd);
        return EXIT_FAILURE;
    }

    if(listen(socketfd,10)){
        perror("listen");
        close(socketfd);
        return EXIT_FAILURE;
    }

    struct sockaddr_in client_addr;
    memset(&client_addr,0,sizeof(client_addr));
    socklen_t client_addr_len=sizeof(client_addr);
    while(1){
        int connfd=accept(socketfd,(struct sockaddr*)&client_addr,&client_addr_len);
        if(connfd<=0){
            if(errno==EINTR)continue;
            perror("accept");
            return EXIT_FAILURE;
        }
        //把栈数据放到堆缓冲中
        int *p=malloc(sizeof(int));
        *p=connfd;

        if(sign_client(p)<0){
            printf("客户端%d连接失败\n",connfd);
            free(p);
            continue;
        }
        else{
        printf("客户端%d连接成功\n",connfd);
        }
    }
    printf("服务器关闭\n");
    close(socketfd);
    return 0;
}