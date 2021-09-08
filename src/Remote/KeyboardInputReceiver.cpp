#include "Lepton.h"
#include "KeyboardInputReceiver.h"
#include "pico/stdlib.h"
#include <iostream>
#include <map>
#include <vector>

// {"Y = ", "GRAPH", "left", "up", "right",
//  "2ND", "X,theta", "GRAPH_FN", "GRAPH_CTRL", "down",
//  "SPEC", "log10", "ln", "d/dx", "integral",
//  "sigma", "i", "TRIG", "sin", "cos",
//  "tan", "(", ")", "Ans", "LOGIC",
//  "SOLVE", "STAT", "MODE", "STO", "ON",
//  "9", "8", "7", "6", "5",
//  "4", "3", "2", "1", "0",
//  ".", "DELETE", "^", " / ", " * ",
//  " - ", " + ", "ENTER", "BLANK", "BLANK"};  

const uint8_t KeyboardInputReceiver::buttonRows[] = {28, 27};
const uint8_t KeyboardInputReceiver::buttonCols[] = {18, 17, 16};

KeyboardInputReceiver::KeyboardInputReceiver(Screen *screen) : secondaryMode(false),
                                                              expression(""),
                                                              screen__(screen) {
    pinSetup();
    initializeKeyMappings();
}

void KeyboardInputReceiver::pinSetup() {
    for (int row = 0; row < 2; row++) {
        gpio_init(buttonRows[row]);
        gpio_set_dir(buttonRows[row], GPIO_OUT);
        gpio_put(buttonRows[row], 0);
    }

    for (int col = 0; col < 3; col++) {
        gpio_init(buttonCols[col]);
        gpio_set_dir(buttonCols[col], GPIO_IN);
    }
}

// void KeyboardInputReceiver::initializeKeyMappings() {
//     primaryKeyEncodings[3] = 260; // KEY_LEFT
//     primaryKeyEncodings[2] = 259; // KEY_UP
//     primaryKeyEncodings[4] = 261; // KEY_RIGHT
//     primaryKeyEncodings[9] = 258; // KEY_DOWN
//     primaryKeyEncodings[41] = 330; // KEY_DC
//     primaryKeyEncodings[47] = 10; // enter key
//     primaryKeyEncodings[29] = 10000; // ON
//     primaryKeyEncodings[0] = 0; // Y=
//     primaryKeyEncodings[7] = 7; // GRAPH FN
//     primaryKeyEncodings[8] = 8; // GRAPH CTRL
//     primaryKeyEncodings[1] = 1; // GRAPH

//     secondaryKeyEncodings[41] = 355; // KEY_CANCEL

//     primaryPrintableKeys[6] = "x"; // X,theta
//     primaryPrintableKeys[11] = "log10(";
//     primaryPrintableKeys[12] = "ln(";
//     primaryPrintableKeys[13] = "d/dx(";
//     primaryPrintableKeys[14] = "fnInt(";
//     primaryPrintableKeys[15] = "sigma(";
//     primaryPrintableKeys[16] = "i";
//     primaryPrintableKeys[18] = "sin(";
//     primaryPrintableKeys[20] = "cos(";
//     primaryPrintableKeys[21] = "tan(";
//     primaryPrintableKeys[22] = "(";
//     primaryPrintableKeys[23] = ")";
//     primaryPrintableKeys[32] = "9";
//     primaryPrintableKeys[31] = "8";
//     primaryPrintableKeys[30] = "7";
//     primaryPrintableKeys[33] = "6";
//     primaryPrintableKeys[34] = "5";
//     primaryPrintableKeys[35] = "4";
//     primaryPrintableKeys[36] = "3";
//     primaryPrintableKeys[37] = "2";
//     primaryPrintableKeys[38] = "1";
//     primaryPrintableKeys[39] = "0";
//     primaryPrintableKeys[40] = ".";
//     primaryPrintableKeys[42] = "^";
//     primaryPrintableKeys[43] = "/";
//     primaryPrintableKeys[44] = "*";
//     primaryPrintableKeys[26] = "-";
//     primaryPrintableKeys[46] = "+";

//     secondaryPrintableKeys[11] = "x10^";
//     secondaryPrintableKeys[12] = "xe^";
//     secondaryPrintableKeys[13] = "lim(";
//     secondaryPrintableKeys[15] = "PI";
//     secondaryPrintableKeys[18] = "asin(";
//     secondaryPrintableKeys[19] = "acos(";
//     secondaryPrintableKeys[20] = "atan(";
//     secondaryPrintableKeys[21] = "[";
//     secondaryPrintableKeys[22] = "]";
//     secondaryPrintableKeys[30] = "C";
//     secondaryPrintableKeys[31] = "B";
//     secondaryPrintableKeys[32] = "A";
//     secondaryPrintableKeys[33] = "F";
//     secondaryPrintableKeys[34] = "E";
//     secondaryPrintableKeys[35] = "D";
//     secondaryPrintableKeys[36] = "I";
//     secondaryPrintableKeys[37] = "H";
//     secondaryPrintableKeys[38] = "G";
//     secondaryPrintableKeys[39] = "J";
//     secondaryPrintableKeys[40] = ",";
//     secondaryPrintableKeys[42] = "sqrt(";
//     secondaryPrintableKeys[43] = "MOD(";
//     secondaryPrintableKeys[44] = "LCM(";
//     secondaryPrintableKeys[45] = "GCD(";
//     secondaryPrintableKeys[46] = "!";  
    
