#include "Lepton.h"
#include "DisabledFeatureException.h"
#ifndef NO_BUTTONS
#include "KeyboardInputReceiver.h"
#endif
#include "DisplayOutputType.h"
#include "DisplayAngleUnit.h"
#include "Features.h"
#include "Receiver.h"
#include "Matrix.h"
#include <iostream>
#include <string>
#include <bitset>
#include <complex>
//Screen Stuff
#include "Screen.h"
#include "LED.h"
#include "RTC.h"
#include "Graph.h"
//#include "hardware/watchdog.h"

Lepton::Parser parser = Lepton::Parser();

//TODO: this might not be safe to modify from an IRQ
void disableFeatures(u64 features) {
    for (int i=0; i<64; i++) {
        unsigned long long feature = features & (1<<i);
        if (feature != 0) {
            auto it = bitToFeaturesMap.find(feature);
            if (it != bitToFeaturesMap.end())
                parser.addFeatureToDisable(it->second);
            else {
                std::bitset<64> x(feature);
                std::cout << "feature not found " << x << std::endl; 
            }
        }
    }
}

//TODO: Finalize pins
#define LED_PIN 8
#define RTC_SDA 10
#define RTC_SCL 11
#define TRANS_SCK 2
#define TRANS_MISO 0
#define TRANS_MOSI 3
#define TRANS_NSS 1
#define TRANS_DIO0 4

//Initialize periphreals
static Receiver receiver(0, TRANS_SCK, TRANS_MISO, TRANS_MOSI, TRANS_NSS, TRANS_DIO0);
static LED led(LED_PIN);
static RTC rtc(1, RTC_SDA, RTC_SCL);

int64_t exitTest(alarm_id_t id, void *data){
    (void) id;
    (void) data;
    #ifdef DEBUG
    std:cout << "Exiting test mode" << std::endl;
    #endif

    parser.clearFeaturesToDisable(); //TODO: This might not be safe to call from an IRQ
    led.exitTest();

    return 0;
}

//TODO: Monitor if these can be safely modified in the IRQ
static absolute_time_t enteredTime;
static u8 enteredDuration;
static Screen* screenPtr;//Global versions of these allow us to access what we cannot reach
static KeyboardInputReceiver* keyboardPtr;
void enterTestMode(u8 seed, u8 duration, u64 features, char* code, bool shouldStore){

    if(code){//If there is a code, entry is optional
        screenPtr->clearImage();//TODO: save the previous somewhere
        screenPtr->drawWrappingString(1, 1, "Would you like to enter test mode for " + std::to_string((int) duration) + " minutes? Press enter to accept or anything else to decline.", &Font12, BLACK, WHITE);
        screenPtr->printImage();
        int64_t startTime = time_us_64();
        int seenVal = keyboardPtr->getButtonKeyIndex(startTime);
        if(seenVal != 47)
            return;
        
        screenPtr->clearImage();
        screenPtr->drawWrappingString(1, 1, "Your special test code is: " + std::string(code) + " Press any key to continue.", &Font12, BLACK, WHITE);
        screenPtr->printImage();
        screenPtr->clearImage();
        keyboardPtr->getButtonKeyIndex(startTime);
    }

    enteredDuration = duration;
    enteredTime = get_absolute_time();

    receiver.enterSleep();//The burden is on us to do this to ensure sleep is consistent from startup or from RX
    if(shouldStore)
        rtc.storeTest(seed, duration, features);
    disableFeatures(features);
    led.enterTest(seed);

    add_alarm_in_ms(duration*1000*60, exitTest, NULL, true);
}

void receivedMsgIRQ(uint gpio, uint32_t events){
    std::cout << "Received an irq on gpio " << gpio << std::endl;

    gpio_set_function(TRANS_MISO, GPIO_FUNC_SPI);

    bool retVal = receiver.handleIRQ();
    gpio_set_function(TRANS_MISO, GPIO_FUNC_SIO);
    gpio_set_dir(TRANS_MISO, GPIO_OUT);
    gpio_put(TRANS_MISO, 0);
    if(retVal){
        enterTestMode(receiver.seed, receiver.duration, receiver.features, receiver.code, true);
    }
    else
        std::cout << "Invalid message received" << std::endl;
}

