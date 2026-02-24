#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"

#include "board.h"
#include "delay.h"
#include "radio.h"
#include "rtc-board.h"
#include "sx126x-board.h"
#include "pico/board-config.h"

#if defined( REGION_AS923 )
#define RF_FREQUENCY                                923000000
#elif defined( REGION_AU915 )
#define RF_FREQUENCY                                915000000
#elif defined( REGION_CN470 )
#define RF_FREQUENCY                                470000000
#elif defined( REGION_CN779 )
#define RF_FREQUENCY                                779000000
#elif defined( REGION_EU433 )
#define RF_FREQUENCY                                433000000
#elif defined( REGION_EU868 )
#define RF_FREQUENCY                                868000000
#elif defined( REGION_KR920 )
#define RF_FREQUENCY                                920000000
#elif defined( REGION_IN865 )
#define RF_FREQUENCY                                865000000
#elif defined( REGION_US915 )
#define RF_FREQUENCY                                915000000
#elif defined( REGION_RU864 )
#define RF_FREQUENCY                                864000000
#else
#error "Please define a frequency band in the compiler options."
#endif

#define TX_OUTPUT_POWER                             14

#if defined( USE_MODEM_LORA )
#define LORA_BANDWIDTH                              0
#define LORA_SPREADING_FACTOR                       7
#define LORA_CODINGRATE                             1
#define LORA_PREAMBLE_LENGTH                        8
#define LORA_SYMBOL_TIMEOUT                         5
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false
#else
#error "Please define a modem in the compiler options."
#endif

static RadioEvents_t RadioEvents;
static volatile bool tx_done = false;
static volatile bool tx_timeout = false;

static void radio_hw_init(void)
{
    Gpio_t gpio_busy = {0};
    Gpio_t gpio_dio1 = {0};
    Gpio_t gpio_reset = {0};
    Gpio_t gpio_nss = {0};
    Spi_t spi = {0};

    gpio_busy.pin = RADIO_BUSY;
    gpio_dio1.pin = RADIO_DIO_1;
    gpio_reset.pin = RADIO_RESET;
    gpio_nss.pin = RADIO_NSS;

    spi.SpiId = SPI_1;
    spi.Nss = gpio_nss;

    SX126x.BUSY = gpio_busy;
    SX126x.DIO1 = gpio_dio1;
    SX126x.Reset = gpio_reset;
    SX126x.Spi = spi;

    SpiInit(&SX126x.Spi, SPI_1, RADIO_MOSI, RADIO_MISO, RADIO_SCLK, NC);
    SX126x.Spi.Nss.pin = RADIO_NSS;

    SX126xIoInit();
}

static void OnTxDone(void)
{
    Radio.Sleep();
    tx_done = true;
}

static void OnTxTimeout(void)
{
    Radio.Sleep();
    tx_timeout = true;
}

int main(void)
{
    stdio_init_all();
    sleep_ms(1500);

    RtcInit();
    BoardInitMcu();
    BoardInitPeriph();

    radio_hw_init();

    RadioEvents.TxDone = OnTxDone;
    RadioEvents.TxTimeout = OnTxTimeout;

    Radio.Init(&RadioEvents);
    Radio.SetChannel(RF_FREQUENCY);

    Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                      LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                      LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                      true, 0, 0, LORA_IQ_INVERSION_ON, 3000);

    const char payload[] = "HELLO";

    while (1)
    {
        tx_done = false;
        tx_timeout = false;

        Radio.Send((uint8_t *)payload, (uint8_t)strlen(payload));

        while (!tx_done && !tx_timeout)
        {
            if (Radio.IrqProcess != NULL)
            {
                Radio.IrqProcess();
            }
        }

        if (tx_timeout)
        {
            printf("TX timeout\n");
        }
        else
        {
            printf("TX done\n");
        }

        sleep_ms(1000);
    }
}
