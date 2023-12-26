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

    int socket_cli;
    sockaddr_in addr_cli;
    socklen_t len = sizeof(addr_cli);

    while(1)
    {
        printf("-------wait for client-------\n");
        socket_cli = accept(socket_serv, (struct sockaddr *)(&addr_cli), &len);

        //连接成功后首先发送信息
        sprintf(sendBuf, "Welcome client(ip=%s,port=%d) to Server!", inet_ntoa(addr_cli.sin_addr), ntohs(addr_cli.sin_port));
        send(socket_cli, sendBuf, strlen(sendBuf)+1, 0);

        //接收信息
        recv(socket_cli, recvBuf, 100, 0);
        printf("Receive client's msg: %s\n",recvBuf);

        while(1)
        {
            recv(socket_cli, recvBuf, 100, 0);
            printf("Receive client's msg: %s\n",recvBuf);
            if(strcmp(recvBuf, "exit") == 0)
            break;
        }
        close(socket_cli);

        puts("continue to listen?(y/n)");
        char ch[2];
        scanf("%s",ch, 2);
        if(ch[0] != 'y')
        break;

    }
    close(socket_serv);
    return 0;
}