void graphLimitsMenu(Graph* graph, KeyboardInputReceiver* keyboardInputReceiver, Screen *screen){
    int8_t selected = 0;
    int64_t startTime = time_us_64();
    std::string limits[4];
    limits[0] = std::to_string(graph->xLeft);
    limits[1] = std::to_string(graph->xRight);
    limits[2] = std::to_string(graph->yTop);
    limits[3] = std::to_string(graph->yBottom);

    screen->printLimitMenu(limits, selected);

    int keyIndex = keyboardInputReceiver->getButtonKeyIndex(startTime);
    while (keyIndex != 47){ //Enter button exit and save changes
        // Up key is pressed
        if (keyIndex == 2 && selected > 0)
            selected--;
        // Down key is pressed
        else if (keyIndex == 9 && selected < 3)
            selected++;
        else if(keyIndex == 41 && limits[selected].length() != 0)//Delete
            limits[selected].pop_back();
        else if ((keyIndex >= 30 && keyIndex <= 40) || keyIndex == 26) //Number or decimal, negative sign
            limits[selected] += keyboardInputReceiver->getkeyIndexValue(keyIndex);
        else if (keyIndex == 29) //on button pressed discard changes
            return;

        screen->printLimitMenu(limits, selected);

        keyIndex = keyboardInputReceiver->getButtonKeyIndex(startTime);
    }

    //Check parameters, only update if valid
    double first, second;
    bool worked = false;
    try{//First do left/right
        first = std::stod(limits[0], NULL);
        second = std::stod(limits[1], NULL);
        if(first < second)
            worked = true;
    } catch(...){}//Silently discard errors
    if(worked){
        graph->xLeft = first;
        graph->xRight = second;
        graph->dirty = true;
        worked = false;
    }

    try{//Now try up/down
        first = std::stod(limits[2], NULL);
        second = std::stod(limits[3], NULL);
        if(first > second)
            worked = true;
    } catch(...){}//Silently discard errors
    if(worked){
        graph->yTop = first;
        graph->yBottom = second;
        graph->dirty = true;
    }
}

GraphFunctions graphFnMenu(unsigned int* fn1, unsigned int* fn2, KeyboardInputReceiver* keyboardInputReceiver, Screen* screen){
    uint8_t leftSelected = 0;
    uint8_t rightSelected = 0;
    uint8_t localFn1 = *fn1;
    uint8_t localFn2 = *fn2;
    int64_t startTime = time_us_64();
    

    screen->printGraphFunctionMenu(leftSelected, rightSelected, 0, 0);

    int keyIndex = keyboardInputReceiver->getButtonKeyIndex(startTime);
    while (true){
        if(rightSelected){
            if(keyIndex == 29)//Go back
                rightSelected = 0;
            else if(leftSelected == 3 && (keyIndex == 2 || keyIndex == 9))//Intersect needs 2
                rightSelected = rightSelected == 1 ? 2 : 1;
            else if(keyIndex == 47){//Enter, return
                *fn1 = localFn1;
                *fn2 = localFn2;
                if(leftSelected == 0)
                    return MAX;
                else if(leftSelected == 1)
                    return MIN;
                else if(leftSelected == 2)
                    return ZERO;
                else if(leftSelected == 3)
                    return INTERSECT;
                else
                    return INTEGRAL;
            }
            else if(keyIndex >= 35 && keyIndex <= 38){//Must be 1-4
                uint8_t* which = rightSelected == 1 ? &localFn1 : &localFn2;
                if(keyIndex == 35)
                    *which = 3;
                else if(keyIndex == 36)
                    *which = 2;
                else if(keyIndex == 37)
                    *which = 1;
                else
                    *which = 0;
            }
        }
        else{
            if(keyIndex == 47)//Enter, switch to right side
                rightSelected = 1;
            else if(keyIndex == 2 && leftSelected > 0)
                leftSelected--;
            else if(keyIndex == 9 && leftSelected < 4)
                leftSelected++;
            else if(keyIndex == 29)//Exit and do nothing
                return NONE;
        }
        
        screen->printGraphFunctionMenu(leftSelected, rightSelected, localFn1, localFn2);
        keyIndex = keyboardInputReceiver->getButtonKeyIndex(startTime);
    }
}

