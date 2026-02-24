#include <cstdint>
#include <cstdio>
#include <cstring>

#include "pico/stdlib.h"
#include "pico/radio_stream.hpp"

extern "C" {
#include "aes.h"
}

namespace {
constexpr size_t kIvSize = 16;
constexpr size_t kLenSize = 2;
constexpr size_t kMaxPayload = 255;
constexpr size_t kMaxCipher = 224; // Multiple of 16, fits in payload with header+IV.
constexpr size_t kMaxPlaintext = kMaxCipher - 1;
constexpr uint32_t kSendIntervalMs = 1000;

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

void build_iv(uint8_t* iv, uint64_t counter) {
    memset(iv, 0, kIvSize);
    for (size_t i = 0; i < 8; ++i) {
        iv[kIvSize - 1 - i] = static_cast<uint8_t>(counter >> (i * 8));
    }
}
} // namespace

int main() {
    RadioStream radio;
    RadioStream::Config config;
    config.frequency_hz = 915000000;
    config.lora_spreading_factor = 12;
    radio.init(config);

    uint64_t message_counter = 1;
    uint64_t iv_counter = 1;
    uint32_t next_send_ms = to_ms_since_boot(get_absolute_time()) + kSendIntervalMs;

    uint8_t tx_frame[kMaxPayload] = {0};

    while (true) {
        radio.poll();

        uint32_t now_ms = to_ms_since_boot(get_absolute_time());
        if (!radio.tx_busy() && static_cast<int32_t>(now_ms - next_send_ms) >= 0) {
            char message[kMaxPlaintext + 1] = {0};
            int msg_len = snprintf(message, sizeof(message), "message%llu\n",
                                   static_cast<unsigned long long>(message_counter++));
            if (msg_len > 0 && static_cast<size_t>(msg_len) < sizeof(message)) {
                uint8_t iv[kIvSize];
                build_iv(iv, iv_counter++);

                uint8_t cipher[kMaxCipher];
                memcpy(cipher, message, static_cast<size_t>(msg_len));
                size_t padded_len = pkcs7_pad(cipher, static_cast<size_t>(msg_len), sizeof(cipher));
                if (padded_len > 0) {
                    AES_ctx ctx;
                    AES_init_ctx_iv(&ctx, kAesKey, iv);
                    AES_CBC_encrypt_buffer(&ctx, cipher, static_cast<uint32_t>(padded_len));

                    tx_frame[0] = static_cast<uint8_t>((padded_len >> 8) & 0xFF);
                    tx_frame[1] = static_cast<uint8_t>(padded_len & 0xFF);
                    memcpy(&tx_frame[kLenSize], iv, kIvSize);
                    memcpy(&tx_frame[kLenSize + kIvSize], cipher, padded_len);
                    size_t frame_len = kLenSize + kIvSize + padded_len;

                    radio.send(tx_frame, frame_len);
                }
            }
            next_send_ms = now_ms + kSendIntervalMs;
        }

        sleep_ms(5);
        tight_loop_contents();
    }

    return 0;
}
