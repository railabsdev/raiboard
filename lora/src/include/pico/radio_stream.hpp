#ifndef PICO_RADIO_STREAM_HPP
#define PICO_RADIO_STREAM_HPP

#include <cstddef>
#include <cstdint>

class RadioStream {
public:
    struct Config {
        uint32_t frequency_hz;
        int8_t tx_power_dbm;
        uint8_t lora_bandwidth;
        uint8_t lora_spreading_factor;
        uint8_t lora_coding_rate;
        uint16_t lora_preamble_len;
        uint16_t lora_symbol_timeout;
        bool lora_fix_length_payload;
        bool lora_iq_inverted;
        uint32_t tx_timeout_ms;

        Config();
    };

    struct TxBuffer {
        const uint8_t* data;
        size_t length;
    };

    struct RxBuffer {
        uint8_t* data;
        size_t max_length;
        size_t length;
    };

    RadioStream();
    bool init();
    bool init(const Config& config);
    void poll();

    bool send(const uint8_t* data, size_t length);
    void start_rx();

    bool available() const;
    size_t read(uint8_t* out, size_t max_length);

    bool tx_busy() const;
    bool last_tx_timeout() const;
    int16_t last_rssi() const;
    int8_t last_snr() const;

    RadioStream& operator<<(const TxBuffer& buffer);
    RadioStream& operator>>(RxBuffer& buffer);
    RadioStream& operator<<(const char* text);

private:
    static void on_tx_done();
    static void on_tx_timeout();
    static void on_rx_done(uint8_t* payload, uint16_t size, int16_t rssi, int8_t snr);
    static void on_rx_timeout();
    static void on_rx_error();

    void handle_rx_done(uint8_t* payload, uint16_t size, int16_t rssi, int8_t snr);

    static RadioStream* instance_;

    Config config_;
    bool initialized_ = false;
    bool tx_busy_ = false;
    bool last_tx_timeout_ = false;
    bool rx_ready_ = false;
    int16_t last_rssi_ = 0;
    int8_t last_snr_ = 0;

    static constexpr size_t kBufferSize = 255;
    uint8_t rx_buffer_[kBufferSize];
    size_t rx_size_ = 0;
};

#endif // PICO_RADIO_STREAM_HPP