void graphScreen(std::string strings[], Graph* graph, KeyboardInputReceiver* keyboardInputReceiver, Screen* screen){
    screen->clearImage();
    if(parser.checkFeatureDisabled("Disable graphing"))
        throw DisabledFeatureException("Graphs");
    
    screen->clearImage();
    graph->drawGraphs(strings);
    screen->printImage();

    int64_t startTime = time_us_64();
    int keyIndex = keyboardInputReceiver->getButtonKeyIndex(startTime);
    while (keyIndex != 29){//On button exit
        if(keyIndex == 3 || keyIndex == 9 || keyIndex == 2 || keyIndex == 4){
            Direction dir;
            if(keyIndex == 3)
                dir = LEFT;
            else if(keyIndex == 9)
                dir = DOWN;
            else if(keyIndex == 2)
                dir = UP;
            else
                dir = RIGHT;
            screen->clearImage();
            graph->handleMovement(dir);
            screen->printImage();
        }
        else if(keyIndex == 47){
            screen->clearImage();
            graph->handleEnter();
            screen->printImage();
        }
        else if(keyIndex == 8){//Graph ctrl
            graphLimitsMenu(graph, keyboardInputReceiver, screen);
            screen->clearImage();
            graph->update();
            screen->printImage();
        }
        else if(keyIndex == 7){//Graph fn
            unsigned int fn1 = 0;
            unsigned int fn2 = 1;
            GraphFunctions whichFn = graphFnMenu(&fn1, &fn2, keyboardInputReceiver, screen);
            screen->clearImage();

            if(parser.checkFeatureDisabled("fnInt"))
                throw DisabledFeatureException("fnInt", "function");

            if(whichFn != NONE)
                graph->beginFunction(whichFn, fn1, fn2);
            screen->printImage();
        }
        keyIndex = keyboardInputReceiver->getButtonKeyIndex(startTime);
    }
}

void outputTypeMenu(KeyboardInputReceiver keyboardInputReceiver, Screen* screen, outputTypeEnum* outputType){
    std::string menuOptions1[4] = {"Decimal", "Binary", "Octal", "Hexadecimal"};
    int numOptions1 = 4;
    int selected = 0;
    int64_t startTime = time_us_64();
    
    int keyIndex = 0;
    // While the key pressed is not the ON button to exit the menu
    while (keyIndex != 29){
        screen->drawListMenu("OUTPUT TYPE", menuOptions1, numOptions1, selected);
        screen->printImage();
        
        keyIndex = keyboardInputReceiver.getButtonKeyIndex(startTime);

        // Up key is pressed
        if (keyIndex == 2){
            selected = ((selected - 1) < 0) ? numOptions1-1 : (selected - 1);
        }
        // Down key is pressed
        else if (keyIndex == 9){
            selected = ((selected + 1) > (numOptions1 - 1) ? 0 : (selected + 1));
        }
        // Enter key is pressed, return index of matrix to select
        else if (keyIndex == 47){
            if (menuOptions1[selected] == "Decimal"){
                *outputType = DECIMAL;
            }
            else if (menuOptions1[selected] == "Binary"){
                *outputType = BINARY;
            }
            else if (menuOptions1[selected] == "Octal"){
                *outputType = OCTAL;
            }
            else{
                *outputType = HEXADECIMAL;
            }
            return;
        }
    }
    
    return;
}

void angleUnitsMenu(KeyboardInputReceiver keyboardInputReceiver, Screen* screen, angleUnitEnum* angleUnit){
    std::string menuOptions1[2] = {"Radians", "Degrees"};
    int numOptions1 = 2;
    int selected = 0;
    int64_t startTime = time_us_64();
    
    int keyIndex = 0;
    // While the key pressed is not the ON button to exit the menu
    while (keyIndex != 29){
        screen->drawListMenu("ANGLE UNITS", menuOptions1, numOptions1, selected);
        screen->printImage();
        
        keyIndex = keyboardInputReceiver.getButtonKeyIndex(startTime);

        // Up key is pressed
        if (keyIndex == 2){
            selected = ((selected - 1) < 0) ? numOptions1-1 : (selected - 1);
        }
        // Down key is pressed
        else if (keyIndex == 9){
            selected = ((selected + 1) > (numOptions1 - 1) ? 0 : (selected + 1));
        }
        // Enter key is pressed, return index of matrix to select
        else if (keyIndex == 47){
            if (menuOptions1[selected] == "Radians"){
                *angleUnit = RAD;
            }
            else{
                *angleUnit = DEG;
            }
            return;
        }
    }
    
    return;
}

