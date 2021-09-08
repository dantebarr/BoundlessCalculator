#ifndef REMOTE_KEYBOARD_INPUT_RECEIVER_H_
#define REMOTE_KEYBOARD_INPUT_RECEIVER_H_

#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <map>

class RemoteKeyboardInputReceiver {
public:
    RemoteKeyboardInputReceiver();

    int getMenuKey();    

private:
    static const int M = 2;
    static const int N = 3;
    static const uint8_t buttonRows[];
    static const uint8_t buttonCols[];
    static const std::map<int, int> keyEncodings;

    void pinSetup();
    int64_t debounce(int64_t startTime);
    int getKeyIndex(int row, int col);
};

#endif // REMOTE_KEYBOARD_INPUT_RECEIVER_H_
