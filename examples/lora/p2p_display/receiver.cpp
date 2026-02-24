#include <cstdint>
#include <cstdio>
#include <cstring>

#include "pico/stdlib.h"
#include "pico/radio_stream.hpp"
#include "displaylib_16/ili9341.hpp"

extern "C" {
#include "aes.h"
}

namespace {
constexpr size_t kIvSize = 16;
constexpr size_t kLenSize = 2;
constexpr size_t kMaxPayload = 255;
constexpr size_t kMaxCipher = 224; // Multiple of 16, fits in payload with header+IV.
constexpr size_t kMaxPlaintext = kMaxCipher - 1;

static const uint8_t kAesKey[32] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
    0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
    0x0f, 0x1e, 0x2d, 0x3c, 0x4b, 0x5a, 0x69, 0x78,
    0x87, 0x96, 0xa5, 0xb4, 0xc3, 0xd2, 0xe1, 0xf0,
};

size_t pkcs7_unpad(uint8_t* buffer, size_t length) {
    if (length == 0 || (length % 16) != 0) {
        return 0;
    }
    uint8_t pad_len = buffer[length - 1];
    if (pad_len == 0 || pad_len > 16 || pad_len > length) {
        return 0;
    }
    for (size_t i = 0; i < pad_len; ++i) {
        if (buffer[length - 1 - i] != pad_len) {
            return 0;
        }
    }
    return length - pad_len;
}

void init_display(ILI9341_TFT& display) {
    display.SetupGPIO(20, 21, 17, 18, 19, 16);
    display.SetupScreenSize(240, 320);
    display.SetupSPI(25000000, spi0);
    display.ILI9341Initialize();

    display.fillScreen(display.C_BLACK);
    display.setTextColor(display.C_WHITE, display.C_BLACK);
    display.setTextWrap(true);
    display.setCursor(0, 0);
    display.println("Waiting for LoRa...");
}
} // namespace

int main() {
    RadioStream radio;
    RadioStream::Config config;
    config.frequency_hz = 915000000;
    config.lora_spreading_factor = 12;
    radio.init(config);

    ILI9341_TFT display;
    init_display(display);

    uint8_t rx_buffer[kMaxPayload] = {0};
    RadioStream::RxBuffer rx{rx_buffer, sizeof(rx_buffer), 0};

    while (true) {
        radio.poll();

        if (radio.available()) {
            radio >> rx;
            if (rx.length >= kLenSize + kIvSize) {
                size_t cipher_len = (static_cast<size_t>(rx_buffer[0]) << 8) | rx_buffer[1];
                size_t expected = kLenSize + kIvSize + cipher_len;
                if (cipher_len > 0 && cipher_len <= kMaxCipher && (cipher_len % 16) == 0 &&
                    rx.length >= expected) {
                    uint8_t iv[kIvSize];
                    memcpy(iv, &rx_buffer[kLenSize], kIvSize);

                    uint8_t plain[kMaxCipher];
                    memcpy(plain, &rx_buffer[kLenSize + kIvSize], cipher_len);

                    AES_ctx ctx;
                    AES_init_ctx_iv(&ctx, kAesKey, iv);
                    AES_CBC_decrypt_buffer(&ctx, plain, static_cast<uint32_t>(cipher_len));

                    size_t plain_len = pkcs7_unpad(plain, cipher_len);
                    if (plain_len > 0) {
                        size_t copy_len = (plain_len < kMaxPlaintext) ? plain_len : kMaxPlaintext;
                        char text[kMaxPlaintext + 1];
                        memcpy(text, plain, copy_len);
                        text[copy_len] = '\0';

                        display.fillScreen(display.C_BLACK);
                        display.setCursor(0, 0);
                        display.print(text);
                    }
                }
            }
            radio.start_rx();
        }

        sleep_ms(5);
        tight_loop_contents();
    }

    return 0;
}
