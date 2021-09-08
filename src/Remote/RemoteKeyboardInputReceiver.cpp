#include "RemoteKeyboardInputReceiver.h"
#include <map>

const uint8_t RemoteKeyboardInputReceiver::buttonRows[] = {0, 5};
const uint8_t RemoteKeyboardInputReceiver::buttonCols[] = {6, 13, 19};
const std::map<int, int> RemoteKeyboardInputReceiver::keyEncodings = { {0, 356},   /*KEY_CLOSE*/
                                                                       {1, 259},   /*KEY_UP*/
                                                                       {2, 10},    /*enter key*/
                                                                       {3, 260},   /*KEY_LEFT*/                                                                                                                                              
                                                                       {4, 258},   /*KEY_DOWN*/
                                                                       {5, 261} }; /*KEY_RIGHT*/
                                                                       

RemoteKeyboardInputReceiver::RemoteKeyboardInputReceiver() {
    wiringPiSetupGpio();
    pinSetup();
}

int RemoteKeyboardInputReceiver::getMenuKey() {
    struct timespec tStart;
    clock_gettime(CLOCK_REALTIME, &tStart);
    int64_t startTime = tStart.tv_sec * INT64_C(1000) + tStart.tv_nsec / 1000000;
    int64_t tmpTime = 0;

    while (true) {
        for (int row = 0; row < M; row++) {
            digitalWrite(buttonRows[row], HIGH);
            for (int col = 0; col < N; col++) {    
                if (digitalRead(buttonCols[col]) == HIGH) {
                    tmpTime = debounce(startTime);
                    if (startTime != tmpTime) {
                        return keyEncodings.at(getKeyIndex(row, col));                        
                    }                
                }
            }
            digitalWrite(buttonRows[row], LOW);
        }  
    }
}

void RemoteKeyboardInputReceiver::pinSetup() {
    for (int row = 0; row < M; row++) {
        pinMode(buttonRows[row], OUTPUT);
        digitalWrite(buttonRows[row], LOW);
    }

    for (int col = 0; col < N; col++) {
        pinMode(buttonCols[col], INPUT);
    }
}

int64_t RemoteKeyboardInputReceiver::debounce(int64_t startTime) {
    struct timespec tEnd;    
    clock_gettime(CLOCK_REALTIME, &tEnd);
    int64_t endTime = tEnd.tv_sec * INT64_C(1000) + tEnd.tv_nsec / 1000000;
    if (endTime - startTime > 200) {
        return endTime;
    }
    return startTime;
}

int RemoteKeyboardInputReceiver::getKeyIndex(int row, int col) {
    return (row * N) + col;
}