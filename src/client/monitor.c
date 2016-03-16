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
#include <lib/proxy.h>

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

    if (n_read != sizeof(*response)) {
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
        return -1;
    }
    else {
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
static MonitorState city_query_handler(int socket_fd, char city_name[], size_t city_name_len, const char *command)
{
    if (!strcmp(command, "c")) {
        system("clear");
        puts(GREETING);
        return QUERY_CITY;
    }
    else if (!strcmp(command, "#")) {
        return EXIT;
    }
    else {
        if (query_city_exists(socket_fd, command) == 0) {
            system("clear");
            puts(CITY_HEADER);
            strncpy(city_name, command, city_name_len);
            return QUERY_WEATHER;
        }
        else {
            puts(NO_CITY_ERROR_MESSAGE(command));
            return QUERY_CITY;
        }
    }
}

static MonitorState weather_query_handler(int socket_fd, char city_name[], size_t city_name_len, const char *command)
{
    if (!strcmp(command, "c")) {
        system("clear");
        puts(CITY_HEADER);
        return QUERY_WEATHER;
    }

    if (!strcmp(command, "r")) {
        system("clear");
        puts(GREETING);
        return QUERY_CITY;
    }

    if (!strcmp(command, "#")) {
        return EXIT;
    }

    CityResponseHeader response = {};

    if (!strcmp(command, "1")) {
        request_helper(socket_fd, REQUEST_SINGLE_DAY, city_name, 1, &response);
        puts(CITY_INFO(response.city_name, response.year, response.month, response.day));
        puts(WEATHER_INFO(1, "N/A", response.status[0].temperature, 1));
        return QUERY_WEATHER;
    }

    if (!strcmp(command, "2")) {
        request_helper(socket_fd, REQUEST_MULTIPLE_DAY, city_name, 3, &response);
        puts(CITY_INFO(response.city_name, response.year, response.month, response.day));
        for (int i = 0; i < response.n_status; i++) {
            puts(WEATHER_INFO((uint8_t)i, "N/A", response.status[i].temperature, 0));
        }
        return QUERY_WEATHER;
    }

    if (!strcmp(command, "3")) {
        printf("%s", REQUEST_CUSTOM_DAY);
        int no;
        while (scanf("%d", &no) == 0) {
            puts(INPUT_ERROR);
            do {
                no = getchar();
            } while (no != '\n' && no != EOF);
            printf("%s", REQUEST_CUSTOM_DAY);
        };
        request_helper(socket_fd, REQUEST_SINGLE_DAY, city_name, (uint8_t)no, &response);
        puts(CITY_INFO(response.city_name, response.year, response.month, response.day));
        puts(WEATHER_INFO((uint8_t)no, "N/A", response.status[0].temperature, 1));
        return QUERY_WEATHER;
    }

    puts(INPUT_ERROR);

    return QUERY_WEATHER;
}

/**
 * @brief 控制台状态机
 * @param socket_fd 客户端套接字
 */
void monitor_main_loop(int socket_fd)
{
    MonitorState state = QUERY_CITY;

    system("clear");
    puts(GREETING);

    char city_name[64];
    char command[1024];

    while (state != EXIT) {

        fgets(command, sizeof(command), stdin);

        // 城市名带上 \n 就不好了
        size_t len = strlen(command);
        if (len == 1 || len == 0) {  // Avoid \n left in buffer by scanf above.
            continue;
        }
        if (command[len - 1] == '\n') {
            command[len - 1] = '\0';
        }

        switch (state) {
            case QUERY_CITY:
                state = city_query_handler(socket_fd, city_name, sizeof(city_name), command);
                break;
            case QUERY_WEATHER:
                state = weather_query_handler(socket_fd, city_name, sizeof(city_name), command);
                break;
            default:
                state = EXIT;
        }
    }
}