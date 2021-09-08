
#include "KeyboardInputReceiver.h"
#include "Features.h"
#include <iostream>
#include <string>
#include <set>
#include "Screen.h"
#include "Sender.h"

uint8_t DURATION = 1;
uint64_t FEATURES_TO_DISABLE = 0;

Screen *screen;
std::set<std::string> featuresToBeDisabled;

void printDurationMenu() {
    KeyboardInputReceiver keyboardInputReceiver = KeyboardInputReceiver(screen);
    int64_t startTime = time_us_64();
    uint8_t selected = 0;
    uint8_t duration = DURATION;
    uint8_t menuLength = 7;
    std::string menuList[7] = {"+10", "+5", "+1", "-10", "-5", "-1", "Minutes = " + std::to_string(duration)};
    screen->drawListMenu("Duration", menuList, menuLength, selected);
    screen->printImage();
    int keyIndex = keyboardInputReceiver.getButtonKeyIndex(startTime);
    while (keyIndex != 3) { //while left key is not pressed
        std::cout << keyIndex << std::endl;
        switch (keyIndex) {
            case 1: //Up key
                if (selected > 0) {
                    selected--;
                }
                break;
            case 4: //Down key
                if (selected < menuLength - 2) {
                    selected++;
                }
                break;
            case 2: { //Enter key 
                int minutes = atoi(menuList[selected].c_str());
                if (duration + minutes < 256 && duration + minutes > 0) {
                    duration+=minutes;
                }
                
                menuList[menuLength-1] = "Minutes = " + std::to_string(duration);
                break;
            }
        }

        screen->drawListMenu("Duration", menuList, menuLength, selected);
        screen->printImage();
        keyIndex = keyboardInputReceiver.getButtonKeyIndex(startTime);
    }

    DURATION = duration;
}


void printSubFunctionsMenu(std::string menuName, std::string menuList[], uint8_t menuLength) {
    KeyboardInputReceiver keyboardInputReceiver = KeyboardInputReceiver(screen);
    int64_t startTime = time_us_64();
    uint8_t selected = 0;
    bool checked[menuLength] = {false};
    for (uint8_t i = 0; i < menuLength; i++) {
        if (featuresToBeDisabled.find(menuList[i]) != featuresToBeDisabled.end()) {
            checked[i] = true;
        }
    }

    screen->drawCheckListMenu(menuName, menuList, menuLength, selected, checked);
    screen->printImage();
    int keyIndex = keyboardInputReceiver.getButtonKeyIndex(startTime);
    while (keyIndex != 3) { //while left key is not pressed
        switch (keyIndex) {
            case 1: //Up key
                if (selected > 0) {
                    selected--;
                }
                break;
            case 4: //Down key
                if (selected < menuLength - 1) {
                    selected++;
                }
                break;
            case 2: { //Enter key 
                if (checked[selected])
                    featuresToBeDisabled.erase(menuList[selected]);
                else
                    featuresToBeDisabled.insert(menuList[selected]);

                checked[selected] = !checked[selected];
                break;
            }
        }

        screen->drawCheckListMenu(menuName, menuList, menuLength, selected, checked);
        screen->printImage();
        keyIndex = keyboardInputReceiver.getButtonKeyIndex(startTime);
    }
}

void printFunctionsMenu() {
    KeyboardInputReceiver keyboardInputReceiver = KeyboardInputReceiver(screen);
    int64_t startTime = time_us_64();
    uint8_t menuLength = 7;
    uint8_t selected = 0;
    std::string features[menuLength] = {"Trig functions", "Bitwise operators", "Math functions", "Matrices", "Custom variables", "Complex numbers", "Graphing"};
    screen->printRemoteFunctionsMenu("Features", features, menuLength, selected); 
    int keyIndex = keyboardInputReceiver.getButtonKeyIndex(startTime);
    while (keyIndex != 3) { //while left key is not pressed
        switch (keyIndex) {
            case 1: //Up key
                if (selected > 0) {
                    selected--;
                }
                break;
            case 4: //Down key
                if (selected < menuLength - 1) {
                    selected++;
                }
                break;
            case 2: { //Enter key 
                // enter appropriate submenu when enter is pressed
                switch (selected) {
                    case 0: {
                        uint8_t menuLength = 24;
                        std::string menuList[menuLength] =  {
                            "sin",
                            "asin",
                            "asinh", 
                            "asinh",
                            "cos",
                            "acos", 
                            "cosh",
                            "acosh",
                            "tan",
                            "atan", 
                            "tanh",
                            "atanh",
                            "csc",
                            "acsc",
                            "csch",
                            "acsch",
                            "sec",
                            "asec", 
                            "sech",
                            "asech",
                            "cot",
                            "acot", 
                            "coth",
                            "acoth",
                        };
                        printSubFunctionsMenu("Trig Funct's", menuList, menuLength);
                        break;
                    }
                    case 1: {
                        uint8_t menuLength = 6;
                        std::string menuList[menuLength] =  {
                            "and",
                            "or",
                            "not",
                            "xor",
                            "lls",
                            "lrs",
                        };
                        printSubFunctionsMenu("Bitwise Ops", menuList, menuLength);
                        break;
                    }
                    case 2: {
                        uint8_t menuLength = 10;
                        std::string menuList[menuLength] =  {
                            "fnInt",
                            "sigma",
                            "prod",
                            "log",
                            "ln",
                            "nPr",
                            "nCr",
                            "gcd",
                            "lcm",
                            "rand",
                        };
                        printSubFunctionsMenu("Math Funct's", menuList, menuLength);
                        break;
                    }
                    case 3: {
                        uint8_t menuLength = 6;
                        std::string menuList[menuLength] =  {
                            "transpose",
                            "det",
                            "dot",
                            "cross",
                            "solveSOLE",
                            "[A]",
                        };
                        printSubFunctionsMenu("Matrices", menuList, menuLength);
                        break;
                    }
                    case 4: {
                        uint8_t menuLength = 1;
                        std::string menuList[menuLength] =  {
                            "Disable variable",
                        };
                        printSubFunctionsMenu("Variables", menuList, menuLength);
                        break;
                    }
                    case 5: {
                        uint8_t menuLength = 1;
                        std::string menuList[menuLength] =  {
                            "Disable complex",
                        };
                        printSubFunctionsMenu("Complex Nums", menuList, menuLength);
                        break;
                    }
                    case 6: {
                        uint8_t menuLength = 1;
                        std::string menuList[menuLength] =  {
                            "Disable graphing",
                        };
                        printSubFunctionsMenu("Graphing", menuList, menuLength);
                        break;
                    }
                }
                break;
            }
        }

        screen->printRemoteFunctionsMenu("Features", features, menuLength, selected);
        sleep_ms(500);
        keyIndex = keyboardInputReceiver.getButtonKeyIndex(startTime);
    }

    return;
}

