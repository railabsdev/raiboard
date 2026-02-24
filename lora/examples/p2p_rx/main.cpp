#include <cstdint>
#include <cstdio>

#include "pico/stdlib.h"
#include "pico/radio_stream.hpp"

int main()
{
    stdio_init_all();
    sleep_ms(1500);

    RadioStream radio;
    RadioStream::Config config;
    config.frequency_hz = 915000000;

    radio.init(config);

    uint8_t buffer[255];
    RadioStream::RxBuffer rx{buffer, sizeof(buffer), 0};

    while (true)
    {
        radio.poll();

        if (radio.available())
        {
            radio >> rx;
            printf("RX %u bytes RSSI %d SNR %d: ", static_cast<unsigned>(rx.length), radio.last_rssi(), radio.last_snr());
            for (size_t i = 0; i < rx.length; ++i)
            {
                printf("%02x", buffer[i]);
            }
            printf("\n");
            radio.start_rx();
        }

        sleep_ms(5);
    }
}
