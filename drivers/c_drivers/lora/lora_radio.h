/**
 * @file lora_radio.h
 * @brief Bare-bones C LoRa radio driver for SX126x
 * @author RaiLabs
 */

#ifndef LORA_RADIO_H
#define LORA_RADIO_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*  Configuration */
#define LORA_DEFAULT_FREQ_HZ        915000000
#define LORA_DEFAULT_TX_POWER       14
#define LORA_DEFAULT_SPREADING_FACTOR 12
#define LORA_MAX_PAYLOAD            255
#define LORA_BUFFER_SIZE            255

/*  LoRa Operating Modes  */
typedef enum {
    LORA_MODE_IDLE,
    LORA_MODE_TX,
    LORA_MODE_RX,
    LORA_MODE_ERROR
} lora_mode_t;

/*  LoRa Bandwidth (MHz)  */
typedef enum {
    LORA_BW_7_8   = 0,
    LORA_BW_10    = 1,
    LORA_BW_15    = 2,
    LORA_BW_20    = 3,
    LORA_BW_31    = 4,
    LORA_BW_41    = 5,
    LORA_BW_62    = 6,
    LORA_BW_125   = 7,
    LORA_BW_250   = 8,
    LORA_BW_500   = 9
} lora_bandwidth_t;

/*  LoRa Spreading Factor  */
typedef enum {
    LORA_SF_5  = 5,
    LORA_SF_6  = 6,
    LORA_SF_7  = 7,
    LORA_SF_8  = 8,
    LORA_SF_9  = 9,
    LORA_SF_10 = 10,
    LORA_SF_11 = 11,
    LORA_SF_12 = 12
} lora_spreading_factor_t;

/*  LoRa Configuration  */
typedef struct {
    uint32_t frequency_hz;
    int8_t tx_power_dbm;
    lora_bandwidth_t bandwidth;
    lora_spreading_factor_t spreading_factor;
    uint8_t coding_rate;
    uint16_t preamble_length;
    uint16_t symbol_timeout;
    bool fix_length_payload;
    bool iq_inverted;
} lora_config_t;

/*  RX Callback  */
typedef void (*lora_rx_callback_t)(const uint8_t* payload, uint16_t length, int16_t rssi, int8_t snr);

/*  LoRa Handle  */
typedef struct {
    bool initialized;
    lora_config_t config;
    lora_mode_t current_mode;
    bool tx_busy;
    bool rx_ready;
    uint8_t rx_buffer[LORA_BUFFER_SIZE];
    size_t rx_length;
    int16_t last_rssi;
    int8_t last_snr;
    lora_rx_callback_t on_rx;
} lora_handle_t;

/*  Public API  */

/**
 * @brief Initialize LoRa radio with default config
 * @param radio Pointer to lora_handle_t
 * @return true if initialization successful
 */
bool lora_init(lora_handle_t* radio);

/**
 * @brief Initialize LoRa radio with custom config
 * @param radio Pointer to lora_handle_t
 * @param config Custom configuration
 * @return true if initialization successful
 */
bool lora_init_with_config(lora_handle_t* radio, const lora_config_t* config);

/**
 * @brief Poll radio state machine (call frequently)
 * @param radio Pointer to lora_handle_t
 */
void lora_poll(lora_handle_t* radio);

/**
 * @brief Send data via LoRa
 * @param radio Pointer to lora_handle_t
 * @param data Pointer to data buffer
 * @param length Length of data
 * @return true if transmission started
 */
bool lora_send(lora_handle_t* radio, const uint8_t* data, size_t length);

/**
 * @brief Start RX mode
 * @param radio Pointer to lora_handle_t
 */
void lora_start_rx(lora_handle_t* radio);

/**
 * @brief Check if data is available
 * @param radio Pointer to lora_handle_t
 * @return true if data is ready to read
 */
bool lora_available(const lora_handle_t* radio);

/**
 * @brief Read received data
 * @param radio Pointer to lora_handle_t
 * @param out Output buffer
 * @param max_length Max bytes to read
 * @return Number of bytes read
 */
size_t lora_read(lora_handle_t* radio, uint8_t* out, size_t max_length);

/**
 * @brief Check if TX is in progress
 * @param radio Pointer to lora_handle_t
 * @return true if TX busy
 */
bool lora_is_tx_busy(const lora_handle_t* radio);

/**
 * @brief Get last RSSI (Received Signal Strength Indicator)
 * @param radio Pointer to lora_handle_t
 * @return RSSI in dBm
 */
int16_t lora_last_rssi(const lora_handle_t* radio);

/**
 * @brief Get last SNR (Signal-to-Noise Ratio)
 * @param radio Pointer to lora_handle_t
 * @return SNR in dB
 */
int8_t lora_last_snr(const lora_handle_t* radio);

/**
 * @brief Set RX callback
 * @param radio Pointer to lora_handle_t
 * @param callback Function to call on RX
 */
void lora_set_rx_callback(lora_handle_t* radio, lora_rx_callback_t callback);

/**
 * @brief Get default config
 * @return Default lora_config_t
 */
lora_config_t lora_get_default_config(void);

#ifdef __cplusplus
}
#endif

#endif /* LORA_RADIO_H */
