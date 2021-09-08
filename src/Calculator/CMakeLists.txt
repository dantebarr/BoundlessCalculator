if (TARGET tinyusb_device)
    add_executable(calculator
            calculator.cpp
	    CompiledExpression.cpp
	    DisabledFeatureException.cpp
	    ExpressionTreeNode.cpp
	    KeyboardInputReceiver.cpp
	    Operation.cpp
	    ParsedExpression.cpp
	    Parser.cpp
        DEV_Config.c
        LCD_1in8.c
        GUI_Paint.c
        font8.c
        font12.c
        font16.c
        font20.c
        font24.c
        Screen.cpp
        Matrix.cpp
            )

    # Pull in our pico_stdlib which aggregates commonly used features
    target_link_libraries(calculator pico_stdlib hardware_spi hardware_pwm hardware_gpio hardware_i2c)

    # enable usb output, disable uart output
    pico_enable_stdio_usb(calculator 1)
    pico_enable_stdio_uart(calculator 0)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(calculator)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building main because TinyUSB submodule is not initialized in the SDK")
endif()
