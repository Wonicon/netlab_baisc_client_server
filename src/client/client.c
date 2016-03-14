/**
 * @file     client.c
 * @author   whz
 * @brief    客户端主程序
 */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "client/config.h"

/**
 * @brief  客户端程序主体
 *
 * 负责创建连接，并执行交互循环。
 */
int main(void)
{
    struct sockaddr_in client_address = {};
    client_address.sin_family = AF_INET;
    client_address.sin_addr.s_addr = inet_addr(SERVER_IP);
    client_address.sin_port = htons(SERVER_PORT);

    int client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket_fd == -1) {
        perror(MSG_SOCKET_FAILURE);
    }

    if (connect(client_socket_fd, (struct sockaddr *)&client_address, sizeof(client_address))) {
        perror(MSG_CONNECT_FAILURE);
    }
}
