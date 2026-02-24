project(game1 C CXX ASM)

add_executable(game1 
        main.cpp
        xpt2046.c
)

add_executable(touch_test
        touch_test.cpp
        xpt2046.c
)

add_executable(buzzer_test
        buzzer_test.cpp
)

add_executable(touch_calibration
        touch_calibration.cpp
        xpt2046.c
)

pico_set_program_name(game1 "game1")
pico_set_program_version(game1 "0.1")

pico_enable_stdio_uart(game1 0)
pico_enable_stdio_usb(game1 1)

pico_enable_stdio_uart(touch_test 0)
pico_enable_stdio_usb(touch_test 1)

pico_enable_stdio_uart(buzzer_test 0)
pico_enable_stdio_usb(buzzer_test 1)

pico_enable_stdio_uart(touch_calibration 0)
pico_enable_stdio_usb(touch_calibration 1)

target_link_libraries(game1
        pico_stdlib
        hardware_spi
        hardware_pwm
        hardware_adc
        pico_lora_radio
        pico_game
        displaylib_16
)

target_link_libraries(touch_test
        pico_stdlib
        hardware_spi
        displaylib_16
        pico_game
)

target_link_libraries(buzzer_test
        pico_stdlib
        hardware_pwm
        pico_game
)

target_link_libraries(touch_calibration
        pico_stdlib
        hardware_spi
        displaylib_16
        pico_game
)

target_include_directories(game1 PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/../../game
        ${CMAKE_CURRENT_LIST_DIR}/../../displaylib_16bit_PICO/include)

target_include_directories(touch_test PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/../../game
        ${CMAKE_CURRENT_LIST_DIR}/../../displaylib_16bit_PICO/include)

target_include_directories(buzzer_test PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/../../game)

target_include_directories(touch_calibration PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/../../game
        ${CMAKE_CURRENT_LIST_DIR}/../../displaylib_16bit_PICO/include)

pico_add_extra_outputs(game1)
pico_add_extra_outputs(touch_test)
pico_add_extra_outputs(buzzer_test)
pico_add_extra_outputs(touch_calibration)