/**
 * @file     weather_service.h
 * @author   whz
 * @brief    服务接口
 */

#ifndef WEATHER_SERVICE_H
#define WEATHER_SERVICE_H

#include <netinet/in.h>
#include <pthread.h>

/**
 * @brief 描述连接状态
 */
typedef struct {
    pthread_t           tid;        /**< 服务线程 ID */
    int                 socket_fd;  /**< 连接套接字 */
    struct sockaddr_in  address;    /**< 客户端 IP 地址 */
    socklen_t           length;     /**< 客户端地址长度 */
} Connection;

/**
 * 服务入口
 */
void *weather_service_main_loop(void *socket_fd);

#endif // WEATHER_SERVICE_H
