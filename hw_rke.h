#ifndef __HW_RKE_H
#define __HW_RKE_H

#define useRTOS    0U

typedef enum{
    PATH_A = 0,
    PATH_B,
} RkeMsgPath;

typedef enum{
    CHANNEL_0 = 0,
    CHANNEL_1,
    CHANNEL_2
}RkeMsgChannel;

typedef enum{
    SERVICE_0 = 0,
    SERVICE_1,
    SERVICE_2,
    SERVICE_3,
    SERVICE_4,
}RkeMsgService;

typedef struct {
    uint8_t path;
    uint8_t ch;
    uint8_t ser;
    uint8_t data[32];
    uint8_t len;
} RkeMsg;

uint8_t RkeInit(void);
uint8_t RkeRecv(RkeMsg *msg);

#endif