void modeMenu(KeyboardInputReceiver keyboardInputReceiver, Screen* screen, outputTypeEnum* outputType, angleUnitEnum* angleUnit){
    std::string menuOptions1[2] = {"Angle Units", "Output Type"};
    int numOptions1 = 2;
    int selected = 0;
    int64_t startTime = time_us_64();
    
    int keyIndex = 0;
    // While the key pressed is not the ON button to exit the menu
    while (keyIndex != 29){
        screen->drawListMenu("MODE", menuOptions1, numOptions1, selected);
        screen->printImage();
        
        keyIndex = keyboardInputReceiver.getButtonKeyIndex(startTime);

        // Up key is pressed
        if (keyIndex == 2){
            selected = ((selected - 1) < 0) ? numOptions1-1 : (selected - 1);
        }
        // Down key is pressed
        else if (keyIndex == 9){
            selected = ((selected + 1) > (numOptions1 - 1) ? 0 : (selected + 1));
        }
        // Enter key is pressed, return index of matrix to select
        else if (keyIndex == 47){
            if (menuOptions1[selected] == "Angle Units"){
                angleUnitsMenu(keyboardInputReceiver, screen, angleUnit);
            }
            else{
                outputTypeMenu(keyboardInputReceiver, screen, outputType);
            }
        }
    }
    
    return;
}

std::string logicMenu(KeyboardInputReceiver keyboardInputReceiver, Screen* screen){
    std::string menuOptions1[6] = {"AND", "OR", "NOT", "XOR", "Logical Shift Left", "Logical Shift Right"};
    int numOptions1 = 6;
    int selected = 0;
    int64_t startTime = time_us_64();
    std::string retOp = "";
    
    int keyIndex = 0;
    // While the key pressed is not the ON button to exit the menu
    while (keyIndex != 29){
        screen->drawListMenu("Bitwise", menuOptions1, numOptions1, selected);
        screen->printImage();
        
        keyIndex = keyboardInputReceiver.getButtonKeyIndex(startTime);

        // Up key is pressed
        if (keyIndex == 2){
            selected = ((selected - 1) < 0) ? numOptions1-1 : (selected - 1);
        }
        // Down key is pressed
        else if (keyIndex == 9){
            selected = ((selected + 1) > (numOptions1 - 1) ? 0 : (selected + 1));
        }
        // Enter key is pressed, return index of matrix to select
        else if (keyIndex == 47){
            switch(selected){
                case 0:
                    retOp = retOp + "and" + "(";
                    break;
                case 1:
                    retOp = retOp + "or" + "(";
                    break;
                case 2:
                    retOp = retOp + "not" + "(";
                    break;
                case 3:
                    retOp = retOp + "xor" + "(";
                    break;
                case 4:
                    retOp = retOp + "lls" + "(";
                    break;
                case 5:
                    retOp = retOp + "lrs" + "(";
                    break;
            }
            return retOp;
        }
    }
    
    return "";
}

std::string trigMenu(KeyboardInputReceiver keyboardInputReceiver, Screen* screen){
    std::string menuOptions1[7] = {"Secant", "Cosecant", "Cotangent", "Cosh", "Sinh", "Tanh", "Acosh"};
    std::string menuOptions2[7] = {"Asinh", "Atanh", "Asecant", "Acosecant", "Acotangent", "Secanth", "Cosecanth"};
    std::string menuOptions3[4] = {"Cotangenth", "Asecanth", "Acosecanth", "Acotangenth"};
    int numOptions1 = 7;
    int numOptions2 = 7;
    int numOptions3 = 4;
    int selected = 0;
    int64_t startTime = time_us_64();
    std::string retOp = "";
    
    int keyIndex = 0;
    // While the key pressed is not the ON button to exit the menu
    while (keyIndex != 29){
        if (selected < 7){
            screen->drawListMenu("Trigonometry", menuOptions1, numOptions1, selected % 7);
        }
        else if (selected < 14){
            screen->drawListMenu("Trigonometry", menuOptions2, numOptions2, selected % 7);
        }
        else{
            screen->drawListMenu("Trigonometry", menuOptions3, numOptions3, selected % 7);
        }
        
        screen->printImage();
        
        keyIndex = keyboardInputReceiver.getButtonKeyIndex(startTime);

        // Up key is pressed
        if (keyIndex == 2){
            selected = ((selected - 1) < 0) ? numOptions1+numOptions2+numOptions3-1 : (selected - 1);
        }
        // Down key is pressed
        else if (keyIndex == 9){
            selected = ((selected + 1) > (numOptions1 + numOptions2 + numOptions3 - 1) ? 0 : (selected + 1));
        }
        // Enter key is pressed, return index of matrix to select
        else if (keyIndex == 47){
            switch(selected){
                case 0:
                    retOp = retOp + "sec" + "(";
                    break;
                case 1:
                    retOp = retOp + "csc" + "(";
                    break;
                case 2:
                    retOp = retOp + "cot" + "(";
                    break;
                case 3:
                    retOp = retOp + "cosh" + "(";
                    break;
                case 4:
                    retOp = retOp + "sinh" + "(";
                    break;
                case 5:
                    retOp = retOp + "tanh" + "(";
                    break;
                case 6:
                    retOp = retOp + "acosh" + "(";
                    break;
                case 7:
                    retOp = retOp + "asinh" + "(";
                    break;
                case 8:
                    retOp = retOp + "atanh" + "(";
                    break;
                case 9:
                    retOp = retOp + "asec" + "(";
                    break;
                case 10:
                    retOp = retOp + "acsc" + "(";
                    break;
                case 11:
                    retOp = retOp + "acot" + "(";
                    break;
                case 12:
                    retOp = retOp + "sech" + "(";
                    break;
                case 13:
                    retOp = retOp + "csch" + "(";
                    break;
                case 14:
                    retOp = retOp + "coth" + "(";
                    break;
                case 15:
                    retOp = retOp + "asech" + "(";
                    break;
                case 16:
                    retOp = retOp + "acsch" + "(";
                    break;
                case 17:
                    retOp = retOp + "acoth" + "(";
                    break;
            }
            return retOp;
        }
    }
    
    return "";
}