//     primaryMenuKeys[27] = "MODE";
// }


void KeyboardInputReceiver::initializeKeyMappings() {
    primaryKeyEncodings[2] = 260; // KEY_LEFT
    primaryKeyEncodings[3] = 259; // KEY_UP
    primaryKeyEncodings[4] = 261; // KEY_RIGHT
    primaryKeyEncodings[9] = 258; // KEY_DOWN
    primaryKeyEncodings[41] = 330; // KEY_DC
    primaryKeyEncodings[47] = 10; // enter key
    primaryKeyEncodings[29] = 10000; // ON
    primaryKeyEncodings[0] = 0; // Y=

    secondaryKeyEncodings[41] = 355; // KEY_CANCEL

    primaryPrintableKeys[11] = "log10";
    primaryPrintableKeys[12] = "ln";
    primaryPrintableKeys[13] = "d/dx";
    primaryPrintableKeys[14] = "integral";
    primaryPrintableKeys[15] = "sigma";
    primaryPrintableKeys[16] = "i";
    primaryPrintableKeys[18] = "sin";
    primaryPrintableKeys[19] = "cos";
    primaryPrintableKeys[20] = "tan";
    primaryPrintableKeys[21] = "(";
    primaryPrintableKeys[22] = ")";
    primaryPrintableKeys[30] = "9";
    primaryPrintableKeys[31] = "8";
    primaryPrintableKeys[32] = "7";
    primaryPrintableKeys[33] = "6";
    primaryPrintableKeys[34] = "5";
    primaryPrintableKeys[35] = "4";
    primaryPrintableKeys[36] = "3";
    primaryPrintableKeys[37] = "2";
    primaryPrintableKeys[38] = "1";
    primaryPrintableKeys[39] = "0";
    primaryPrintableKeys[40] = ".";
    primaryPrintableKeys[42] = "^";
    primaryPrintableKeys[43] = " / ";
    primaryPrintableKeys[44] = " * ";
    primaryPrintableKeys[45] = " - ";
    primaryPrintableKeys[46] = " + ";

    secondaryPrintableKeys[11] = "x10^";
    secondaryPrintableKeys[12] = "xe^";
    secondaryPrintableKeys[13] = "lim";
    secondaryPrintableKeys[15] = "PI";
    secondaryPrintableKeys[18] = "asin";
    secondaryPrintableKeys[19] = "acos";
    secondaryPrintableKeys[20] = "atan";
    secondaryPrintableKeys[21] = "[";
    secondaryPrintableKeys[22] = "]";
    secondaryPrintableKeys[30] = "C";
    secondaryPrintableKeys[31] = "B";
    secondaryPrintableKeys[32] = "A";
    secondaryPrintableKeys[33] = "F";
    secondaryPrintableKeys[34] = "E";
    secondaryPrintableKeys[35] = "D";
    secondaryPrintableKeys[36] = "I";
    secondaryPrintableKeys[37] = "H";
    secondaryPrintableKeys[38] = "G";
    secondaryPrintableKeys[39] = "J";
    secondaryPrintableKeys[40] = ",";
    secondaryPrintableKeys[42] = "sqrt";
    secondaryPrintableKeys[43] = "MOD";
    secondaryPrintableKeys[44] = "LCM";
    secondaryPrintableKeys[45] = "GCD";
    secondaryPrintableKeys[46] = "!";  
    
    primaryMenuKeys[27] = "MODE";
}

int64_t KeyboardInputReceiver::debounce(int64_t startTime) {
    int64_t endTime = time_us_64();
    if (endTime - startTime > 300*1000) {
        return endTime;
    }
    return startTime;
}

// Retrieve a single key from the keypad
int KeyboardInputReceiver::getButtonKeyIndex(int64_t &startTime) {
    int keyIndex = 0;
    int64_t tmpTime = 0;
    while (true) {
        for (int row = 0; row < 2; row++) {
            gpio_put(buttonRows[row], 1);
            for (int col = 0; col < 3; col++) {
                if (gpio_get(buttonCols[col])) {
                    tmpTime = debounce(startTime);
                    if (startTime != tmpTime) {
                        startTime = tmpTime;
                        keyIndex = getKeyIndex(row, col);
                        //std::cout << "KEYINDEX: " << keyIndex << std::endl;
                        // 2nd button pressed
                        if (keyIndex == 5) {
                            toggleSecondaryMode();                        
                        } else {
                            return keyIndex;
                        }
                    }
                }
            }
            gpio_put(buttonRows[row], 0);
        }
    }
}

