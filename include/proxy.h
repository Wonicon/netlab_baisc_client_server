#ifndef PROXY_H
#define PROXY_H

#include <inttypes.h>

#pragma pack(1)
typedef struct {
    uint16_t type;
    char city_name[20];
    uint8_t date;
} CityRequestHeader;

#pragma pack(1)
typedef struct {
    uint16_t type;
    char city_name[20];
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t n_status;
    struct {
        uint8_t weather_type;
        uint8_t temperature;
    } status[25];
} CityResponseHeader;

#endif // PROXY_H
