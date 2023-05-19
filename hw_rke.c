#include <string.h>
#include "common.h"
#include "spi.h"
#include "ata578x_reg.h"
#include "hw_rke.h"


#if (useRTOS==1)

#include "cmsis_os.h"
#include "user_queue.h"

#define RKE_EOT_EVT_MSK     0x00000001U /* EOT */
#define RKE_ALL_EVT_MSK     0xFFFFFFFFU  

#define RKE_QUEUE_SIZE      (sizeof(RkeMsg))
#define RKE_QUEUE_COUNT     3U

typedef struct {
    ata578x_ctx dev;
    ata578x_evt evt;

    osThreadId_t delay_work;
    osEventFlagsId_t evt_flag;

    queue_type queue;
    uint8_t queue_buf[RKE_QUEUE_SIZE * RKE_QUEUE_COUNT];

    uint8_t err_code;
} rke_ctl;

extern SPI_HandleTypeDef hspi2;
const osThreadAttr_t g_delayWorkAttrs = {
    .name = "rke_delay_work",
    .stack_size = 1024,
    .priority = (osPriority_t) osPriorityRealtime,
};
rke_ctl g_ctl;

static uint8_t platform_evt_pin(void)
{
    /* 1 irq pin */
    return HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6);
}

static void platform_delay_us(uint16_t us)
{
    uint16_t delay = 0;

    if (us < 1000) {
        delay = us * 5;
        do {
            __NOP();__NOP();__NOP();__NOP();
            __NOP();__NOP();__NOP();__NOP();
            __NOP();
        } while (delay --);
    } else {
        osDelay((uint32_t)us/1000);;
    }

    return;
}

static uint8_t platform_spi_transfer(void *handle, const uint8_t *send, uint8_t *recv, uint16_t len)
{
    uint8_t ret = ERR_COMMON;

    if (NULL == send || NULL == recv || 0 == len) {
        goto exit;
    }

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
    if (HAL_OK == HAL_SPI_TransmitReceive(&hspi2, (uint8_t *)send, recv, len, 10)) {
        ret = ERR_OK;
    }
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

exit:
    return ret;
}

static void _rkeDelayWork(void *argument)
{
    RkeMsg msg;
    uint8_t depth = 0;
    int evt = osFlagsError;

    for (;;) {
        /* 1 read eot event flag */
        evt = osEventFlagsWait(g_ctl.evt_flag, RKE_EOT_EVT_MSK, osFlagsWaitAny, osWaitForever);
        if (evt & RKE_EOT_EVT_MSK) {
            /* 2 to idle mode */
            ata578x_set_idle_mode(&g_ctl.dev);
            /* 3 read and clear event */
            if (ERR_OK == ata578x_read_event(&g_ctl.dev, &g_ctl.evt)) {
                if (g_ctl.evt.system.sys_err) {
                    /* 4 get error code */
                    ata578x_get_error_code(&g_ctl.dev, &g_ctl.err_code);
                }
                if (g_ctl.evt.config.patha) {
                    msg.path = PATH_A;
                } else if (g_ctl.evt.config.pathb) {
                    msg.path = PATH_B;
                }
                msg.ch = g_ctl.evt.config.ch;
                msg.ser = g_ctl.evt.config.ser;
                /* 5 read fifo depth */
                if (ERR_OK == ata578x_read_dfifo_depth(&g_ctl.dev , &depth)) {
                    /* 6 read dfifo */
                    if (ERR_OK == ata578x_read_dfifo(&g_ctl.dev, msg.data, depth)) {
                        msg.len = depth;
                        /* 7 put msg to rke queue */
                        queue_put(&g_ctl.queue, &msg);
                    }
                }
            }
            /* 8 to polling mode */
            ata578x_set_polling_mode(&g_ctl.dev);
        }
    }

}

/* irq handler */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    /* 1 set EOT event */
    if (NULL != g_ctl.evt_flag) {
        osEventFlagsSet(g_ctl.evt_flag, RKE_EOT_EVT_MSK);
    }
}

