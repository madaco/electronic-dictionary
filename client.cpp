#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdio>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

int main()
{
    char sendBuf[100];
    char recvBuf[100];

    int socket_client;

    socket_client = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_client == -1)
    {
        perror("socket error!");
        return -1;
    }
    printf("socket ok!\n");

    int ret;
    sockaddr_in addr_client;
    
    memset(&addr_client, 0, sizeof(addr_client));
    addr_client.sin_addr.s_addr = inet_addr("192.168.174.185");
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

    while(1)
    {
        printf("发送内容(输入exit代表退出): ");
        scanf("%s",sendBuf);
        send(socket_client, sendBuf, strlen(sendBuf)+1, 0); 
        if(strcmp(sendBuf, "exit") == 0)
        break;
    }
    close(socket_client);

    return 0;
}