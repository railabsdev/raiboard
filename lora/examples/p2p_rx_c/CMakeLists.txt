cmake_minimum_required(VERSION 3.12)

add_executable(pico_lora_p2p_rx_c
    main.c
)

target_link_libraries(pico_lora_p2p_rx_c pico_lora_radio)

target_compile_definitions(pico_lora_p2p_rx_c PRIVATE
    USE_MODEM_LORA
    REGION_US915
)

pico_enable_stdio_usb(pico_lora_p2p_rx_c 1)
pico_enable_stdio_uart(pico_lora_p2p_rx_c 0)

pico_add_extra_outputs(pico_lora_p2p_rx_c)
