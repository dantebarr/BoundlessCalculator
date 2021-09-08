if (TARGET tinyusb_device)
    add_executable(main
            main.cpp
            )

    # Pull in our pico_stdlib which aggregates commonly used features
    target_link_libraries(main pico_stdlib hardware_spi hardware_gpio)

    # enable usb output, disable uart output
    pico_enable_stdio_usb(main 1)
    pico_enable_stdio_uart(main 0)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(main)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building main because TinyUSB submodule is not initialized in the SDK")
endif()