std::vector<std::string> functionVectors[4];
void functionMenu(std::string storedFunctions[], KeyboardInputReceiver keyboardInputReceiver, Screen* screen){
    int8_t selected = 0;
    std::string expressionComponent;
    int64_t startTime = time_us_64();    

    screen->printFunctionMenu(storedFunctions, selected);

    int keyIndex = keyboardInputReceiver.getButtonKeyIndex(startTime);
    while (keyIndex != 29 && keyIndex != 47){//On or enter button exit menu
        // Up key is pressed
        if (keyIndex == 2 && selected > 0){
            selected--;
        }
        // Down key is pressed
        else if (keyIndex == 9 && selected < 3){
            selected++;
        }
        else if (keyIndex == 6){//x/theta button
            functionVectors[selected].push_back("x");
            storedFunctions[selected] += "x";
        }
        //TODO if delete key is pressed remove last char from storedFunctions[selected]
        // Delete button pressed
        else if (keyIndex == 41 && functionVectors[selected].size() != 0) {
            std::string tmpString = "";
            functionVectors[selected].pop_back();
            for (std::string exp : functionVectors[selected]) {
                tmpString += exp;
            }
            storedFunctions[selected] = tmpString;
        }
        else{
            expressionComponent = keyboardInputReceiver.getkeyIndexValue(keyIndex);
            functionVectors[selected].push_back(expressionComponent);
            storedFunctions[selected] += expressionComponent;
        }
        
        screen->printFunctionMenu(storedFunctions, selected);

        keyIndex = keyboardInputReceiver.getButtonKeyIndex(startTime);
    }

}

void matrixMenuEdit(KeyboardInputReceiver keyboardInputReceiver, Screen* screen, int64_t startTime){
    std::string menuOptions[6] = {"[A]", "[B]", "[C]", "[D]", "[E]", "[F]"};
    std::string expression = "";
    int selected = 0;
    int inputCounter = 0;
    int rows = 0;
    int cols = 0;
    std::string matrixName;
    screen->drawListMenu("EDIT", menuOptions, 6, selected);
    screen->printImage();
    int keyIndex = keyboardInputReceiver.getButtonKeyIndex(startTime);
    // While the key pressed is not the enter button to select a matrix
    while (keyIndex != 47){
        // Up key is pressed
        if (keyIndex == 2){
            selected = ((selected - 1) < 0) ? 5 : (selected - 1);
        }
        // Down key is pressed
        else if (keyIndex == 9){
            selected = (selected + 1) % 6;
        }
        // ON key is pressed, exit to previous menu
        else if (keyIndex == 29){
            return;
        }
        screen->drawListMenu("EDIT", menuOptions, 6, selected);
        screen->printImage();

        keyIndex = keyboardInputReceiver.getButtonKeyIndex(startTime);
    }

    matrixName = menuOptions[selected];

    deleteMatrixValue(matrixName);

    screen->clearImage();
    
    while (inputCounter < 2){
        screen->drawMatrixEditMenu(matrixName, rows, cols, inputCounter, expression);
        screen->printImage();
        keyIndex = keyboardInputReceiver.getButtonKeyIndex(startTime);
        //Enter key pressed move to next value
        if (keyIndex == 47){
            // Read in matrix rows
            if(inputCounter == 0){
                rows = std::stoi(expression);
            }
            // Read in matrix columns
            else if(inputCounter == 1){
                cols = std::stoi(expression);
            }
            inputCounter++;
            expression = "";
        }
        else{
            expression += keyboardInputReceiver.getkeyIndexValue(keyIndex);
        }
    }

    Eigen::MatrixXd A(rows,cols);

    while (inputCounter < (2+rows*cols)){
        screen->drawMatrixEditMenu(matrixName, rows, cols, inputCounter, expression);
        screen->printImage();
        keyIndex = keyboardInputReceiver.getButtonKeyIndex(startTime);
        // Enter key is pressed, increment input counter
        if (keyIndex == 47){
            A((inputCounter-2)/cols,(inputCounter-2)%cols) = std::stof(expression);
            inputCounter++;
            expression = "";
        }
        else{
            expression += keyboardInputReceiver.getkeyIndexValue(keyIndex);
        }
    }

    storeMatrixValue(matrixName, A);

    return;
}

