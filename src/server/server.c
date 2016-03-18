/**
 * @file     server.c
 * @author   whz
 * @brief    服务器程序入口，负责一些初始化工作
 */

#include "server/weather_service.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/**
 * @brief 初始化服务器，获得监听套接字
 */
static int init_server(uint16_t port_no);

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <port-number>\n", argv[0]);
        exit(-1);
    }

    long port_no = strtol(argv[1], NULL, 10);
    if (port_no > USHRT_MAX && port_no < 0) {
        fprintf(stderr, "ERROR: port number %ld is invalid.\n", port_no);
        exit(-1);
    }

    int listen_socket = init_server((uint16_t)port_no);

    int count = 0;
    for(;;) {
        Connection *link = malloc(sizeof(Connection));
        link->id = count++;
        link->socket_fd = accept(listen_socket, (struct sockaddr *)&link->address, &link->length);
        pthread_create(&link->tid, NULL, weather_service_main_loop, link);
    }
}


/**
 * @brief 初始化服务器
 * @param portno 端口号
 * @return 绑定本机地址的监听套接字
 *
 * 创建套接字，绑定本机默认 IP 地址与给定端口号。
 * 如果发生错误会直接结束程序。
 */
static int init_server(uint16_t port_no)
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("Cannot open socket");
        exit(-1);
    }

    struct sockaddr_in server_address = {
        .sin_family      = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port        = htons(port_no)
    };

    if (bind(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address))) {
        perror("Cannot bind");
        exit(-1);
    }

    listen(socket_fd, 5);

    return socket_fd;
}