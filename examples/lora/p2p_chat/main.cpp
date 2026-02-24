#include <cstdint>
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "pico/stdio_usb.h"
#include "pico/radio_stream.hpp"
extern "C" {
#include "aes.h"
}

namespace {
constexpr size_t kChunkSize = 8;
constexpr size_t kMaxPayload = 255;
constexpr size_t kIvSize = 16;
constexpr size_t kLenSize = 2;
constexpr size_t kMaxCipher = 224; // Multiple of 16, fits in payload with header+IV.
constexpr size_t kMaxPlaintext = kMaxCipher - 1; // Leave room for PKCS7 padding.
constexpr uint8_t kInputTerminator = '.';
constexpr uint8_t kWireTerminator = '\n';

static const uint8_t kAesKey[32] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
    0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
    0x0f, 0x1e, 0x2d, 0x3c, 0x4b, 0x5a, 0x69, 0x78,
    0x87, 0x96, 0xa5, 0xb4, 0xc3, 0xd2, 0xe1, 0xf0,
};

size_t pkcs7_pad(uint8_t* buffer, size_t length, size_t max_length) {
    size_t pad_len = 16 - (length % 16);
    if (length + pad_len > max_length) {
        return 0;
    }
    for (size_t i = 0; i < pad_len; ++i) {
        buffer[length + i] = static_cast<uint8_t>(pad_len);
    }
    return length + pad_len;
}

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

void build_iv(uint8_t* iv, uint64_t counter) {
    memset(iv, 0, kIvSize);
    for (size_t i = 0; i < 8; ++i) {
        iv[kIvSize - 1 - i] = static_cast<uint8_t>(counter >> (i * 8));
    }
}
} // namespace

int main() {
    stdio_usb_init();
    while (!stdio_usb_connected()) {
        sleep_ms(10);
    }

    RadioStream radio;
    RadioStream::Config config;
    config.frequency_hz = 915000000;
    config.lora_spreading_factor = 12;
    radio.init(config);

    uint8_t tx_plain[kMaxPlaintext] = {0};
    size_t tx_plain_len = 0;
    bool tx_pending = false;
    size_t tx_offset = 0;
    size_t tx_total_padded = 0;
    uint64_t tx_iv_counter = 1;

    uint8_t tx_chunk[kChunkSize] = {0};
    uint8_t tx_frame[kMaxPayload] = {0};
    size_t tx_frame_len = 0;

    uint8_t rx_buffer[kMaxPayload];
    RadioStream::RxBuffer rx{rx_buffer, sizeof(rx_buffer), 0};
    uint8_t rx_frame[kMaxPayload] = {0};
    size_t rx_len = 0;
    size_t rx_expected = 0;

    bool rx_active = true;
    radio.start_rx();

    while (true) {
        radio.poll();

        if (radio.available()) {
            radio >> rx;
            for (size_t i = 0; i < rx.length; ++i) {
                uint8_t byte = rx_buffer[i];
                if (rx_len < kMaxPayload) {
                    rx_frame[rx_len++] = byte;
                }
                if (rx_expected == 0 && rx_len >= kLenSize) {
                    size_t cipher_len = (static_cast<size_t>(rx_frame[0]) << 8) | rx_frame[1];
                    if (cipher_len == 0 || cipher_len > kMaxCipher || (cipher_len % 16) != 0) {
                        rx_len = 0;
                        rx_expected = 0;
                        continue;
                    }
                    rx_expected = kLenSize + kIvSize + cipher_len;
                }
                if (rx_expected > 0 && rx_len >= rx_expected) {
                    size_t cipher_len = rx_expected - kLenSize - kIvSize;
                    uint8_t iv[kIvSize];
                    memcpy(iv, &rx_frame[kLenSize], kIvSize);
                    uint8_t plain[kMaxCipher];
                    memcpy(plain, &rx_frame[kLenSize + kIvSize], cipher_len);

                    AES_ctx ctx;
                    AES_init_ctx_iv(&ctx, kAesKey, iv);
                    AES_CBC_decrypt_buffer(&ctx, plain, static_cast<uint32_t>(cipher_len));

                    size_t plain_len = pkcs7_unpad(plain, cipher_len);
                    if (plain_len > 0) {
                        for (size_t j = 0; j < plain_len; ++j) {
                            putchar_raw(static_cast<char>(plain[j]));
                        }
                    }
                    rx_len = 0;
                    rx_expected = 0;
                }
            }
            rx_active = false;
        }

        if (!tx_pending && tx_plain_len < kMaxPlaintext) {
            int ch = getchar_timeout_us(0);
            if (ch != PICO_ERROR_TIMEOUT) {
                uint8_t byte = static_cast<uint8_t>(ch);
                if (byte == kInputTerminator) {
                    putchar_raw('\n');
                    byte = kWireTerminator;
                } else {
                    putchar_raw(static_cast<char>(byte));
                }

                tx_plain[tx_plain_len++] = byte;
                if (byte == kWireTerminator) {
                    uint8_t iv[kIvSize];
                    build_iv(iv, tx_iv_counter++);

                    uint8_t cipher[kMaxCipher];
                    memcpy(cipher, tx_plain, tx_plain_len);
                    size_t padded_len = pkcs7_pad(cipher, tx_plain_len, sizeof(cipher));
                    if (padded_len == 0) {
                        tx_plain_len = 0;
                        continue;
                    }

                    AES_ctx ctx;
                    AES_init_ctx_iv(&ctx, kAesKey, iv);
                    AES_CBC_encrypt_buffer(&ctx, cipher, static_cast<uint32_t>(padded_len));

                    tx_frame[0] = static_cast<uint8_t>((padded_len >> 8) & 0xFF);
                    tx_frame[1] = static_cast<uint8_t>(padded_len & 0xFF);
                    memcpy(&tx_frame[kLenSize], iv, kIvSize);
                    memcpy(&tx_frame[kLenSize + kIvSize], cipher, padded_len);
                    tx_frame_len = kLenSize + kIvSize + padded_len;

                    size_t padded = (tx_frame_len + kChunkSize - 1) / kChunkSize * kChunkSize;
                    tx_total_padded = padded;
                    tx_offset = 0;
                    tx_pending = true;
                    tx_plain_len = 0;
                }
            }
        }

        if (tx_pending && !radio.tx_busy()) {
            for (size_t i = 0; i < kChunkSize; ++i) {
                size_t idx = tx_offset + i;
                tx_chunk[i] = (idx < tx_frame_len) ? tx_frame[idx] : 0;
            }
            radio.send(tx_chunk, kChunkSize);
            tx_offset += kChunkSize;
            rx_active = false;

            if (tx_offset >= tx_total_padded) {
                tx_pending = false;
                tx_frame_len = 0;
            }
        }

        if (!radio.tx_busy() && !radio.available() && !rx_active) {
            radio.start_rx();
            rx_active = true;
        }

        tight_loop_contents();
    }

    return 0;
}