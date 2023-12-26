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

//线程1负责发送数据,处理对象 sendBuf
void *client_send(void* socketfd)
{
    int *fd = (int *)(socketfd);
    while(1)
    {
        pthread_mutex_lock(&mutex);
        //memset(&sendBuf, 0, sizeof(sendBuf));
        printf("输入exit退出\rto server:");
        scanf("%s",sendBuf);
        send(*fd, sendBuf, strlen(sendBuf)+1, 0);
        pthread_mutex_unlock(&mutex);

        sleep(2);
    }


}
// //线程2负责接收数据,处理对象 recBuf
// void *client_recv(void* socketfd)
// {
//     int *fd = (int *)(socketfd);
//     while(1)
//     {
//         pthread_mutex_lock(&mutex);
//         memset(&recvBuf, 0, sizeof(recvBuf));
//         recv(*fd, recvBuf, 100, 0);
//         printf("recv: %s\n",recvBuf);
//         pthread_mutex_unlock(&mutex);

//         sleep(2);
//     }
// }


void pthread(void *socketfd)
{
    pthread_t tid_send,tid_recv;
    int ret;

    pthread_mutex_init(&mutex, NULL);

    ret = pthread_create(&tid_send, NULL, client_send, &socketfd);
    if (ret)
    {
        handle_error_en(ret, "pthread_send failed");
    }

   
    // ret = pthread_create(&tid_recv, NULL, client_recv, &socketfd);
    // if (ret)
    // {
    //     handle_error_en(ret, "pthread_recv failed");
    // }

    pthread_join(tid_send, NULL);
    // pthread_join(tid_recv, NULL);

    pthread_mutex_destroy(&mutex);
}

int main()
{
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

    //pthread((void *)&socket_client);
    while(1)
    {
        printf("1---%s---\n",sendBuf);

        printf("发送内容(输入exit代表退出): ");
        scanf("%s",sendBuf);
        send(socket_client, sendBuf, strlen(sendBuf)+1, 0); 

        printf("2---%s---\n",sendBuf);
        if(strcmp(sendBuf, "exit") == 0)
        break;
    }
    close(socket_client);

    return 0;
}