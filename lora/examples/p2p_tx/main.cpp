#include <cstdint>
#include <cstdio>
#include <cstring>

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

    const uint8_t payload[] = {'H', 'E', 'L', 'L', 'O'};
    RadioStream::TxBuffer tx{payload, sizeof(payload)};
    absolute_time_t next_tx = make_timeout_time_ms(1000);

    while (true)
    {
        radio.poll();

        if (!radio.tx_busy() && time_reached(next_tx))
        {
            radio << tx;
            if (radio.tx_busy())
            {
                next_tx = make_timeout_time_ms(1000);
            }
        }

        if (radio.last_tx_timeout())
        {
            printf("TX timeout\n");
        }

        sleep_ms(5);
    }
}
