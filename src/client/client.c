/**
 * @file     client.c
 * @author   whz
 * @brief    客户端主程序
 */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "lib/proxy.h"
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

    // Just a demo

    CityRequestHeader header;
    construct_request(&header, 0x0202, "nanjing", 10);

    if (send(client_socket_fd, &header, sizeof(header), 0) == -1) {
        perror(MSG_SEND_FAILURE);
    }

    CityResponseHeader response = {};
    ssize_t n_read = recv(client_socket_fd, &response, sizeof(response), 0);
    printf("n_read = %ld\n", n_read);
    printf("type %04x\n", ntohs(response.type));
    printf("name %s\n", response.city_name);
    printf("year %d\n", ntohs(response.year));
    printf("month %d\n", response.month);
    printf("day %d\n", response.day);
    printf("n %d\n", response.n_status);
}
