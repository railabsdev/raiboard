#include "pico/radio_stream.hpp"

#include <string.h>

#include "pico/stdlib.h"

extern "C" {
#include "board.h"
#include "delay.h"
#include "radio.h"
#include "rtc-board.h"
#include "sx126x-board.h"
#include "pico/board-config.h"
}

RadioStream* RadioStream::instance_ = nullptr;

RadioStream::RadioStream() = default;

RadioStream::Config::Config()
    : frequency_hz(915000000),
      tx_power_dbm(14),
      lora_bandwidth(0),
      lora_spreading_factor(7),
      lora_coding_rate(1),
      lora_preamble_len(8),
      lora_symbol_timeout(5),
      lora_fix_length_payload(false),
      lora_iq_inverted(false),
      tx_timeout_ms(3000)
{
}

static void radio_hw_init()
{
    Gpio_t gpio_busy{};
    Gpio_t gpio_dio1{};
    Gpio_t gpio_reset{};
    Gpio_t gpio_nss{};
    Spi_t spi{};

    gpio_busy.pin = static_cast<PinNames>(RADIO_BUSY);
    gpio_dio1.pin = static_cast<PinNames>(RADIO_DIO_1);
    gpio_reset.pin = static_cast<PinNames>(RADIO_RESET);
    gpio_nss.pin = static_cast<PinNames>(RADIO_NSS);

    spi.SpiId = SPI_1;
    spi.Nss = gpio_nss;

    SX126x.BUSY = gpio_busy;
    SX126x.DIO1 = gpio_dio1;
    SX126x.Reset = gpio_reset;
    SX126x.Spi = spi;

        SpiInit(&SX126x.Spi, SPI_1,
            static_cast<PinNames>(RADIO_MOSI),
            static_cast<PinNames>(RADIO_MISO),
            static_cast<PinNames>(RADIO_SCLK),
            NC);
        SX126x.Spi.Nss.pin = static_cast<PinNames>(RADIO_NSS);

    SX126xIoInit();
}

bool RadioStream::init(const Config& config)
{
    if (initialized_)
    {
        return true;
    }

    if (instance_ != nullptr && instance_ != this)
    {
        return false;
    }

    config_ = config;
    instance_ = this;

    RtcInit();
    BoardInitMcu();
    BoardInitPeriph();

    radio_hw_init();

    static RadioEvents_t events;
    memset(&events, 0, sizeof(events));
    events.TxDone = RadioStream::on_tx_done;
    events.TxTimeout = RadioStream::on_tx_timeout;
    events.RxDone = RadioStream::on_rx_done;
    events.RxTimeout = RadioStream::on_rx_timeout;
    events.RxError = RadioStream::on_rx_error;

    Radio.Init(&events);
    Radio.SetChannel(config_.frequency_hz);

    Radio.SetTxConfig(MODEM_LORA, config_.tx_power_dbm, 0, config_.lora_bandwidth,
                      config_.lora_spreading_factor, config_.lora_coding_rate,
                      config_.lora_preamble_len, config_.lora_fix_length_payload,
                      true, 0, 0, config_.lora_iq_inverted, config_.tx_timeout_ms);

    Radio.SetRxConfig(MODEM_LORA, config_.lora_bandwidth, config_.lora_spreading_factor,
                      config_.lora_coding_rate, 0, config_.lora_preamble_len,
                      config_.lora_symbol_timeout, config_.lora_fix_length_payload,
                      0, true, 0, 0, config_.lora_iq_inverted, true);

    Radio.SetMaxPayloadLength(MODEM_LORA, static_cast<uint8_t>(kBufferSize));

    initialized_ = true;
    start_rx();
    return true;
}

bool RadioStream::init()
{
    return init(Config());
}

void RadioStream::poll()
{
    // Non-blocking IRQ processing
    if (Radio.IrqProcess != NULL)
    {
        Radio.IrqProcess();
    }
}

bool RadioStream::send(const uint8_t* data, size_t length)
{
    if (!initialized_ || tx_busy_)
    {
        return false;
    }

    if (length == 0)
    {
        return false;
    }

    if (length > kBufferSize)
    {
        length = kBufferSize;
    }

    tx_busy_ = true;
    last_tx_timeout_ = false;

    Radio.Send(const_cast<uint8_t*>(data), static_cast<uint8_t>(length));
    return true;
}

void RadioStream::start_rx()
{
    if (!initialized_)
    {
        return;
    }

    rx_ready_ = false;
    Radio.Rx(0);
}

bool RadioStream::available() const
{
    return rx_ready_;
}

size_t RadioStream::read(uint8_t* out, size_t max_length)
{
    if (!rx_ready_ || out == nullptr || max_length == 0)
    {
        return 0;
    }

    size_t to_copy = rx_size_ < max_length ? rx_size_ : max_length;
    memcpy(out, rx_buffer_, to_copy);
    rx_ready_ = false;
    rx_size_ = 0;
    return to_copy;
}

bool RadioStream::tx_busy() const
{
    return tx_busy_;
}

bool RadioStream::last_tx_timeout() const
{
    return last_tx_timeout_;
}

int16_t RadioStream::last_rssi() const
{
    return last_rssi_;
}

int8_t RadioStream::last_snr() const
{
    return last_snr_;
}

RadioStream& RadioStream::operator<<(const TxBuffer& buffer)
{
    send(buffer.data, buffer.length);
    return *this;
}

RadioStream& RadioStream::operator>>(RxBuffer& buffer)
{
    buffer.length = read(buffer.data, buffer.max_length);
    return *this;
}

RadioStream& RadioStream::operator<<(const char* text)
{
    if (text != nullptr)
    {
        send(reinterpret_cast<const uint8_t*>(text), strlen(text));
    }
    return *this;
}

void RadioStream::on_tx_done()
{
    if (instance_ != nullptr)
    {
        Radio.Sleep();
        instance_->tx_busy_ = false;
    }
}

void RadioStream::on_tx_timeout()
{
    if (instance_ != nullptr)
    {
        Radio.Sleep();
        instance_->tx_busy_ = false;
        instance_->last_tx_timeout_ = true;
    }
}

void RadioStream::on_rx_done(uint8_t* payload, uint16_t size, int16_t rssi, int8_t snr)
{
    if (instance_ != nullptr)
    {
        Radio.Sleep();
        instance_->handle_rx_done(payload, size, rssi, snr);
    }
}

void RadioStream::on_rx_timeout()
{
    if (instance_ != nullptr)
    {
        Radio.Sleep();
        instance_->start_rx();
    }
}

void RadioStream::on_rx_error()
{
    if (instance_ != nullptr)
    {
        Radio.Sleep();
        instance_->start_rx();
    }
}

void RadioStream::handle_rx_done(uint8_t* payload, uint16_t size, int16_t rssi, int8_t snr)
{
    rx_size_ = (size > kBufferSize) ? kBufferSize : size;
    memcpy(rx_buffer_, payload, rx_size_);
    last_rssi_ = rssi;
    last_snr_ = snr;
    rx_ready_ = true;
}