std::string matrixMenuMath(KeyboardInputReceiver keyboardInputReceiver, Screen* screen, int64_t startTime){
    std::string menuOptions[5] = {"Transpose", "Det", "Dot", "Cross", "Solve Ax=b"};
    int numOptions = 5;
    int selected = 0;
    std::string retOp = "";
    
    int keyIndex = 0;
    // While the key pressed is not the ON button to exit the menu
    while (keyIndex != 29){
        screen->drawListMenu("MATH", menuOptions, numOptions, selected);
        screen->printImage();
        
        keyIndex = keyboardInputReceiver.getButtonKeyIndex(startTime);

        // Up key is pressed
        if (keyIndex == 2){
            selected = ((selected - 1) < 0) ? numOptions-1 : (selected - 1);
        }
        // Down key is pressed
        else if (keyIndex == 9){
            selected = (selected + 1) % numOptions;
        }
        // Enter key is pressed, return index of matrix to select
        else if (keyIndex == 47){
            switch(selected){
                case 0:
                    retOp = retOp + "transpose" + "(";
                    break;
                case 1:
                    retOp = retOp + "det" + "(";
                    break;
                case 2:
                    retOp = retOp + "dot" + "(";
                    break;
                case 3:
                    retOp = retOp + "cross" + "(";
                    break;
                case 4:
                    retOp = retOp + "solveSOLE" + "(";
                    break;
            }
            return retOp;
        }
    }
    
    return "";
}

std::string matrixMenuNames(KeyboardInputReceiver keyboardInputReceiver, Screen* screen, int64_t startTime){
    std::string menuOptions[6] = {"[A]", "[B]", "[C]", "[D]", "[E]", "[F]"};
    int selected = 0;
    screen->drawListMenu("NAMES", menuOptions, 6, selected);
    screen->printImage();
    int keyIndex = keyboardInputReceiver.getButtonKeyIndex(startTime);
    // While the key pressed is not the ON button to exit the menu
    while (keyIndex != 29){
        // Up key is pressed
        if (keyIndex == 2){
            selected = ((selected - 1) < 0) ? 5 : (selected - 1);
        }
        // Down key is pressed
        else if (keyIndex == 9){
            selected = (selected + 1) % 6;
        }
        // Enter key is pressed, return index of matrix to select
        else if (keyIndex == 47){
            return menuOptions[selected];
        }
        screen->drawListMenu("NAMES", menuOptions, 6, selected);
        screen->printImage();

        keyIndex = keyboardInputReceiver.getButtonKeyIndex(startTime);
    }
    
    return "";
}

