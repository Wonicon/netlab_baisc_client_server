/**
 * @file     config.h
 * @author   whz
 * @brief    此头文件包含对客户端使用到的所有常量的引用
 */

#ifndef CLIENT_CONFIG_H
#define CLIENT_CONFIG_H

#include <stdio.h>

extern const char *SERVER_IP;

extern const unsigned short SERVER_PORT;

extern const char *MSG_SOCKET_FAILURE;

extern const char *MSG_CONNECT_FAILURE;

extern const char *MSG_SEND_FAILURE;

#endif /* CLIENT_CONFIG_H */
