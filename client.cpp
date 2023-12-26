#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdio>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

pthread_t tid_send,tid_recv;
int socket_client;

char sendBuf[100];
char recvBuf[100];


void *thfunc_send(void *arg)
{
    while(1)
    {
        memset(sendBuf, 0, sizeof(sendBuf));
        //printf("send:");
        scanf("%s",sendBuf);
        send(socket_client, sendBuf, strlen(sendBuf)+1, 0);
    }
    pthread_exit(NULL);
}

void *thfunc_recv(void *arg)
{
    while(1)
    {
        int buflen = recv(socket_client, recvBuf, 100, 0);
        if(recv > 0)
        {
            printf("receive:%s\n",recvBuf);
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{

    socket_client = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_client == -1)
    {
        perror("socket error!");
        return -1;
    }
    printf("socket ok!\n");

    int ret,res;
    sockaddr_in addr_client;
    
    memset(&addr_client, 0, sizeof(addr_client));
    addr_client.sin_addr.s_addr = inet_addr("192.168.174.185");
    //addr_client.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_client.sin_family = AF_INET;
    addr_client.sin_port = htons(7000);

    ret = connect(socket_client, (struct sockaddr *)(&addr_client), sizeof(struct sockaddr));
    if (ret == -1)
    {
        perror("connect error!");
        return -1;
    }
    printf("---connect ok!---\n");

    //连接成功后首先发送信息

    sprintf(sendBuf, "Client(ip=%s,port=%d) is connected!",
    inet_ntoa(addr_client.sin_addr), ntohs(addr_client.sin_port));
    send(socket_client, sendBuf, strlen(sendBuf)+1, 0);

    //接收信息

    recv(socket_client, recvBuf, 100, 0);
    printf("Receive server' msg: %s\n",recvBuf);

    // while(1)
    // {
    //     printf("发送内容(输入exit代表退出): ");
    //     scanf("%s",sendBuf);
    //     send(socket_client, sendBuf, strlen(sendBuf)+1, 0); 
    //     if(strcmp(sendBuf, "exit") == 0)
    //     break;
    // }

    res = pthread_create(&tid_send, NULL, thfunc_send, NULL);
    if (res)
    {
        perror("thread_send create failed");
        return -1;
    }
    
    res = pthread_create(&tid_recv, NULL, thfunc_recv, NULL);
    if (res)
    {
        perror("thread_recv create failed");
        return -1;
    }

    pthread_join(tid_send, NULL);
    pthread_join(tid_recv, NULL);

    close(socket_client);

    return 0;
}