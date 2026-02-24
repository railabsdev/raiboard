/**
 * @file LoRa.hpp
 * @brief C++ wrapper for LoRa radio driver
 * @author Custom driver
 * 
 * Object-oriented C++ interface for the bare-bones C LoRa driver.
 */

#ifndef LORA_HPP
#define LORA_HPP

#include "lora_radio.h"
#include <functional>
#include <cstdint>
#include <cstddef>

class LoRa {
public:
    using RxCallback = std::function<void(const uint8_t*, size_t, int16_t, int8_t)>;

    /**
     * @brief Constructor
     */
    LoRa();

    /**
     * @brief Destructor
     */
    ~LoRa();

    /**
     * @brief Initialize with default configuration
     * @return true if successful
     */
    bool init();

    /**
     * @brief Initialize with custom frequency and spreading factor
     * @param frequency_hz Frequency in Hz (default 915000000)
     * @param spreading_factor LoRa SF (default 12)
     * @return true if successful
     */
    bool init(uint32_t frequency_hz, lora_spreading_factor_t spreading_factor);

    /**
     * @brief Initialize with full configuration
     * @param config Custom configuration structure
     * @return true if successful
     */
    bool init(const lora_config_t& config);

    /**
     * @brief Poll radio state machine (call this frequently)
     */
    void poll();

    /**
     * @brief Send data
     * @param data Data buffer
     * @param length Data length
     * @return true if transmission started
     */
    bool send(const uint8_t* data, size_t length);

    /**
     * @brief Send C string
     * @param str Null-terminated string
     * @return true if transmission started
     */
    bool send(const char* str);

    /**
     * @brief Start receiving
     */
    void startRx();

    /**
     * @brief Check if data is available
     * @return true if data ready
     */
    bool available() const;

    /**
     * @brief Read received data
     * @param out Output buffer
     * @param max_length Maximum bytes to read
     * @return Number of bytes read
     */
    size_t read(uint8_t* out, size_t max_length);

    /**
     * @brief Check if TX is busy
     * @return true if transmitting
     */
    bool isTxBusy() const;

    /**
     * @brief Get last RSSI
     * @return RSSI in dBm
     */
    int16_t lastRssi() const;

    /**
     * @brief Get last SNR
     * @return SNR in dB
     */
    int8_t lastSnr() const;

    /**
     * @brief Set RX callback
     * @param callback Function to call on RX
     */
    void setRxCallback(RxCallback callback);

    /**
     * @brief Get const reference to C handle
     * @return Reference to underlying lora_handle_t
     */
    const lora_handle_t& getHandle() const { return handle_; }

    /**
     * @brief Get mutable reference to C handle
     * @return Reference to underlying lora_handle_t
     */
    lora_handle_t& getHandle() { return handle_; }

private:
    lora_handle_t handle_;
    RxCallback rx_callback_;

    /* Callback wrapper for C function pointer */
    static void rx_callback_wrapper_(const uint8_t*, uint16_t, int16_t, int8_t);
};

#endif /* LORA_HPP */