uint8_t RkeInit(void)
{
    uint8_t ret = ERR_COMMON;

    /* 1 memset g_ctl to 0 */
    memset(&g_ctl, 0, sizeof(g_ctl));
    /* 2 handlers */
    g_ctl.dev.transfer = platform_spi_transfer;
    g_ctl.dev.evt_pin = platform_evt_pin;
    g_ctl.dev.delay_us = platform_delay_us;
    /* 3 reset */
    ata578x_system_reset(&g_ctl.dev);
    /* 4 check avilable */
    if (ERR_OK == ata578x_read_event(&g_ctl.dev, &g_ctl.evt)) {
        if (g_ctl.evt.system.sys_rdy) {
            /* 4 queue init */
            queue_init(&g_ctl.queue, g_ctl.queue_buf, RKE_QUEUE_SIZE, RKE_QUEUE_COUNT);
            g_ctl.evt_flag = osEventFlagsNew(NULL);
            osEventFlagsClear(g_ctl.evt_flag, RKE_ALL_EVT_MSK);
            g_ctl.delay_work = osThreadNew(_rkeDelayWork, NULL, &g_delayWorkAttrs);
            ret = ERR_OK;
        }
    }

    return ret;
}

uint8_t RkeRecv(RkeMsg *msg)
{
    uint8_t ret = ERR_COMMON;

    if (1 == queue_get(&g_ctl.queue, msg)) {
        ret = ERR_OK;
    }

    return ret;
}

#else

typedef struct {
    ata578x_ctx dev;
    ata578x_evt evt;

    uint8_t err_code;
} rke_ctl;

rke_ctl g_ctl;
extern SPI_HandleTypeDef hspi2;

static uint8_t platform_evt_pin(void)
{
    return HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_5);
}

static void platform_delay_us(uint16_t us)
{
    uint16_t delay = 0;

    if (us < 1000) {
        delay = us * 5;
        do {
            __NOP();__NOP();__NOP();__NOP();
            __NOP();__NOP();__NOP();__NOP();
            __NOP();
        } while (delay --);
    } else {
        HAL_Delay((uint32_t)us/1000);
    }

    return;
}

static uint8_t platform_spi_transfer(void *handle, const uint8_t *send, uint8_t *recv, uint16_t len)
{
    uint8_t ret = ERR_COMMON;

    if (NULL == send || NULL == recv || 0 == len) {
        goto exit;
    }

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
    if (HAL_OK == HAL_SPI_TransmitReceive(&hspi2, (uint8_t *)send, recv, len, 10)) {
        ret = ERR_OK;
    }
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

exit:
    return ret;
}

uint8_t RkeInit(void)
{
    uint8_t ret = ERR_COMMON;

    /* 1 memset g_ctl to 0 */
    memset(&g_ctl, 0, sizeof(g_ctl));
    /* 2 handlers */
    g_ctl.dev.transfer = platform_spi_transfer;
    g_ctl.dev.evt_pin = platform_evt_pin;
    g_ctl.dev.delay_us = platform_delay_us;
    /* 3 reset */
    ata578x_system_reset(&g_ctl.dev);
    /* 4 check avilable */
    if (ERR_OK == ata578x_read_event(&g_ctl.dev, &g_ctl.evt)) {
        if (g_ctl.evt.system.sys_rdy) {
            ret  = ERR_OK;
        }
    }

    return ret;
}

uint8_t RkeRecv(RkeMsg *msg)
{
    uint8_t ret = ERR_COMMON;
    uint8_t depth = 0;

    /* 1 read event pin */
    if (BOOL_TRUE == ata578x_event_pin(&g_ctl.dev)) {
        /* 2 to idle mode */
        ata578x_set_idle_mode(&g_ctl.dev);
        /* 3 read and clear event */
        if (ERR_OK == ata578x_read_event(&g_ctl.dev, &g_ctl.evt)) {
            if (g_ctl.evt.system.sys_err) {
                /* 4 get error code */
                ata578x_get_error_code(&g_ctl.dev, &g_ctl.err_code);
            }
            if (g_ctl.evt.config.patha) {
                msg->path = PATH_A;
            } else if (g_ctl.evt.config.pathb) {
                msg->path = PATH_B;
            }
            msg->ch = g_ctl.evt.config.ch;
            msg->ser = g_ctl.evt.config.ser;
            /* 5 read fifo depth */
            if (ERR_OK == ata578x_read_dfifo_depth(&g_ctl.dev , &depth)) {
                /* 6 read dfifo */
                if (ERR_OK == ata578x_read_dfifo(&g_ctl.dev, msg->data, depth)) {
                    msg->len = depth;
                    ret = ERR_OK;
                }
            }
        }
        /* 8 to polling mode */
        ata578x_set_polling_mode(&g_ctl.dev);
    }

    return ret;
}

#endif

