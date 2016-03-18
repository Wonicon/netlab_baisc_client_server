/**
 * @file     client.c
 * @author   whz
 * @brief    客户端主程序
 */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include "client/config.h"
#include "client/monitor.h"

/**
 * @brief  客户端程序主体
 *
 * 负责创建连接，并执行交互循环。
 */
int main(int argc, char *argv[])
{
    if ((argc != 1 && argc != 3) || (argc > 1 && !strcmp(argv[1], "help"))) {
        int indent;
        printf("Usage: %n%s                            # connects to official server\n", &indent, argv[0]);
        printf(      "%*s%s <server-ip> <server-port>  # connects to custom server\n", indent, "", argv[0]);
        printf(      "%*s%s help                       # show this message\n", indent, "", argv[0]);
        return 0;
    }

    struct sockaddr_in client_address = {};
    client_address.sin_family = AF_INET;
    client_address.sin_addr.s_addr =
        (argc == 3) ? inet_addr(argv[1]) : inet_addr(SERVER_IP);
    client_address.sin_port =
        (argc == 3) ? htons((uint16_t)atoi(argv[2])) : htons(SERVER_PORT);

    int client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket_fd == -1) {
        perror(MSG_SOCKET_FAILURE);
        exit(-1);
    }

    if (connect(client_socket_fd, (struct sockaddr *)&client_address, sizeof(client_address))) {
        perror(MSG_CONNECT_FAILURE);
        exit(-1);
    }

    monitor_main_loop(client_socket_fd);
}
