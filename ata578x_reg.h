#ifndef __ATA578X_REG_H
#define __ATA578X_REG_H

#define DRV_LITTLE_ENDIAN                               1U

#define ATA578X_MAX_TRANSFER_SIZE                       64U
#define ATA578X_SPI_IDLE_TELEGRAM_TIME                  20U      /* 16.1us Internal data processing */
#define ATA578X_POLLING_TO_IDLE_MODE_TIME               200U     /* PollingMode->IDLEMode(RC) 150(Min)~230us(max) */
#define ATA578X_IDLE_TO_POLLING_MODE_TIME               1000U    /* IDLEMode(RC)->PollingMode 740us(Min)~1090us(max) */
#define ATA578X_SYSTEM_RESET_TIME                       6000U    /* 6ms */
#define ATA578X_DFIFO_MAX_DEPTH                         32U      /* */


#define ATA578X_CMD_READ_FILL_LEVEL_RX_FIFO             0x01U
#define ATA578X_CMD_READ_FILL_LEVEL_RSSI_FIFO           0x03U
#define ATA578X_CMD_GET_EVENT_BYTES                     0x04U
#define ATA578X_CMD_READ_RSSI_FIFO                      0x05U
#define ATA578X_CMD_READ_RX_FIFO                        0x06U
#define ATA578X_CMD_WRITE_SRAM_REGISTER                 0x07U
#define ATA578X_CMD_READ_SRAM_REGISTER                  0x08U
#define ATA578X_CMD_WRITE_EEPROM                        0x09U
#define ATA578X_CMD_READ_EEPROM                         0x0AU
#define ATA578X_CMD_SET_SYSTEM_MODE                     0x0DU
#define ATA578X_CMD_CALIBRATE_AND_CHECK                 0x0EU
#define ATA578X_CMD_PATCH_SPI                           0x0FU
#define ATA578X_CMD_SYSTEM_RESET_ROM                    0x10U
#define ATA578X_CMD_GET_VERSION_ROM                     0x12U
#define ATA578X_CMD_GET_VERSION_FLASH                   0x13U
#define ATA578X_CMD_CUSTOMER_CONFIGURABLE_COMMAND       0x14U
#define ATA578X_CMD_SYSTEM_RESET                        0x15U
#define ATA578X_CMD_TRIGER_EEPROM_SECURE_WRITE          0x16U
#define ATA578X_CMD_SET_VOLTAGE_MONITOR                 0x17U
#define ATA578X_CMD_OFF_COMMAND                         0x18U
#define ATA578X_CMD_READ_TEMPERATURE_VALUE              0x19U
#define ATA578X_CMD_INIT_SRAM_SERVICE                   0x1AU
#define ATA578X_CMD_START_RSSI_MEASUREMENT              0x1BU
#define ATA578X_CMD_GET_RSSI_RSSI_VALUE                 0x1CU
#define ATA578X_CMD_READ_RX_FIFO_BYTE_INTERRUPT         0x1DU
#define ATA578X_CMD_READ_RSSI_FIFO_BYTE_INTERRUPT       0x1EU

#define ATA578X_OPM_IDLE_MODE                           0x00U
#define ATA578X_OPM_RESERVED                            0x01U
#define ATA578X_OPM_RX_MODE                             0x02U
#define ATA578X_OPM_POLLING_MODE                        0x03U

typedef enum {
    ERROR_CODE_SYSTEM_ERROR_EEPROM_NOT_VALID          = 0,
    ERROR_CODE_SETIDLEMODE_MISS_SSMRDY,
    ERROR_CODE_SETIDLEMODE_TIMER_LOCKED,
    ERROR_CODE_RX_STATESTARTSSM_TIMER_LOCKED,
    ERROR_CODE_RX_STATEWAIT4SSMSTATE_GETTELEGRAM_TIMEOUT,
    ERROR_CODE_RESERVED_01,
    ERROR_CODE_RESERVED_02,
    ERROR_CODE_RESERVED_03,
    ERROR_CODE_POLL_STATEWAIT4SSMSTATE_GETTELEGRAM_TIMEOUT,
    ERROR_CODE_RESERVED_04,
    ERROR_CODE_RESERVED_05,
    ERROR_CODE_VCOCAL_STATESTARTSSM_TIMER_LOCKED,
    ERROR_CODE_VCOCAL_STATEWAIT4SSMRDY_TIMEOUT,
    ERROR_CODE_SHUTDOWNTRX_TIMER_LOCKED,
    ERROR_CODE_SHUTDOWNTRX_TIMEOUT,
    ERROR_CODE_POLLING_TIMER1_LOCKED,
    ERROR_CODE_INVALID_OPM_SWITCHING,
    ERROR_CODE_INVALID_OPM_MODE_DURING_TUNE_AND_CHECK,
    ERROR_CODE_RESERVED_06,
    ERROR_CODE_SERVICE_INIT_FAILURE,
    ERROR_CODE_DFIFO_OVER_UNDER_FLOW,
    ERROR_CODE_SFIFO_OVER_UNDER_FLOW,
    ERROR_CODE_RSSI_STATESTARTSSM_TIMER_LOCKED,
    ERROR_CODE_SRC_FRC_CALIB_FAILED,
    ERROR_CODE_GETRXTELEGRAM_SSM_ERROR,
    ERROR_CODE_TEMPMEAS_STATESTARTSSM_TIMER_LOCKED,
    ERROR_CODE_TEMPMEAS_STATEWAIT4SSMRDY_TIMEOUT,
    ERROR_CODE_AVCCLOW_TIMEOUT,
    ERROR_CODE_SUBCHAN_STATESTARTSSM_CENTERFREQ_TIMER_LOCKED,
    ERROR_CODE_SUBCHAN_STATESTARTSSM_LOWERFREQ_TIMER_LOCKED,
    ERROR_CODE_SUBCHAN_STATESTARTSSM_UPPERFREQ_TIMER_LOCKED,
    ERROR_CODE_SUBCHAN_STATESTARTSSM_CENTERFREQ_TIMEOUT,
    ERROR_CODE_SUBCHAN_STATESTARTSSM_LOWERFREQ_TIMEOUT,
    ERROR_CODE_SUBCHAN_STATESTARTSSM_UPPERFREQ_TIMEOUT,
    ERROR_CODE_NONE
} ata578x_err_code;

