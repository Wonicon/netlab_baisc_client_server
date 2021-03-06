/**
 * @file     config.h
 * @author   whz
 * @brief    此头文件包含对客户端使用到的所有常量的引用
 */

#ifndef CLIENT_CONFIG_H
#define CLIENT_CONFIG_H

#include <stdio.h>
#include <inttypes.h>

extern const char *SERVER_IP;

extern const unsigned short SERVER_PORT;

extern const char *MSG_SOCKET_FAILURE;

extern const char *MSG_CONNECT_FAILURE;

extern const char *MSG_SEND_FAILURE;

extern const char *GREETING;

extern const char *CITY_HEADER;

extern const char *REQUEST_CUSTOM_DAY;

extern const char *INPUT_ERROR;

extern const char *CUSTOM_DAY_LIMIT;

extern const char *CMD_EXIT;

extern const char *CMD_RETURN;

extern const char *CMD_CLEAR;

extern const char *CMD_CUSTOM_DAY;

extern const char *CMD_THREE_DAY;

extern const char *CMD_TODAY;

const char *NO_CITY_ERROR_MESSAGE(const char *city);

char *CITY_INFO(const char *city, uint16_t year, uint8_t month, uint8_t day);

char *NO_WEATHER(const char *city);

char *WEATHER_INFO(uint8_t day, uint8_t weather, int8_t temperature, int today_enable);

#endif /* CLIENT_CONFIG_H */