static volatile bool test = false;

void gpioIRQ(uint gpio, uint32_t events) {
    printf("GPIO %d with event %u\n", gpio, events);
    test = true;
    //To check if the transmission was successful, it would be advisable to set a boolean flag here
    //Then wait to see if it changes in a time frame

    //As a result of this transmission, handleIRQ() MUST be eventually called on the appropriate object
}

int main() {
    stdio_init_all();

    Sender sender(0, 2, 0, 3, 1, 4);
    gpio_set_irq_enabled_with_callback(4, GPIO_IRQ_EDGE_RISE, true, &gpioIRQ);
    //Currently the GPIO parameter is ignored, and this callback will be called for any enabled GPIO IRQ on any pin. (lol)

    //Initialize the screen
    screen = new Screen();

    //Draw Strings on image
    screen->drawString(1, 24, "BOUNDLESS", &Font20, BLACK, WHITE);
    screen->drawString(1, 48, "Adminstrative Remote", &Font12, BLACK, WHITE);
    screen->printImage();
    DEV_Delay_ms(3000);
    //Display blank white screen
    screen->clearImage();
    screen->printImage();
   
    KeyboardInputReceiver keyboardInputReceiver = KeyboardInputReceiver(screen);

    std::string menuName = "Choose";
    std::string menuList[4] = {"Features to disable", "Disable duration", "", "Send disable request"};
    int8_t selected = 0;
    int64_t startTime = time_us_64();
    screen->printInitialRemoteMenu(menuName, menuList, selected);
    
    int keyIndex = keyboardInputReceiver.getButtonKeyIndex(startTime);
     // While the keyIndex != power off
    while (keyIndex != 0){
        std::cout << keyIndex << std::endl;
        switch (keyIndex) {
            case 1: //Up key
                if (selected > 0) {
                    if (selected == 3) {
                        selected--;
                    }
                    selected--;
                }
                break;
            case 4: //Down key
                if (selected < 3) {
                    if (selected == 1) {
                        selected++;
                    }

                    selected++;
                }
                break;
            case 5: //Right key
            case 3: {//Left key
                break;
            }
            case 2: {//Enter key 
                // enter appropriate submenu when enter is pressed
                if (selected == 0) {
                    printFunctionsMenu();
                }
                else if (selected == 1) {
                    printDurationMenu();
                }
                else if (selected == 3) {
                    if (!featuresToBeDisabled.empty()) {
                        for (std::string feature : featuresToBeDisabled) {
                            if (featuresToBitMap.find(feature) != featuresToBitMap.end())
                                FEATURES_TO_DISABLE |= featuresToBitMap.at(feature);
                        }
                    }

                    sender.sendMessage(DURATION, FEATURES_TO_DISABLE);
                    screen->clearImage();
                    screen->drawString(1, 48, "SENDING...", &Font16, BLACK, WHITE);
                    screen->printImage();
                    while(!test){}
                    screen->clearImage();
                    screen->drawString(1, 32, "SENT!", &Font12, BLACK, WHITE);
                    screen->drawString(1, 48, "Your remote code is: ", &Font12, BLACK, WHITE);
                    screen->drawString(1, 61, std::string(sender.code), &Font12, BLACK, WHITE);
                    screen->drawString(1, 80, "Press any key to", &Font12, BLACK, WHITE);
                    screen->drawString(1, 93, "continue", &Font12, BLACK, WHITE);
                    screen->printImage();
                    keyboardInputReceiver.getButtonKeyIndex(startTime);
                    screen->clearImage();
                    screen->printImage();
                    test = false;
                    sender.handleIRQ();
                }

                break;
            }
        }
        
        screen->printInitialRemoteMenu(menuName, menuList, selected);
        sleep_ms(500);
        keyIndex = keyboardInputReceiver.getButtonKeyIndex(startTime);
    }

    screen->cleanup();
    delete screen;
    return 0;
}