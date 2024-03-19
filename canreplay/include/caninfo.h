#pragma once

typedef struct {
    unsigned long long time;
    unsigned int id;
    unsigned int len;
    unsigned char data[8];
} CanInfo;
