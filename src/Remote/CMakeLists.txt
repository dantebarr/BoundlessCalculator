if (TARGET tinyusb_device)
    add_executable(remote
        remote.cpp
	    KeyboardInputReceiver.cpp
        DEV_Config.c
        LCD_1in8.c
        GUI_Paint.c
        font8.c
        font12.c
        font16.c
        font20.c
        font24.c
        Screen.cpp
            )

    # Pull in our pico_stdlib which aggregates commonly used features
    target_link_libraries(remote pico_stdlib hardware_spi hardware_pwm hardware_gpio hardware_i2c)


    # enable usb output, disable uart output
    pico_enable_stdio_usb(remote 1)
    pico_enable_stdio_uart(remote 0)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(remote)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building main because TinyUSB submodule is not initialized in the SDK")
endif()
