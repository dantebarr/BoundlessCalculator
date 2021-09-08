#ifndef SCREEN_H_
#define SCREEN_H_

#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <map>
#include <set>
#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "Debug.h"
#include "LCD_1in8.h"
#include "Lepton.h"
#include <stdlib.h>


class Screen {
public:
    Screen();
    void clearImage();
    void drawString(UWORD Xstart, UWORD Ystart, std::string pString, sFONT* Font, UWORD Foreground, UWORD Background);
    void drawWrappingString(UWORD Xstart, UWORD Ystart, std::string pString, sFONT* Font, UWORD Foreground, UWORD Background);
    void printImage();
    void printFunctionMenu(std::string storedFunctions[], uint8_t selected);
    void printGraphFunctionMenu(uint8_t marker1, uint8_t marker2, unsigned int fn1, unsigned int fn2);
    void printLimitMenu(std::string storedLimits[], uint8_t selected);
    void drawListMenu(std::string title, std::string options[], uint8_t numOptions, uint8_t selected);
    void drawCheckListMenu(std::string title, std::string options[], uint8_t numOptions, uint8_t selected, bool checked[]);
    void drawX5x5(uint16_t Xstart, uint16_t Ystart, uint16_t Colour);
    void printInitialRemoteMenu(std::string menuName, std::string menuList[], uint8_t selected);
    void printRemoteFunctionsMenu(std::string menuName, std::string menuList[], uint8_t menuLength, uint8_t selected);
    void drawRightArrow(uint16_t Xstart, uint16_t Ystart, uint16_t Colour);
    void drawMatrixResult(Eigen::MatrixXd matrix, uint16_t Xstart, uint16_t Ystart);
    void drawMatrixEditMenu(std::string matrixName, uint8_t rows, uint8_t cols, uint8_t selected, std::string expression);
    void cleanup();
    void debug();
private:
    UWORD imageWidth;
    UWORD imageHeight;
    uint32_t imageSize;
    UBYTE* image;
};

#endif // SCREEN_H_
