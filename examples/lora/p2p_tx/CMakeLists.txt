cmake_minimum_required(VERSION 3.12)

add_executable(pico_lora_p2p_tx
    main.cpp
)

target_link_libraries(pico_lora_p2p_tx pico_lora_radio)

pico_enable_stdio_usb(pico_lora_p2p_tx 1)
pico_enable_stdio_uart(pico_lora_p2p_tx 0)

pico_add_extra_outputs(pico_lora_p2p_tx)
