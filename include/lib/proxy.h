/**
 * @file     proxy.h
 * @author   whz
 * @brief    此头文件包含协议的结构体定义以及相关 API
 */

#ifndef PROXY_H
#define PROXY_H

#include <inttypes.h>

#define REQUEST_CITY          0x0101
#define REQUEST_SINGLE_DAY    0x0201
#define REQUEST_MULTIPLE_DAY  0x0202

#define RESPONSE_CITY_EXISTS  0x0100
#define RESPONSE_NO_CITY      0x0200
#define RESPONSE_SINGLE_DAY   0x0341
#define RESPONSE_MULTIPLE_DAY 0x0342
#define RESPONSE_NO_DAY       0x0441

/**
 * @brief 客户端请求通用结构
 */
#pragma pack(push, 1)
typedef struct {
    uint16_t  type;            /**< 请求类型 */
    char      city_name[20];   /**< 城市名称，含终结符 */
    uint8_t   date;            /**< 日期距离，1 = 今天，2 = 昨天，依次类推 */
} CityRequestHeader;
#pragma pack(pop)

/**
 * @brief 服务器响应通用结构
 */
#pragma pack(push, 1)
typedef struct {
    uint16_t  type;            /**< 响应类型 */
    char      city_name[20];   /**< 城市名称，含终结符 */
    uint16_t  year;            /**< 年份 */
    uint8_t   month;           /**< 月份 */
    uint8_t   day;             /**< 日期 */
    uint8_t   n_status;        /**< 包含的状态数，决定 #status 数据有效元素的个数 */
    struct {
        uint8_t weather_type;  /**< 天气类型 */
        int8_t temperature;    /**< 温度 */
    } status[25];              /**< 状态数组 */
} CityResponseHeader;
#pragma pack(pop)

/**
 * @brief weather_type 的值对应的枚举值
 *
 * 将 weather_type 的值语义化
 */
typedef enum {
    WEATHER_SHOWER,
    WEATHER_CLEAR,
    WEATHER_CLOUDY,
    WEATHER_RAIN,
    WEATHER_FOG,
    NR_WEATHER
} WeatherType;

CityRequestHeader *construct_request(CityRequestHeader *header, uint16_t type, const char *city_name, uint8_t date);

/*
 * 根据请求设定响应报文，自动获取时间。
 */
CityResponseHeader *construct_response(CityResponseHeader *response, CityRequestHeader *request);

/*
 * 将请求报文从网络字节序转换成主机字节序
 */
CityRequestHeader *request_ntoh(CityRequestHeader *header);

CityResponseHeader *response_ntoh(CityResponseHeader *header);

/*
 * 将响应报文中的多字节数值字段由本机字节序转换成网络字节序
 */
CityResponseHeader *response_hton(CityResponseHeader *header);

#endif // PROXY_H