typedef enum {
    OFF_MODE = 0,
    IDLE_MODE_RC,
    IDLE_MODE_XTO,
    RX_MODE,
    POLLING_MODE_RX,
    POLLING_MODE_XTO
} ata578x_mode;

/* LITTLE_ENDIAN */
#if defined(DRV_LITTLE_ENDIAN)
typedef struct
{
    uint8_t reserved                : 1;
    uint8_t dfifo_rx                : 1;
    uint8_t sfifo                   : 1;
    uint8_t lowbatt                 : 1;
    uint8_t avcclow                 : 1;
    uint8_t sys_rdy                 : 1;
    uint8_t cmd_rdy                 : 1;
    uint8_t sys_err                 : 1;
} ata578x_evt_system;

typedef struct
{
    uint8_t eotb                    : 1;
    uint8_t sotb                    : 1;
    uint8_t wcokb                   : 1;
    uint8_t idchkb                  : 1;
    uint8_t eota                    : 1;
    uint8_t sota                    : 1;
    uint8_t wcoka                   : 1;
    uint8_t idchka                  : 1;
} ata578x_evt_events;

typedef struct
{
    uint8_t npwron1                 : 1;
    uint8_t npwron2                 : 1;
    uint8_t npwron3                 : 1;
    uint8_t npwron4                 : 1;
    uint8_t npwron5                 : 1;
    uint8_t npwron6                 : 1;
    uint8_t reserved                : 1;
    uint8_t pwron                   : 1;
} ata578x_evt_power;

typedef struct
{
    uint8_t ser                     : 3;
    uint8_t reserved                : 1;
    uint8_t ch                      : 2;
    uint8_t patha                   : 1;
    uint8_t pathb                   : 1;
} ata578x_evt_config;

#else

typedef struct
{
    uint8_t sys_err                   : 1;
    uint8_t cmd_rdy                   : 1;
    uint8_t sys_rdy                   : 1;
    uint8_t avcclow                   : 1;
    uint8_t lowbatt                   : 1;
    uint8_t sfifo                     : 1;
    uint8_t dfifo_rx                  : 1;
    uint8_t reserved                  : 1;
} ata578x_evt_system;

typedef struct
{
    uint8_t idchka                    : 1;
    uint8_t wcoka                     : 1;
    uint8_t sota                      : 1;
    uint8_t eota                      : 1;
    uint8_t idchkb                    : 1;
    uint8_t wcokb                     : 1;
    uint8_t sotb                      : 1;
    uint8_t eotb                      : 1;
} ata578x_evt_events;

typedef struct
{
    uint8_t pwron                     : 1;
    uint8_t reserved                  : 1;
    uint8_t npwron6                   : 1;
    uint8_t npwron5                   : 1;
    uint8_t npwron4                   : 1;
    uint8_t npwron3                   : 1;
    uint8_t npwron2                   : 1;
    uint8_t npwron1                   : 1;
} ata578x_evt_power;

typedef struct
{
    uint8_t pathb                     : 1;
    uint8_t patha                     : 1;
    uint8_t ch                        : 2;
    uint8_t reserved                  : 1;
    uint8_t ser                       : 3;
} ata578x_evt_config;

#endif

typedef struct
{
    ata578x_evt_system system;
    ata578x_evt_events events;
    ata578x_evt_power power;
    ata578x_evt_config config;
} ata578x_evt;

typedef uint8_t (*dev_transfer)(void *, const uint8_t *, uint8_t *, uint16_t);
typedef uint8_t (*dev_evt)(void);
typedef void (*dev_delay)(uint16_t);

typedef struct {
    struct {
        uint8_t cmd;
        uint8_t param[ATA578X_MAX_TRANSFER_SIZE - 1];
    } frm;
    uint8_t len;
} dev_tx;

typedef struct {
    struct {
        uint8_t evt_system;
        uint8_t evt_events;
        uint8_t param[ATA578X_MAX_TRANSFER_SIZE - 2];
    } frm;
    uint8_t len;
} dev_rx;

typedef struct
{
    void *handle;

    dev_transfer transfer;
    dev_evt evt_pin;
    dev_delay delay_us;

    dev_tx tx;
    dev_rx rx;
} ata578x_ctx;

uint8_t ata578x_system_reset(ata578x_ctx *ctx);
uint8_t ata578x_get_rom_version(ata578x_ctx *ctx, uint8_t *version);
uint8_t ata578x_set_idle_mode(ata578x_ctx *ctx);
uint8_t ata578x_set_polling_mode(ata578x_ctx *ctx);
uint8_t ata578x_event_pin(ata578x_ctx *ctx);
uint8_t ata578x_get_error_code(ata578x_ctx *ctx, uint8_t *code);
uint8_t ata578x_read_event(ata578x_ctx *ctx, ata578x_evt *evt);
uint8_t ata578x_read_dfifo_depth(ata578x_ctx *ctx, uint8_t *depth);
uint8_t ata578x_read_dfifo(ata578x_ctx *ctx, uint8_t *data, uint8_t len);

#endif

