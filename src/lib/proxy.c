/**
 * @file     proxy.c
 * @author   whz
 * @brief    协议处理相关的 API
 *
 */

#include "lib/proxy.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
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

/**
 * @brief 根据请求构造响应报文
 * @param response 待构造的响应报文指针，期望已初始化
 * @param request 指向接收的请求报文的指针
 * @return 构造好的响应报文指针（同参数）
 *
 * 报文的类型和天气信息，在服务器的具体业务逻辑中完成。
 * 类型可以配合初始化在构造之前设定。
 */
CityResponseHeader *
construct_response(CityResponseHeader *response, CityRequestHeader *request)
{
    strncpy(response->city_name, request->city_name, sizeof(response->city_name) - 1);
    response->n_status = request->date;

    time_t raw_time = time(NULL);
    struct tm time_info;
    localtime_r(&raw_time, &time_info);  // 并发场景，使用可重入版本

    response->year = (uint16_t )(time_info.tm_year + 1900);
    response->month = (uint8_t)(time_info.tm_mon + 1);
    response->day = (uint8_t)time_info.tm_mday;

    return response;
}

/**
 * @brief 将请求报文中的多字节数值字段从网络字节序转换成主机字节序
 * @param header 指向请求报文的指针
 * @return       转换后的与参数一样的指针
 */
CityRequestHeader *
request_ntoh(CityRequestHeader *header)
{
    header->type = htons(header->type);
    return header;
}

/**
 * @brief 转换响应报文中的字节序
 * @param header 直接接收的，尚未转换字节序的响应报文头指针
 * @return       转换字节序后的响应报文头，与参数相同。
 */
CityResponseHeader *
response_ntoh(CityResponseHeader *header)
{
    header->type = htons(header->type);
    header->year = htons(header->year);
    return header;
}

/**
 * @brief 将响应报文中的多字节数值字段由本机字节序转换成网络字节序
 * @param header 指向响应报文的指针
 * @return       转换后的响应报文指针（与参数相同）
 */
CityResponseHeader *
response_hton(CityResponseHeader *header)
{
    header->type = ntohs(header->type);
    header->year = ntohs(header->year);
    return header;
}
