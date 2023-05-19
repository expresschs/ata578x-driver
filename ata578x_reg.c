#include <string.h>
#include "common.h"
#include "ata578x_reg.h"

uint8_t ata578x_system_reset(ata578x_ctx *ctx)
{
    uint8_t ret = ERR_COMMON;

    ctx->delay_us(ATA578X_SPI_IDLE_TELEGRAM_TIME);
    ctx->tx.frm.cmd = ATA578X_CMD_SYSTEM_RESET;
    ctx->tx.len = 1 + 1;
    if (ERR_OK == ctx->transfer(ctx, (const uint8_t *)&ctx->tx.frm, (uint8_t *)&ctx->rx.frm, ctx->tx.len)) {
        ctx->rx.len = ctx->tx.len;
        ctx->delay_us(ATA578X_SYSTEM_RESET_TIME);
        ret = ERR_OK;
    }

    return ret;
}

uint8_t ata578x_get_rom_version(ata578x_ctx *ctx, uint8_t *version)
{
    uint8_t ret = ERR_COMMON;

    ctx->delay_us(ATA578X_SPI_IDLE_TELEGRAM_TIME);
    ctx->tx.frm.cmd = ATA578X_CMD_GET_VERSION_ROM;
    ctx->tx.len = 1 + 2;
    if (ERR_OK == ctx->transfer(ctx, (const uint8_t *)&ctx->tx.frm, (uint8_t *)&ctx->rx.frm, ctx->tx.len)) {
        ctx->rx.len = ctx->tx.len;
        *version = ctx->rx.frm.param[0];
        ret = ERR_OK;
    }

    return ret;
}

uint8_t ata578x_set_idle_mode(ata578x_ctx *ctx)
{
    uint8_t ret = ERR_COMMON;
#if 1 /* already in idle mode */
    ctx->delay_us(ATA578X_SPI_IDLE_TELEGRAM_TIME);
    ctx->tx.frm.cmd = ATA578X_CMD_SET_SYSTEM_MODE; /* to idle mode */
    ctx->tx.frm.param[0] = ATA578X_OPM_IDLE_MODE;
    ctx->tx.frm.param[1] = 0;
    ctx->tx.len = 1 + 2;
    if (ERR_OK == ctx->transfer(ctx, (const uint8_t *)&ctx->tx.frm, (uint8_t *)&ctx->rx.frm, ctx->tx.len)) {
        ctx->rx.len = ctx->tx.len;
        ctx->delay_us(ATA578X_POLLING_TO_IDLE_MODE_TIME);
        ret = ERR_OK;
    }
#endif

    return ret;
}

uint8_t ata578x_set_polling_mode(ata578x_ctx *ctx)
{
    uint8_t ret = ERR_COMMON;

    ctx->delay_us(ATA578X_SPI_IDLE_TELEGRAM_TIME);
    ctx->tx.frm.cmd = ATA578X_CMD_SET_SYSTEM_MODE; /* to idle mode */
    ctx->tx.frm.param[0] = ATA578X_OPM_POLLING_MODE;
    ctx->tx.frm.param[1] = 0;
    ctx->tx.len = 1 + 2;
    if (ERR_OK == ctx->transfer(ctx, (const uint8_t *)&ctx->tx.frm, (uint8_t *)&ctx->rx.frm, ctx->tx.len)) {
        ctx->rx.len = ctx->tx.len;
        //ctx->delay_us(ATA578X_IDLE_TO_POLLING_MODE_TIME);
        ret = ERR_OK;
    }

    return ret;
}

uint8_t ata578x_event_pin(ata578x_ctx *ctx)
{
    uint8_t ret = BOOL_FALSE;

    if (1 == ctx->evt_pin()) {
        ret = BOOL_TRUE;
    }

    return ret;
}

uint8_t ata578x_get_error_code(ata578x_ctx *ctx, uint8_t *code)
{
    uint8_t ret =ERR_COMMON;

    ctx->delay_us(ATA578X_SPI_IDLE_TELEGRAM_TIME);
    ctx->tx.frm.cmd = ATA578X_CMD_READ_SRAM_REGISTER; /* sys err code */
    ctx->tx.frm.param[0] = 0x01;
    ctx->tx.frm.param[1] = 0x03; /* address 0x0300 errorCode */
    ctx->tx.frm.param[2] = 0x00;
    ctx->tx.len = 1 + 7;
    if (ERR_OK == ctx->transfer(ctx, (const uint8_t *)&ctx->tx.frm, (uint8_t *)&ctx->rx.frm, ctx->tx.len)) {
        ctx->rx.len = ctx->tx.len;
        *code = ctx->rx.frm.param[3];
        ret = ERR_OK;
    }

    return ret;
}

uint8_t ata578x_read_event(ata578x_ctx *ctx, ata578x_evt *evt)
{
    uint8_t ret =ERR_COMMON;

    ctx->delay_us(ATA578X_SPI_IDLE_TELEGRAM_TIME);
    ctx->tx.frm.cmd = ATA578X_CMD_GET_EVENT_BYTES; /* clear events */
    ctx->tx.len = 1 + 3;
    if (ERR_OK == ctx->transfer(ctx, (const uint8_t *)&ctx->tx.frm, (uint8_t *)&ctx->rx.frm, ctx->tx.len)) {
        ctx->rx.len = ctx->tx.len;
        evt->system = *(ata578x_evt_system *)&ctx->rx.frm.evt_system;
        evt->events = *(ata578x_evt_events *)&ctx->rx.frm.evt_events;
        evt->power = *(ata578x_evt_power *)&ctx->rx.frm.param[0];
        evt->config = *(ata578x_evt_config *)&ctx->rx.frm.param[1];
        ret = ERR_OK;
    }

    return ret;
}

uint8_t ata578x_read_dfifo_depth(ata578x_ctx *ctx, uint8_t *depth)
{
    uint8_t ret =ERR_COMMON;

    ctx->delay_us(ATA578X_SPI_IDLE_TELEGRAM_TIME);
    ctx->tx.frm.cmd = ATA578X_CMD_READ_FILL_LEVEL_RX_FIFO; /* get dfifo depth */
    ctx->tx.len = 1 + 2;
    if (ERR_OK == ctx->transfer(ctx, (const uint8_t *)&ctx->tx.frm, (uint8_t *)&ctx->rx.frm, ctx->tx.len)) {
        ctx->rx.len = ctx->tx.len;
        *depth = ctx->rx.frm.param[0];
        if (*depth > 0 && *depth <= ATA578X_DFIFO_MAX_DEPTH) {
            ret = ERR_OK;
        }
    }

    return ret;
}

uint8_t ata578x_read_dfifo(ata578x_ctx *ctx, uint8_t *data, uint8_t len)
{
    uint8_t ret = ERR_COMMON;

    if (len <= ATA578X_DFIFO_MAX_DEPTH) {
        ctx->delay_us(ATA578X_SPI_IDLE_TELEGRAM_TIME);
        ctx->tx.frm.cmd = ATA578X_CMD_READ_RX_FIFO; /* read dfifo */
        ctx->tx.frm.param[0] = len;
        ctx->tx.len = 1 + 2 + len;
        if (ERR_OK == ctx->transfer(ctx, (const uint8_t *)&ctx->tx.frm, (uint8_t *)&ctx->rx.frm, ctx->tx.len)) {
            ctx->rx.len = ctx->tx.len;
            memcpy(data, ctx->rx.frm.param + 1, len);
            ret = ERR_OK;
        }
    }

    return ret;
}

