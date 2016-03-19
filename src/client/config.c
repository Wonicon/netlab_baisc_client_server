/**
 * @file     config.c
 * @author   whz
 * @brief    这个文件存放客户端使用到的相关的常量
 */

#include <stdlib.h>
#include "client/config.h"
#include "lib/proxy.h"

/**
 * @brief 服务器的 IP
 *
 * 这个 IP 地址可以使用通过抓包或者 strace 命令观察到
 */
const char *SERVER_IP = "114.212.191.33";

/**
 * @brief 服务器的端口号
 *
 * 这个端口号同 #SERVER_IP 一样，可以通过抓包或者 strace 命令观察到
 */
const unsigned short SERVER_PORT = 4321;

/**
 * @brief socket() 调用失败时配合 perror() 使用
 */
const char *MSG_SOCKET_FAILURE = "Failed to create socket for client";

/**
 * @brief connect() 调用失败时配合 perror() 使用
 */
const char *MSG_CONNECT_FAILURE = "Failed to connect to server";

/**
 * @brief send() / write() 调用失败时配合 perror() 使用
 */
const char *MSG_SEND_FAILURE = "Failed to send request";

/**
 * @brief 控制台首屏输出信息
 */
const char *GREETING =
    "Welcome to NJUCS Weather Forecast Demo Program!\n"
    "Please input City Name in Chinese pinyin(e.g. nanjing or beijing)\n"
    "(c)cls,(#)exit";

/**
 * @brief 第二屏信息
 */
const char *CITY_HEADER =
    "Please enter the given number to query\n"
    "1.today\n"
    "2.three days from today\n"
    "3.custom day by yourself\n"
    "(r)back,(c)cls,(#)exit\n"
    "===================================================";

/**
 * @brief 指定日期的提示信息，不换行！
 */
const char *REQUEST_CUSTOM_DAY = "Please enter the day number(below 10, e.g. 1 means today):";

/**
 * @brief 输入错误的通用提示
 */
const char *INPUT_ERROR = "input error!";

/**
 * @brief 没有城市的提示信息，此函数不可并发重入
 * @param city 城市名
 * @return 格式化之后的信息
 */
const char *NO_CITY_ERROR_MESSAGE(const char *city)
{
    static char message[1024];
    snprintf(message, sizeof(message), "Sorry, Server does not have weather information for city %s!", city);
    return message;
}

/**
 * @brief 城市信息，此函数不可并发重入
 *
 * 打印天气之前需要打印今天的日期等信息
 *
 * @param city  城市名
 * @param year  年份
 * @param month 月份
 * @param day   日期
 * @return 格式化之后的信息
 */
char *CITY_INFO(const char *city, uint16_t year, uint8_t month, uint8_t day)
{
    size_t size = 1024;
    char *message = malloc(size);
    snprintf(message, size, "City: %s  Today is: %u/%02u/%02u  Weather information is as follows: ",
             city, year, month, day);
    return message;
}


/**
 * @breif weather_type 对应的文本信息
 */
static const char *weather_type_literals[] = {
    [WEATHER_SHOWER] = "shower",
    [WEATHER_CLEAR]  = "clear" ,
    [WEATHER_CLOUDY] = "cloudy",
    [WEATHER_RAIN]   = "rain"  ,
    [WEATHER_FOG]    = "fog"   ,
};

/**
 * @brief 将 weather_type 数值字段转换成对应的字符串，可重入
 * @param weather_type 从报文中获取的
 * @return 返回 weather_type 对应的字符串，如果值异常，返回 NULL。
 */
static const char *weather_to_string(uint8_t weather_type)
{
    if (weather_type >= sizeof(weather_type_literals)) {
        fprintf(stderr, "weather_type %d exceeds the limit, which is %lu\n",
                weather_type, sizeof(weather_type_literals));
        return "N/A";
    }

    return weather_type_literals[weather_type];
}

/**
 * @brief 天气信息
 * @param day          日期序号
 * @param weather      天气
 * @param temperature  气温
 * @param today_enable 是否允许将 day == 1 打印成 Today
 * @return 格式化之后的信息，动态申请，由调用者负责释放
 */
char *WEATHER_INFO(uint8_t day, uint8_t weather, int8_t temperature, int today_enable)
{
    char day_text[256];
    if (today_enable && day == 1) {
        snprintf(day_text, sizeof(day_text), "Today");
    }
    else {
        snprintf(day_text, sizeof(day_text), "The %uth day", day);
    }

    size_t size = 1024;
    char *message = malloc(size);
    snprintf(message, size, "%s's Weather is: %s;  Temp:%d",
            day_text, weather_to_string(weather), temperature);
    return message;
}
