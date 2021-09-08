#ifndef KEYBOARD_INPUT_RECEIVER_H_
#define KEYBOARD_INPUT_RECEIVER_H_

#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <map>
#include <vector>
#include <Screen.h>

class KeyboardInputReceiver {
public:
    KeyboardInputReceiver(Screen *screen);
    std::string getExpression(std::map<std::string, double> &variables);
    int getMenuKey();
    int getButtonKeyIndex(int64_t &startTime);
    std::string getkeyIndexValue(int keyIndex);
    void buildExpression(std::string expressionComponent);

private:
    static const int M = 10;
    static const int N = 5;
    static const uint8_t buttonRows[];
    static const uint8_t buttonCols[];
    std::map<int, int> primaryKeyEncodings;
    std::map<int, int> secondaryKeyEncodings;
    std::map<int, std::string> primaryPrintableKeys;
    std::map<int, std::string> secondaryPrintableKeys;
    std::map<int, std::string> primaryMenuKeys;
    std::map<int, std::string> secondaryMenuKeys;

    bool secondaryMode;
    std::vector<std::string> expressionComponents;
    std::string expression;

    Screen *screen__;

    void pinSetup();
    void initializeKeyMappings();
    int64_t debounce(int64_t startTime);
    void buildExpression(int keyIndex);
    bool storeValue(std::string expressionToStore, std::map<std::string, double> &variables);
    void clearExpression();
    void toggleSecondaryMode();
    int getKeyIndex(int row, int col);
    void deleteButtonPressed();
    void createExpressionString();
};

#endif // KEYBOARD_INPUT_RECEIVER_H_
