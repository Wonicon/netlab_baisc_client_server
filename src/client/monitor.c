/**
 * @file     monitor.h
 * @author   whz
 * @brief    控制台界面的相关接口
 */

#include "client/monitor.h"
#include "client/config.h"
#include "lib/proxy.h"
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 发送请求辅助函数
 * @param socket_fd 客户端的套接字
 * @param type      请求类型，见 proxy.h 以 REQUEST_ 开头的宏定义
 * @param city_name 城市名称，只接受前 19 个有效字节
 * @param date      在单天请求中，表示日期下标，在多天请求中，表示天数
 * @return 响应报文指针，为 NULL 时发生错误
 *
 * 在函数内部会构造报文、转换字节序并将其发送，并且返回响应。
 * 注意返回的报文也转换过字节序。
 */
static CityResponseHeader *
request_helper(int socket_fd, uint16_t type, const char *city_name, uint8_t date, CityResponseHeader *response)
{
    CityRequestHeader request;
    construct_request(&request, type, city_name, date);

    if (send(socket_fd, &request, sizeof(request), 0) == -1) {
        perror(MSG_SEND_FAILURE);
        return NULL;
    }

    ssize_t n_read = recv(socket_fd, response, sizeof(*response), 0);

    if (n_read != sizeof(request)) {
        perror(MSG_SEND_FAILURE);
        return NULL;
    }

    return response_ntoh(response);
}

/**
 * @brief 查询城市是否存在
 * @param socket_fd 客户端的套接字
 * @param city_name 城市名
 * @return 成功时返回 0, 失败时返回 -1.
 */
static int query_city_exists(int socket_fd, const char *city_name)
{
    CityResponseHeader response;

    if (request_helper(socket_fd, REQUEST_CITY, city_name, 1, &response) == NULL) {
        return -1;
    }

    if (response.type == RESPONSE_NO_CITY) {
        printf("%s doesn't exist\n", city_name);
        return -1;
    }
    else {
        printf("%s exists\n", city_name);
        return 0;
    }
}

/**
 * @brief 控制台状态
 */
typedef enum {
    QUERY_CITY,
    QUERY_WEATHER,
    EXIT
} MonitorState;

/**
 * @brief 根据请求结果进行响应处理以及状态转移
 * @param socket_fd 客户端套接字
 * @param command   命令内容
 * @return 查询城市成功返回 QUERY_WEATHER, 如果为 # 则返回 EXIT, 其余情况保持 QUERY_CITY 状态
 */
static MonitorState city_query_handler(int socket_fd, const char *command)
{
    if (!strcmp(command, "c")) {
        system("clear");
        return QUERY_CITY;
    }
    else if (!strcmp(command, "#")) {
        return EXIT;
    }
    else {
        if (query_city_exists(socket_fd, command) == 0) {
            return QUERY_WEATHER;
        }
        else {
            return QUERY_CITY;
        }
    }
}

/**
 * @brief 控制台状态机
 * @param socket_fd 客户端套接字
 */
void monitor_main_loop(int socket_fd)
{
    MonitorState state = QUERY_CITY;

    char command[1024];

    while (state != EXIT) {

        fgets(command, sizeof(command), stdin);

        // 城市名带上 \n 就不好了
        // 不过不需要检查 len - 1 的具体内容，因为要么是 \n 要么是 \0
        // 用 \0 写掉 \n 是目的，另一种情况没有影响。
        size_t len = strlen(command);
        command[len - 1] = '\0';

        switch (state) {
            case QUERY_CITY:
                state = city_query_handler(socket_fd, command);
                break;
            case QUERY_WEATHER:
                break;
            default:
                state = EXIT;
        }
    }
}