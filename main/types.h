#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>

typedef struct __attribute__((__packed__)) {
    uint32_t timestamp;
    float temperature;
    float humidity;
    uint8_t battery_level;
    uint8_t status_flags;
} vaccine_data_t;

#endif
