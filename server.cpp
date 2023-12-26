#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdio>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#define handle_error_en(en, msg) \
    do {errno = en; perror(msg); exit(EXIT_FAILURE);} while(0)

char sendBuf[100];
char recvBuf[100];

pthread_mutex_t mutex;

// //线程1负责发送数据,处理对象 sendBuf
// void *client_send(void* socketfd)
// {
//     int *fd = (int *)(socketfd);
//     while(1)
//     {
//         pthread_mutex_lock(&mutex);
//         memset(&sendBuf, 0, sizeof(sendBuf));
//         printf("输入exit退出\rto server:");
//         scanf("%s",sendBuf);
//         send(*fd, sendBuf, strlen(sendBuf)+1, 0);
//         pthread_mutex_unlock(&mutex);

//         sleep(2);
//     }


// }
//线程2负责接收数据,处理对象 recBuf
void *client_recv(void* socketfd)
{
    int *fd = (int *)(socketfd);
    while(1)
    {
        pthread_mutex_lock(&mutex);
        //memset(&recvBuf, 0, sizeof(recvBuf));
        recv(*fd, recvBuf, 100, 0);
        printf("recv: %s\n",recvBuf);
        pthread_mutex_unlock(&mutex);

        sleep(2);
    }
}


void pthread(void *socketfd)
{
    pthread_t tid_send,tid_recv;
    int ret;

    pthread_mutex_init(&mutex, NULL);

    // ret = pthread_create(&tid_send, NULL, client_send, &socketfd);
    // if (ret)
    // {
    //     handle_error_en(ret, "pthread_send failed");
    // }

    ret = pthread_create(&tid_recv, NULL, client_recv, &socketfd);
    if (ret)
    {
        handle_error_en(ret, "pthread_recv failed");
    }

    // pthread_join(tid_send, NULL);
    pthread_join(tid_recv, NULL);

    pthread_mutex_destroy(&mutex);
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

        //pthread((void *)&socket_cli);
        while(1)
        {
            printf("1---%s---\n",recvBuf);

            recv(socket_cli, recvBuf, 100, 0);

            printf("2-Receive client's msg: %s\n",recvBuf);
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