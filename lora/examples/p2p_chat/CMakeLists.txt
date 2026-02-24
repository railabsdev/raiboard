cmake_minimum_required(VERSION 3.12)

add_executable(p2p_chat
    main.cpp
)

add_library(tiny_aes STATIC
    ../../lib/tiny-AES-c/aes.c
)

target_include_directories(tiny_aes PUBLIC
    ../../lib/tiny-AES-c
)

target_compile_definitions(tiny_aes PUBLIC AES256=1 CBC=1)

target_link_libraries(p2p_chat
    pico_lora_radio
    pico_stdlib
    pico_stdio_usb
    pico_stdio
    tiny_aes
)

pico_enable_stdio_usb(p2p_chat 1)
pico_enable_stdio_uart(p2p_chat 0)

pico_add_extra_outputs(p2p_chat)
