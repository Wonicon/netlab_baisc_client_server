/**
 * @file     monitor.h
 * @author   whz
 * @brief    控制台界面的相关接口
 */

#include "client/monitor.h"
#include "client/config.h"
#include "lib/proxy.h"
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 控制台状态类型
 */
typedef enum {
    QUERY_CITY,
    QUERY_WEATHER,
    EXIT
} MonitorState;

/**
 * @brief 控制台描述
 */
typedef struct {
    MonitorState state;
    int client_socket;
    char city[64];
    char command[1024];
} Monitor;

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
 * @brief 根据请求结果进行响应处理以及状态转移
 * @param monitor_ptr 控制台对象指针
 */
static void city_query_handler(Monitor *monitor_ptr)
{
    if (!strcmp(monitor_ptr->command, "c")) {
        system("clear");
        puts(GREETING);
        monitor_ptr->state = QUERY_CITY;
    }
    else if (!strcmp(monitor_ptr->command, "#")) {
        monitor_ptr->state = EXIT;
    }
    else if (query_city_exists(monitor_ptr->client_socket, monitor_ptr->command) == 0) {
        system("clear");
        puts(CITY_HEADER);
        strncpy(monitor_ptr->city, monitor_ptr->command, sizeof(monitor_ptr->city));
        monitor_ptr->state = QUERY_WEATHER;
    }
    else {
        puts(NO_CITY_ERROR_MESSAGE(monitor_ptr->command));
        monitor_ptr->state = QUERY_CITY;
    }
}

/**
 * @brief 处理具体天气查询
 * @param monitor_ptr 控制台对象指针
 */
static void weather_query_handler(Monitor *monitor_ptr)
{
    CityResponseHeader response = {};

    if (!strcmp(monitor_ptr->command, "c")) {
        system("clear");
        puts(CITY_HEADER);
        monitor_ptr->state = QUERY_WEATHER;
    }
    else if (!strcmp(monitor_ptr->command, "r")) {
        system("clear");
        puts(GREETING);
        monitor_ptr->state = QUERY_CITY;
    }
    else if (!strcmp(monitor_ptr->command, "#")) {
        monitor_ptr->state = EXIT;
    }
    else if (!strcmp(monitor_ptr->command, "1")) {
        request_helper(monitor_ptr->client_socket, REQUEST_SINGLE_DAY, monitor_ptr->city, 1, &response);

        puts(CITY_INFO(response.city_name, response.year, response.month, response.day));
        puts(WEATHER_INFO(1, response.status[0].weather_type, response.status[0].temperature, 1));

        monitor_ptr->state = QUERY_WEATHER;
    }
    else if (!strcmp(monitor_ptr->command, "2")) {
        request_helper(monitor_ptr->client_socket, REQUEST_MULTIPLE_DAY, monitor_ptr->city, 3, &response);

        puts(CITY_INFO(response.city_name, response.year, response.month, response.day));
        for (int i = 0; i < response.n_status; i++) {
            puts(WEATHER_INFO((uint8_t)(i + 1), response.status[i].weather_type, response.status[i].temperature, 0));
        }

        monitor_ptr->state = QUERY_WEATHER;
    }
    else if (!strcmp(monitor_ptr->command, "3")) {
        printf("%s", REQUEST_CUSTOM_DAY);
        int no;
        // 进行错误处理，如果不把输入缓冲区清空，则无法继续使用 scanf
        while (scanf("%d", &no) == 0) {
            puts(INPUT_ERROR);
            // 清空输入缓冲区
            do {
                no = getchar();
            } while (no != '\n' && no != EOF);
            printf("%s", REQUEST_CUSTOM_DAY);
        };

        request_helper(monitor_ptr->client_socket, REQUEST_SINGLE_DAY, monitor_ptr->city, (uint8_t)no, &response);

        puts(CITY_INFO(response.city_name, response.year, response.month, response.day));
        puts(WEATHER_INFO((uint8_t)no, response.status[0].weather_type, response.status[0].temperature, 1));

        monitor_ptr->state = QUERY_WEATHER;
    }
    else {
        puts(INPUT_ERROR);
        monitor_ptr->state = QUERY_WEATHER;
    }
}

/**
 * @brief 控制台状态机
 * @param socket_fd 客户端套接字
 */
void monitor_main_loop(int socket_fd)
{
    Monitor monitor = {
        .state         = QUERY_CITY,
        .client_socket = socket_fd,
    };

    system("clear");
    puts(GREETING);

    while (monitor.state != EXIT) {
        fgets(monitor.command, sizeof(monitor.command), stdin);

        // 城市名带上 \n 就不好了
        size_t len = strlen(monitor.command);
        if (len == 1 || len == 0) {  // Avoid \n left in buffer by scanf above.
            continue;
        }
        if (monitor.command[len - 1] == '\n') {
            monitor.command[len - 1] = '\0';
        }

        switch (monitor.state) {
            case QUERY_CITY:
                city_query_handler(&monitor);
                break;
            case QUERY_WEATHER:
                weather_query_handler(&monitor);
                break;
            default:
                fprintf(stderr, "Unexpected monitor state. Initial the state to QUERY_CITY\n");
                monitor.state = QUERY_CITY;
        }
    }
}
