cmake_minimum_required(VERSION 3.12)

add_executable(pico_lora_p2p_rx
    main.cpp
)

target_link_libraries(pico_lora_p2p_rx pico_lora_radio)

pico_enable_stdio_usb(pico_lora_p2p_rx 1)
pico_enable_stdio_uart(pico_lora_p2p_rx 0)

pico_add_extra_outputs(pico_lora_p2p_rx)
