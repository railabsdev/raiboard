cmake_minimum_required(VERSION 3.12)

add_executable(p2p_display_sender
    sender.cpp
)

add_executable(p2p_display_receiver
    receiver.cpp
)

add_library(tiny_aes_p2p_display STATIC
    ../../lib/tiny-AES-c/aes.c
)

target_include_directories(tiny_aes_p2p_display PUBLIC
    ../../lib/tiny-AES-c
)

target_compile_definitions(tiny_aes_p2p_display PUBLIC AES256=1 CBC=1)

target_link_libraries(p2p_display_sender
    pico_lora_radio
    pico_stdlib
    tiny_aes_p2p_display
)

target_link_libraries(p2p_display_receiver
    pico_lora_radio
    pico_stdlib
    hardware_spi
    displaylib_16
    tiny_aes_p2p_display
)

target_include_directories(p2p_display_receiver PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../../displaylib_16bit_PICO/include
)

pico_enable_stdio_usb(p2p_display_sender 0)
pico_enable_stdio_uart(p2p_display_sender 0)

pico_enable_stdio_usb(p2p_display_receiver 0)
pico_enable_stdio_uart(p2p_display_receiver 0)

pico_add_extra_outputs(p2p_display_sender)
pico_add_extra_outputs(p2p_display_receiver)
