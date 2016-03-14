/**
 * @file     proxy.c
 * @author   whz
 * @brief    协议处理相关的 API
 *
 */

#include "lib/proxy.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

/**
 * @brief 构造请求报文
 *
 * 内部会进行字节序转换
 *
 * @param header      指向报文缓冲区
 * @param type        请求类型
 * @param city_name   城市名称，限制 19 个有效字节
 * @param date        日期距离
 */
CityRequestHeader *
construct_request(CityRequestHeader *header, uint16_t type, const char *city_name, uint8_t date)
{
    memset(header, 0, sizeof(*header));
    header->type = htons(type);
    header->date = date;

    size_t len = strlen(city_name);
    if (len >= sizeof(header->city_name)) {
        fprintf(stderr, "The requested city name %s is too long!\n", header->city_name);
    }
    // 因为之前 memset 过了，所以可以保证 header->city_name 的终结符。
    strncpy(header->city_name, city_name, sizeof(header->city_name) - 1);

    return header;
}