void KeyboardInputReceiver::buildExpression(int keyIndex) {
    if (secondaryMode && secondaryPrintableKeys.find(keyIndex) != secondaryPrintableKeys.end()) {
        expressionComponents.push_back(secondaryPrintableKeys[keyIndex]);
        createExpressionString();
        
        // Console output for testing:
        std::cout<<expression<<std::endl;
    } else if (primaryPrintableKeys.find(keyIndex) != primaryPrintableKeys.end()) {        
        expressionComponents.push_back(primaryPrintableKeys[keyIndex]);
        createExpressionString();

        // Console output for testing:
        std::cout<<expression<<std::endl;

        // std::cout << expressionComponent;
        // expression += expressionComponent;
        // fflush(stdout);
    }
}

std::string KeyboardInputReceiver::getkeyIndexValue(int keyIndex) {
    if (secondaryMode && secondaryPrintableKeys.find(keyIndex) != secondaryPrintableKeys.end()) {
        return secondaryPrintableKeys[keyIndex];
    } else if (primaryPrintableKeys.find(keyIndex) != primaryPrintableKeys.end()) {
        return primaryPrintableKeys[keyIndex];
    } else {
        return "";
    }
}

// Evaluates expressionToStore and adds it to the variables map with associated variable name
bool KeyboardInputReceiver::storeValue(std::string expressionToStore, std::map<std::string, double> &variables) {
    std::cout << " -> ";
    fflush(stdout);

    std::string variableName = "";
    double valueToStore = Lepton::Parser().parse(expressionToStore).evaluate(variables).getReal();

    int64_t startTime = time_us_64();
    int keyIndex = 0;
    // Check if variable name is between A - J
    while (!(secondaryMode && 30 <= keyIndex && keyIndex <= 39)) {
        keyIndex = getButtonKeyIndex(startTime);
    }
    variableName = secondaryPrintableKeys[keyIndex];
    variables[variableName] = valueToStore;

    std::cout << variableName << std::endl;
    fflush(stdout);
    return true;
}

std::string KeyboardInputReceiver::getExpression(std::map<std::string, double> &variables) { 
    if (!expressionComponents.empty()) {
        createExpressionString();        
        std::cout << expression;
    }

    int64_t startTime = time_us_64();
    int keyIndex = 0;
    std::string returnExpression = "";

    while (true) {
        keyIndex = getButtonKeyIndex(startTime);
        // 2nd menu button pressed      
        if (secondaryMode && secondaryMenuKeys.find(keyIndex) != secondaryMenuKeys.end()) {
            return secondaryMenuKeys[keyIndex];
        }
        // primary menu button pressed
        else if (primaryMenuKeys.find(keyIndex) != primaryMenuKeys.end()) {
            return primaryMenuKeys[keyIndex];
        }
        // STO button pressed
        else if (keyIndex == 28) {
            if (storeValue(expression, variables)) {
                clearExpression();
            }            
        }
        // Enter button pressed
        else if (keyIndex == 47) {
            returnExpression = expression;
            clearExpression();
            return returnExpression;
        }
        // Y= Function menu button pressed
        else if (keyIndex == 0) {
            return "Y=";
        }
        else if(keyIndex == 1)
            return "GRAPH";
        // Delete button pressed
        else if (keyIndex == 41) {
            deleteButtonPressed();
        }
        buildExpression(keyIndex);

        screen__->clearImage();
        screen__->drawString(1, 1, expression, &Font16, BLACK, WHITE);
        screen__->printImage();
    }
}

void KeyboardInputReceiver::deleteButtonPressed() {
    if (!expressionComponents.empty()) {
        expressionComponents.pop_back();
        createExpressionString();
    }
}

void KeyboardInputReceiver::createExpressionString() {
    std::string expressionString = "";
    for (std::string component : expressionComponents) {
        expressionString += component;
    }
    expression = expressionString;
}

int KeyboardInputReceiver::getMenuKey() {
    int64_t startTime = time_us_64();
    int keyIndex = 0;
    while (true) {
        keyIndex = getButtonKeyIndex(startTime);
        if (secondaryMode && secondaryKeyEncodings.find(keyIndex) != secondaryKeyEncodings.end()) {
            return secondaryKeyEncodings[keyIndex];
        } else if (primaryKeyEncodings.find(keyIndex) != primaryKeyEncodings.end()) {
            return primaryKeyEncodings[keyIndex];
        }
    }
}

void KeyboardInputReceiver::clearExpression() {
    expression = "";
    expressionComponents.clear();
}

void KeyboardInputReceiver::toggleSecondaryMode() {
    secondaryMode = !secondaryMode;
} 

int KeyboardInputReceiver::getKeyIndex(int row, int col) {
    return (row * 3) + col;
}