std::string matrixMenu(KeyboardInputReceiver keyboardInputReceiver, Screen* screen){
    std::string menuOptions[3] = {"Names", "Math", "Edit"};
    int selected = 0;
    int64_t startTime = time_us_64();
    std::string ret;
    screen->drawListMenu("MATRICES", menuOptions, 3, selected);
    screen->printImage();
    int keyIndex = keyboardInputReceiver.getButtonKeyIndex(startTime);
    // While the key pressed is not the ON button to exit the menu
    while (keyIndex != 29){
        // Up key is pressed
        if (keyIndex == 2){
            selected = ((selected - 1) < 0) ? 2 : (selected - 1);
        }
        // Down key is pressed
        else if (keyIndex == 9){
            selected = (selected + 1) % 3;
        }
        // Enter key is pressed, select that sub menu
        else if (keyIndex == 47){
            if (selected == 0){
                ret = matrixMenuNames(keyboardInputReceiver, screen, startTime);
                if (ret != ""){
                    screen->clearImage();
                    return ret;
                }
            }
            else if (selected == 1){
                ret = matrixMenuMath(keyboardInputReceiver, screen, startTime);
                if (ret != ""){
                    screen->clearImage();
                    return ret;
                }
            }
            else{
                matrixMenuEdit(keyboardInputReceiver, screen, startTime);
            }
            selected = 1;
        }
        screen->drawListMenu("MATRICES", menuOptions, 3, selected);
        screen->printImage();
        
        keyIndex = keyboardInputReceiver.getButtonKeyIndex(startTime);
    }

    return "";
}

std::string trimZeros(std::string num){
    int offset = (num.find_last_not_of('0') == num.find('.')) ? 0 : 1;
    // Remove trailing zeros and decimal place if neccessary
    num.erase(num.find_last_not_of('0') + offset, std::string::npos);

    // Remove leading zeros
    num.erase(0, num.find_first_not_of('0'));

    // If num was already zero return 0
    num = (num == "") ? "0" : num;

    return num;
}

