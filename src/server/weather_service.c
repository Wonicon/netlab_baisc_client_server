/**
 * @file     weather_service.c
 * @author   whz
 * @brief    天气服务具体实现
 */

#include <stdio.h>
#include <string.h>
#include <server/weather_service.h>
#include <stdlib.h>
#include <unistd.h>
#include <lib/proxy.h>

/**
 * @brief 固定的城市集合
 */
static const char *cities[] = {
    "nanjing",
    "beijing",
    "shanghai",
    "shenzhen"
};


/**
 * @brief 判断给定城市是否存在
 * @param 待判断的城市名
 * @return 如果城市存在，返回 1，否则返回 0
 *
 * 遍历实现
 */
static int query_city(const char *city)
{
    for (long i = 0; i < sizeof(cities) / sizeof(cities[0]); i++) {
        if (!strcmp(city, cities[0])) {
            return 1;
        }
    }

    return 0;
}


/**
 * @brief 天气服务的外层逻辑
 * @param arg 实际上是 Connection 指针，表示连接相关的信息
 * @return 返回 arg 自身
 */
void *weather_service_main_loop(void *arg)
{
    Connection *link = arg;

    fprintf(stderr, "%d: service start\n", link->id);

    ssize_t n_read;
    CityRequestHeader request = {};
    while ((n_read = recv(link->socket_fd, &request, sizeof(request), 0))) {
        if (n_read < 0) {
            perror("Failed to receive");
            exit(-1);
        }

        CityResponseHeader response = {};
        switch (request.type) {
            case REQUEST_CITY:
                response.type = (uint16_t)(query_city(request.city_name) ? RESPONSE_CITY_EXISTS : RESPONSE_NO_CITY);
                break;
            case REQUEST_SINGLE_DAY:
                break;
            case REQUEST_MULTIPLE_DAY:
                break;
            default:
                fprintf(stderr, "%d: unrecognized request type %x\n", link->id, request.type);
                close(link->socket_fd);
                exit(-1);
        }

        // 构造通用部分，转换字节序，发送
        construct_reponse(&response, &request);
        response_hton(&response);
        ssize_t n_send = send(link->socket_fd, &response, sizeof(response), 0);
        if (n_send != sizeof(response)) {
            perror("Failed to send no city response");
        }

        memset(&request, 0, sizeof(request));
    }

    fprintf(stderr, "%d: service end\n", link->id);
    close(link->socket_fd);
    return arg;
}
