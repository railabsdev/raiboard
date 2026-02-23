/**
 * @file LoRa.cpp
 * @brief C++ wrapper implementation for LoRa radio driver
 */

#include "LoRa.hpp"
#include <cstring>

/* Static global instance for C -> C++ callback */
static LoRa* g_lora_instance = nullptr;

void LoRa::rx_callback_wrapper_(const uint8_t* payload, uint16_t length, int16_t rssi, int8_t snr) {
    if (g_lora_instance && g_lora_instance->rx_callback_) {
        g_lora_instance->rx_callback_(payload, length, rssi, snr);
    }
}

LoRa::LoRa() : rx_callback_(nullptr) {
    std::memset(&handle_, 0, sizeof(handle_));
    g_lora_instance = this;
}

LoRa::~LoRa() {
    rx_callback_ = nullptr;
}

bool LoRa::init() {
    return lora_init(&handle_);
}

bool LoRa::init(uint32_t frequency_hz, lora_spreading_factor_t spreading_factor) {
    lora_config_t config = lora_get_default_config();
    config.frequency_hz = frequency_hz;
    config.spreading_factor = spreading_factor;
    return lora_init_with_config(&handle_, &config);
}

bool LoRa::init(const lora_config_t& config) {
    return lora_init_with_config(&handle_, &config);
}

void LoRa::poll() {
    lora_poll(&handle_);
}

bool LoRa::send(const uint8_t* data, size_t length) {
    return lora_send(&handle_, data, length);
}

bool LoRa::send(const char* str) {
    if (!str) {
        return false;
    }
    size_t len = std::strlen(str);
    return lora_send(&handle_, reinterpret_cast<const uint8_t*>(str), len);
}

void LoRa::startRx() {
    lora_start_rx(&handle_);
}

bool LoRa::available() const {
    return lora_available(&handle_);
}

size_t LoRa::read(uint8_t* out, size_t max_length) {
    return lora_read(&handle_, out, max_length);
}

bool LoRa::isTxBusy() const {
    return lora_is_tx_busy(&handle_);
}

int16_t LoRa::lastRssi() const {
    return lora_last_rssi(&handle_);
}

int8_t LoRa::lastSnr() const {
    return lora_last_snr(&handle_);
}

void LoRa::setRxCallback(RxCallback callback) {
    rx_callback_ = callback;
    lora_set_rx_callback(&handle_, rx_callback_wrapper_);
}
