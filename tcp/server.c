#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 9000
#define DATA_SIZE 1024
// char* IP= "127.0.0.1";

// server.c
// bind
// listen
// accept
// send/recv
struct SockInfo
{
    int fd;                      // 通信
    // pthread_t tid;               // 线程ID
    // struct sockaddr_in addr;     // 地址信息
};

struct SockInfo infos[128];

void* work(void* arg);
int main()
{
    int lfd = socket(AF_INET, SOCK_STREAM, 0);

    if (lfd == -1)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = bind(lfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (ret == -1)
    {
        perror("bind");
        exit(1);
    }

    ret = listen(lfd, 128);
    if (ret == -1)
    {
        perror("listen");
        exit(1);
    }

    for (int i = 0; i < 128; i++)
        {
            infos[i].fd = -1;
            // infos[i].tid = -1;
        }
    // pthread_t tid = -1;
    while (1)
    {
        struct SockInfo* pinfo;
        for(int i=0; i<128; ++i)
        {
            if(infos[i].fd == -1)
            {
                pinfo = &infos[i];
                break;
            }
            if(i == 128-1)
            {
                sleep(1);
                i--;
            }
        }
        struct sockaddr_in cliaddr ;
        socklen_t len = sizeof(cliaddr);
        int cfd = accept(lfd, (struct sockaddr*)&cliaddr, &len);
        if (cfd == -1)
        {
            perror("accept");
        }
        pinfo->fd = cfd;
        pthread_t tid;
        // pthread_create(&pinfo->tid, NULL, (void*)work, (void*)pinfo);
        // pthread_detach(pinfo->tid);
        pthread_create(&tid, NULL, (void*)work, (void*)pinfo);
        pthread_detach(tid);
    }

    close(lfd);
    return 0;
}

void* work(void* arg)
{
    struct SockInfo* info = (struct SockInfo*)arg;
    char readbuf[1024];
    char sendbuf[1024];

    printf("listen...\n");
    while (1)
    {
        ssize_t size = read(info->fd, readbuf, sizeof(readbuf));
        if (size <= 0)
        {
            printf("client %d disconnect...\n", info->fd);
            break;
        }
        printf("cfd = %d, data from client:%s\n",info->fd, readbuf);
        sprintf(sendbuf, "fd = %d", info->fd);
        write(info->fd, sendbuf, strlen(sendbuf)+1);
        memset(readbuf, 0, sizeof(readbuf));
        memset(sendbuf, 0, sizeof(readbuf));
    }

    close(info->fd);
}
