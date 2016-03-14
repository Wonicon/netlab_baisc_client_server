/**
 * @file     proxy.h
 * @author   whz
 * @brief    此头文件包含协议的结构体定义
 */

#ifndef PROXY_H
#define PROXY_H

#include <inttypes.h>

/**
 * @brief 客户端请求通用结构
 */
#pragma pack(1)
typedef struct {
    uint16_t  type;            /**< 请求类型 */
    char      city_name[20];   /**< 城市名称，含终结符 */
    uint8_t   date;            /**< 日期距离，1 = 今天，2 = 昨天，依次类推 */
} CityRequestHeader;

/**
 * @brief 服务器响应通用结构
 */
#pragma pack(1)
typedef struct {
    uint16_t  type;            /**< 响应类型 */
    char      city_name[20];   /**< 城市名称，含终结符 */
    uint16_t  year;            /**< 年份 */
    uint8_t   month;           /**< 月份 */
    uint8_t   day;             /**< 日期 */
    uint8_t   n_status;        /**< 包含的状态数，决定 #status 数据有效元素的个数 */
    struct {
        uint8_t weather_type;  /**< 天气类型 */
        uint8_t temperature;   /**< 温度 */
    } status[25];              /**< 状态数组 */
} CityResponseHeader;

#endif // PROXY_H
