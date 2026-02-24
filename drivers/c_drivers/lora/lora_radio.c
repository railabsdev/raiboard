/**
 * @file lora_radio.c
 * @brief Bare-bones C LoRa radio driver implementation
 * 
 * This driver wraps the SX126x LoRaMac-node library with a clean C API.
 * It handles initialization, configuration, TX/RX, and polling.
 */

#include "lora_radio.h"
#include <string.h>

/* External LoRaMac-node radio functions */
extern void Radio_Init(void);
extern void Radio_SetChannel(uint32_t freq);
extern void Radio_SetTxConfig(int8_t power, uint8_t bandwidth, 
                              uint8_t spreading_factor, uint8_t coding_rate,
                              uint16_t preamble_len, bool fix_length, bool iq_inverted);
extern void Radio_SetRxConfig(uint8_t bandwidth, uint8_t spreading_factor, 
                              uint8_t coding_rate, uint16_t preamble_len,
                              uint16_t symbol_timeout, bool fix_length, bool iq_inverted);
extern void Radio_Send(const uint8_t* data, uint8_t length);
extern void Radio_Rx(uint32_t timeout);
extern void Radio_Sleep(void);
extern bool Radio_IsReady(void);
extern void Radio_IrqProcess(void);
extern uint16_t Radio_GetStatus(void);

/* Static global instance for callbacks */
static lora_handle_t* g_lora_instance = NULL;

/* IRQ Handlers */
static void lora_on_tx_done(void) {
    if (g_lora_instance) {
        g_lora_instance->tx_busy = false;
        g_lora_instance->current_mode = LORA_MODE_IDLE;
        Radio_Sleep();
    }
}

static void lora_on_rx_done(uint8_t* payload, uint16_t size, int16_t rssi, int8_t snr) {
    if (g_lora_instance) {
        size_t copy_len = (size > LORA_BUFFER_SIZE) ? LORA_BUFFER_SIZE : size;
        memcpy(g_lora_instance->rx_buffer, payload, copy_len);
        g_lora_instance->rx_length = copy_len;
        g_lora_instance->last_rssi = rssi;
        g_lora_instance->last_snr = snr;
        g_lora_instance->rx_ready = true;
        g_lora_instance->current_mode = LORA_MODE_IDLE;
        
        if (g_lora_instance->on_rx) {
            g_lora_instance->on_rx(payload, size, rssi, snr);
        }
    }
}

static void lora_on_rx_timeout(void) {
    if (g_lora_instance) {
        Radio_Sleep();
        lora_start_rx(g_lora_instance);
    }
}

static void lora_on_rx_error(void) {
    if (g_lora_instance) {
        Radio_Sleep();
        lora_start_rx(g_lora_instance);
    }
}

static void lora_on_tx_timeout(void) {
    if (g_lora_instance) {
        g_lora_instance->tx_busy = false;
        g_lora_instance->current_mode = LORA_MODE_IDLE;
        Radio_Sleep();
    }
}

/* ===== API Implementation ===== */

lora_config_t lora_get_default_config(void) {
    lora_config_t config = {
        .frequency_hz = LORA_DEFAULT_FREQ_HZ,
        .tx_power_dbm = LORA_DEFAULT_TX_POWER,
        .bandwidth = LORA_BW_125,
        .spreading_factor = LORA_DEFAULT_SPREADING_FACTOR,
        .coding_rate = 1,  // 4/5
        .preamble_length = 8,
        .symbol_timeout = 5,
        .fix_length_payload = false,
        .iq_inverted = false
    };
    return config;
}

bool lora_init(lora_handle_t* radio) {
    lora_config_t config = lora_get_default_config();
    return lora_init_with_config(radio, &config);
}

bool lora_init_with_config(lora_handle_t* radio, const lora_config_t* config) {
    if (!radio || !config) {
        return false;
    }

    if (radio->initialized) {
        return true;
    }

    /* Set global instance for callbacks */
    g_lora_instance = radio;

    /* Initialize state */
    radio->config = *config;
    radio->initialized = true;
    radio->current_mode = LORA_MODE_IDLE;
    radio->tx_busy = false;
    radio->rx_ready = false;
    radio->rx_length = 0;
    radio->last_rssi = 0;
    radio->last_snr = 0;
    radio->on_rx = NULL;

    /* Initialize LoRaMac-node radio */
    Radio_Init();

    /* Set channel */
    Radio_SetChannel(config->frequency_hz);

    /* Configure TX */
    Radio_SetTxConfig(
        config->tx_power_dbm,
        config->bandwidth,
        config->spreading_factor,
        config->coding_rate,
        config->preamble_length,
        config->fix_length_payload,
        config->iq_inverted
    );

    /* Configure RX */
    Radio_SetRxConfig(
        config->bandwidth,
        config->spreading_factor,
        config->coding_rate,
        config->preamble_length,
        config->symbol_timeout,
        config->fix_length_payload,
        config->iq_inverted
    );

    return true;
}

void lora_poll(lora_handle_t* radio) {
    if (!radio || !radio->initialized) {
        return;
    }

    /* Process radio interrupts */
    Radio_IrqProcess();
}

bool lora_send(lora_handle_t* radio, const uint8_t* data, size_t length) {
    if (!radio || !radio->initialized || radio->tx_busy) {
        return false;
    }

    if (length == 0 || length > LORA_MAX_PAYLOAD) {
        return false;
    }

    radio->tx_busy = true;
    radio->current_mode = LORA_MODE_TX;

    /* Send via LoRaMac-node */
    Radio_Send((uint8_t*)data, (uint8_t)length);

    return true;
}

void lora_start_rx(lora_handle_t* radio) {
    if (!radio || !radio->initialized) {
        return;
    }

    radio->rx_ready = false;
    radio->current_mode = LORA_MODE_RX;
    Radio_Rx(0);
}

bool lora_available(const lora_handle_t* radio) {
    if (!radio || !radio->initialized) {
        return false;
    }

    return radio->rx_ready;
}

size_t lora_read(lora_handle_t* radio, uint8_t* out, size_t max_length) {
    if (!radio || !out || max_length == 0 || !radio->rx_ready) {
        return 0;
    }

    size_t to_copy = (radio->rx_length < max_length) ? radio->rx_length : max_length;
    memcpy(out, radio->rx_buffer, to_copy);

    radio->rx_ready = false;
    radio->rx_length = 0;

    return to_copy;
}

bool lora_is_tx_busy(const lora_handle_t* radio) {
    if (!radio || !radio->initialized) {
        return false;
    }

    return radio->tx_busy;
}

int16_t lora_last_rssi(const lora_handle_t* radio) {
    if (!radio || !radio->initialized) {
        return 0;
    }

    return radio->last_rssi;
}

int8_t lora_last_snr(const lora_handle_t* radio) {
    if (!radio || !radio->initialized) {
        return 0;
    }

    return radio->last_snr;
}

void lora_set_rx_callback(lora_handle_t* radio, lora_rx_callback_t callback) {
    if (!radio) {
        return;
    }

    radio->on_rx = callback;
}