int main() {
    stdio_init_all();
    
    //For use when debugging to enter COM terminal before crash/halt/block
    /*
    sleep_ms(3 * 1000);
    std::cout << "!!BEGIN!!" << std::endl;
    fflush(stdout);
    */
    
    /*
    #ifdef DEBUG
    if(watchdog_caused_reboot())
        std::cout << "Watchdog caused this boot!" << std::endl;
    #endif
    watchdog_enable(0x7fffff, true);//TODO: Monitor watchdog timer 0x7fffff ~= 8.3s, but this depends on frequency
    */

    std::string expression = "";
    outputTypeEnum outputType = DECIMAL;
    angleUnitEnum angleUnit = RAD;
    
    std::string storedFunctions[4];

    //Initialize the screen
    Screen *screen = new Screen();
    screenPtr = screen;
    //Draw Strings on image
    screen->drawString(1, 24, "BOUNDLESS", &Font20, BLACK, WHITE);
    screen->drawString(1, 48, "Graphing Calculator", &Font12, BLACK, WHITE);
    screen->printImage();
    DEV_Delay_ms(3000);
    //Display blank white screen
    screen->clearImage();
    screen->printImage();
   
    #ifndef NO_BUTTONS
    KeyboardInputReceiver keyboardInputReceiver = KeyboardInputReceiver(screen);
    keyboardPtr = &keyboardInputReceiver;
    #endif

    std::map<std::string, double> variables = {{"ans", 0}, {"PI", 3.14159265358979323846}, {"e", 2.71828182845904523536}};
    int wasTest = rtc.readTest();
    if(wasTest == 1)
        enterTestMode(rtc.seed, rtc.duration, rtc.features, NULL, false);//Don't show code on startup
    else if(wasTest == 0)
        gpio_set_irq_enabled_with_callback(TRANS_DIO0, GPIO_IRQ_EDGE_RISE, true, &receivedMsgIRQ);
    else {
        std::cout << "RTC BATTERY HAS RUN DRY YOU CHEATER" << std::endl;
        screen->clearImage();
        screen->drawWrappingString(1, 1, "RTC battery has run dry. Test mode will be disabled until battery is replaced.", &Font12, BLACK, WHITE);
        screen->printImage();
        screenPtr->clearImage();
    }

    rtc.readVariables(variables);

    Graph graph(variables, &parser);

    while (1) {
        #ifndef NO_BUTTONS
        expression = keyboardInputReceiver.getExpression(variables);
        if (expression == "") {
            continue;
        }
        #else
        std::cin >> expression;
        #endif

        if (expression == "Y="){
            functionMenu(storedFunctions, keyboardInputReceiver, screen);
            continue;
        }
        else if(expression == "MATRIX"){
            std::string matrixName = matrixMenu(keyboardInputReceiver, screen);
            keyboardInputReceiver.buildExpression(matrixName);
            continue;
        }
        else if(expression == "TRIG"){
            std::string trigFunc = trigMenu(keyboardInputReceiver, screen);
            keyboardInputReceiver.buildExpression(trigFunc);
            continue;
        }
        else if(expression == "LOGIC"){
            std::string logicFunc = logicMenu(keyboardInputReceiver, screen);
            keyboardInputReceiver.buildExpression(logicFunc);
            continue;
        }
        else if(expression == "MODE"){
            modeMenu(keyboardInputReceiver, screen, &outputType, &angleUnit);
            parser.setAngleUnit(angleUnit == DEG ? true : false);
            keyboardInputReceiver.buildExpression("");
            continue;
        }

        if (expression == "quit")
            break;
            
        try {
            if(expression == "GRAPH"){
                graphScreen(storedFunctions, &graph, &keyboardInputReceiver, screen);
                continue;
            }

            Result result = parser.parse(expression).evaluate(variables);
            Eigen::MatrixXd matrixResult;
            std::complex<double> complexResult;
            double value;
            if (result.dataTypeEnum == DataTypeEnum::matrix){
                matrixResult = result.matrix;
                for (int i = 0; i < matrixResult.rows(); i++){
                    for (int j = 0; j < matrixResult.cols(); j++){
                        std::cout << matrixResult(i,j) << std::endl;
                    }
                }
                screen->drawString(1, 32, "=", &Font16, BLACK, WHITE);
                screen->drawMatrixResult(matrixResult, 1, 48);
                screen->printImage();
                continue;
            }
            else{
                complexResult = result.getComplex();
                value = real(complexResult);
            }
            variables["ans"] = value;
            std::cout << "\n=";
            switch (outputType)
                {
                case DECIMAL: {
                    std::ostringstream stream;
                    std::string complexExpressionOp = "";                        
                    if (imag(complexResult) != 0) {
                        complexExpressionOp = imag(complexResult) < 0 ? " - " : " + ";
                        if (real(complexResult) != 0) {
                            std::cout << real(complexResult) << complexExpressionOp << abs(imag(complexResult)) << "i";

                            stream << real(complexResult) << complexExpressionOp << abs(imag(complexResult)) << "i"; 
                            screen->drawWrappingString(1, 32, "= " + trimZeros(std::__cxx11::to_string(real(complexResult))) + complexExpressionOp + trimZeros(std::__cxx11::to_string(abs(imag(complexResult)))) + "i", &Font16, BLACK, WHITE);
                        } else {
                            std::cout << imag(complexResult) << "i";
            
                            stream << imag(complexResult);
                            screen->drawString(1, 32, "= " + trimZeros(stream.str()) + "i", &Font16, BLACK, WHITE);
                        }
                    } else {
                        std::cout << std::defaultfloat << real(complexResult);

                        stream << std::defaultfloat << real(complexResult);
                        screen->drawString(1, 32, "= " + trimZeros(stream.str()), &Font16, BLACK, WHITE);
                    }
                    fflush(stdout);
                    screen->printImage();
                    break;
                }
                case BINARY: {
                    std::bitset<32> x(value);
                    std::cout << x;
                    screen->drawString(1, 32, "= " + trimZeros(x.to_string()), &Font16, BLACK, WHITE);
                    screen->printImage();
                    break;
                }
                case OCTAL: {
                    // TO DO: Convert to octal appropriately as std::oct does not work with double data types.
                    std::cout << std::oct << value;
                    break;
                }
                case HEXADECIMAL: {
                    std::cout << std::hexfloat << value;
                    break;
                }
                default: {
                    std::cout << std::defaultfloat << value;
                    screen->drawString(1, 32, "= " + trimZeros(std::__cxx11::to_string(value)), &Font16, BLACK, WHITE);
                    screen->printImage();
                    break;
                }
            }

            std::cout << "\n";
        } catch (const DisabledFeatureException& e) {
            char eString[100];
            char duration[4] = {};
            strcpy(eString, e.what());
            strcat(eString, " for ");
            snprintf(duration, sizeof(duration), "%d", (int) (enteredDuration - absolute_time_diff_us(enteredTime, get_absolute_time())/(1000*1000*60)));
            strcat(eString, duration);
            strcat(eString, " min(s)");
            screen->drawWrappingString(1, 32, eString, &Font12, BLACK, WHITE);
            screen->printImage();
        } catch (const std::exception& e) {
            screen->drawWrappingString(1, 100, e.what(), &Font12, BLACK, WHITE);
            screen->printImage();
        }        
    }

    rtc.storeVariables(variables);

    screen->cleanup();
    delete screen;

    return 0;
}
