#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdio>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

pthread_t tid_send,tid_recv;
int socket_cli;

char sendBuf[100];
char recvBuf[100];

void *thfunc_send(void *arg)
{
    while(1)
    {
        memset(sendBuf, 0, sizeof(sendBuf));
        //printf("send:");
        scanf("%s",sendBuf);
        send(socket_cli, sendBuf, strlen(sendBuf)+1, 0);
    }
    pthread_exit(NULL);
}

void *thfunc_recv(void *arg)
{
    while(1)
    {
        int buflen = recv(socket_cli, recvBuf, 100, 0);
        if(recv > 0)
        {
            printf("receive:%s\n",recvBuf);
        }
    }
    pthread_exit(NULL);
}

int main()
{


    int socket_serv;
    sockaddr_in addr_serv;
    int res;
    char on = 1;

    socket_serv = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_serv == -1)
    {
        perror("socket error!");
        return -1;
    }
    printf("socket ok!\n");
    setsockopt(socket_serv, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    memset(&addr_serv, 0, sizeof(struct sockaddr_in));
    addr_serv.sin_addr.s_addr = inet_addr("192.168.174.185");
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_port = htons(7000);

    if (-1 == bind(socket_serv, (struct sockaddr *)(&addr_serv), sizeof(struct sockaddr)))
    {
        perror("bind error!");
        return -1;
    }
    printf("bind ok!\n");

    //获取套接字地址
    sockaddr_in socket_addr;
    socklen_t socklen = sizeof(socket_addr);
    getsockname(socket_serv, (struct sockaddr *)(&socket_addr), &socklen);
    printf("本地套接字:ip=%s,port=%d\n", inet_ntoa(socket_addr.sin_addr), ntohs(socket_addr.sin_port));

    listen(socket_serv, 5);

    
    sockaddr_in addr_cli;
    socklen_t len = sizeof(addr_cli);


    printf("-------wait for client-------\n");
    socket_cli = accept(socket_serv, (struct sockaddr *)(&addr_cli), &len);

    //连接成功后首先发送信息
    sprintf(sendBuf, "Welcome client(ip=%s,port=%d) to Server!", inet_ntoa(addr_cli.sin_addr), ntohs(addr_cli.sin_port));
    send(socket_cli, sendBuf, strlen(sendBuf)+1, 0);

    //接收信息
    recv(socket_cli, recvBuf, 100, 0);
    printf("Receive client's msg: %s\n",recvBuf);

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

    close(socket_cli);
    close(socket_serv);
    return 0;
}