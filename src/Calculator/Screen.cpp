#include "Lepton.h"
#include "KeyboardInputReceiver.h"
#include "pico/stdlib.h"
#include <iostream>
#include <vector>
#include <string.h>
//Screen Stuff
#include "Screen.h"


Screen::Screen() {
    /* Might want to catch this
    if(DEV_Module_Init()!=0){
        return -1;
    }
    */
    imageHeight = LCD_1IN8_WIDTH;
    imageWidth = LCD_1IN8_HEIGHT;
    imageSize = imageWidth*imageHeight/8 + 1;
    DEV_Module_Init();
    LCD_1IN8_Init(HORIZONTAL);
    LCD_1IN8_Clear(WHITE);
    if((image = (UBYTE *)malloc(imageSize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        exit(0);
    }
    Paint_NewImage(image, imageWidth, imageHeight, 0, WHITE);
    Paint_SetScale(65);
    clearImage();
}

void Screen::clearImage(){
    memset(image, 0, imageSize);
}

void Screen::drawString(UWORD Xstart, UWORD Ystart, std::string pString, sFONT* Font, UWORD Foreground, UWORD Background){
    const char * c = pString.c_str();
    Paint_DrawString_EN(Xstart, Ystart, c, Font, BLACK, WHITE);
}

void Screen::drawWrappingString(UWORD Xstart, UWORD Ystart, std::string pString, sFONT* Font, UWORD Foreground, UWORD Background) {
    if ((int) pString.length() * (int) Font->Width < (int) imageWidth || pString.find(" ") == std::string::npos) {
        drawString(Xstart, Ystart, pString, Font, Foreground, Background);
        return;
    }

    std::vector<std::string> tokens;
    char * cString = (char*) pString.c_str();
    char * token = NULL;
    token = strtok(cString, " ");
    //Split string into a vector of tokens delimited by a space
    while (token != NULL) {
        //Draw string normally if single token length is longer than screen width
        if ((int) std::string(token).length() * (int) Font->Width > (int) imageWidth) {
            drawString(Xstart, Ystart, pString, Font, Foreground, Background);
            return;
        }
        tokens.push_back(std::string(token));
        token = strtok(NULL, " ");
    }

    std::string tmpString = "";
    UWORD yOffset = Ystart;
    //Iterate through tokens and create new lines to draw
    for (auto &t : tokens) {
        if (((int) tmpString.length() + 1 + (int) t.length()) * ((int) Font->Width) < (int) imageWidth) {            
            tmpString += " " + t;
        } else {
            Paint_DrawString_EN(Xstart, yOffset, tmpString.substr(tmpString.find_first_not_of(" "), std::string::npos).c_str(), Font, BLACK, WHITE);
            yOffset += Font->Height;
            tmpString = t;
        }
    }
    if (tmpString != "") {
        Paint_DrawString_EN(Xstart, yOffset, tmpString.substr(tmpString.find_first_not_of(" "), std::string::npos).c_str(), Font, BLACK, WHITE);
    }
}

void Screen::printImage(){
    LCD_1IN8_Display(image);
}

void Screen::printFunctionMenu(std::string storedFunctions[], uint8_t selected){
    clearImage();
    drawString(30, 1, "FUNCTIONS", &Font16, BLACK, WHITE);
    
    //Draw lines to separate functions and menu title, currently not working
    //Paint_DrawLine(10, 20, 140, 60, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

    //Draw arrow beside the currently selected function
    drawRightArrow(4, 22 + selected*27, BLACK);

    drawString(15, 20, "f1(x)=" + storedFunctions[0], &Font12, BLACK, WHITE);
    drawString(15, 47, "f2(x)=" + storedFunctions[1], &Font12, BLACK, WHITE);
    drawString(15, 74, "f3(x)=" + storedFunctions[2], &Font12, BLACK, WHITE);
    drawString(15, 101, "f4(x)=" + storedFunctions[3], &Font12, BLACK, WHITE);   

    printImage();
}

void Screen::printGraphFunctionMenu(uint8_t marker1, uint8_t marker2, unsigned int fn1, unsigned int fn2){
    clearImage();
    drawString(27, 1, "GRAPH FUNCTIONS", &Font12, BLACK, WHITE);

    drawRightArrow(4, 18 + marker1*25, BLACK);
    
    drawString(15, 16, "Max", &Font12, BLACK, WHITE);
    drawString(15, 41, "Min", &Font12, BLACK, WHITE);
    drawString(15, 66, "Zero", &Font12, BLACK, WHITE);
    drawString(15, 91, "Intersect", &Font12, BLACK, WHITE);
    drawString(15, 116, "Integral", &Font12, BLACK, WHITE);

    if(marker2){
        drawRightArrow(80, 18+(marker2-1)*25, BLACK);
        drawString(96, 16, "f(x): "+std::to_string((int) fn1+1), &Font12, BLACK, WHITE);//Cast to int because chars are letters
        if(marker1 == 3)//Intersect has two options
            drawString(96, 41, "g(x): "+std::to_string((int) fn2+1), &Font12, BLACK, WHITE);
    }
    
    printImage();
}

void Screen::printLimitMenu(std::string storedLimits[], uint8_t selected){
    clearImage();
    drawString(13, 1, "GRAPH LIMITS", &Font16, BLACK, WHITE);

    //Draw arrow beside the currently selected function
    drawRightArrow(4, 22 + selected*27, BLACK);

    drawString(15, 20, "Left =" + storedLimits[0], &Font12, BLACK, WHITE);
    drawString(15, 47, "Right =" + storedLimits[1], &Font12, BLACK, WHITE);
    drawString(15, 74, "Top =" + storedLimits[2], &Font12, BLACK, WHITE);
    drawString(15, 101, "Bottom =" + storedLimits[3], &Font12, BLACK, WHITE);   

    printImage();
}

void Screen::drawListMenu(std::string title, std::string options[], uint8_t numOptions, uint8_t selected){
    /*
    Dont pass in more than 7 options I'm too lazy to check for it and handle it
    */
    clearImage();
    if (title.length() > 13){
        title = "TitleTooLong";
    }
    uint16_t titleStartPixX = imageWidth/2 - (title.length() * 12 / 2);
    drawString(titleStartPixX, 1, title, &Font16, BLACK, WHITE);

    //Draw arrow beside the currently selected function
    drawRightArrow(4, 22 + selected*15, BLACK);

    //TODO handle more than 8 options
    for (int i = 0; i < numOptions; i++){
        drawString(15, 20 + i*15, options[i], &Font12, BLACK, WHITE);
    }
}

void Screen::drawCheckListMenu(std::string title, std::string options[], uint8_t numOptions, uint8_t selected, bool checked[]){
    /*
    Dont pass in more than 7 options I'm too lazy to check for it and handle it
    */
    clearImage();
    uint8_t boxSide = 6;
    if (title.length() > 13){
        title = "TitleTooLong";
    }
    uint16_t titleStartPixX = imageWidth/2 - (title.length() * 8 / 2);
    drawString(titleStartPixX, 1, title, &Font16, BLACK, WHITE);

    //Draw arrow beside the currently selected function
    drawRightArrow(4, 22 + selected*15, BLACK);

    //TODO handle more than 8 options
    for (int i = 0; i < numOptions; i++){
        Paint_DrawRectangle(18, 23 + i*15, 18 + boxSide, 23 + i*15 + boxSide, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
        if(checked[i]){
            drawX5x5(18, 23 + i*15, BLACK);
        }
        drawString(30, 20 + i*15, options[i], &Font12, BLACK, WHITE);
    }
}

void Screen::drawX5x5(uint16_t Xstart, uint16_t Ystart, uint16_t Colour){
    Paint_SetPixel(Xstart, Ystart, Colour);
    Paint_SetPixel(Xstart + 4, Ystart, Colour);

    Paint_SetPixel(Xstart + 1, Ystart + 1, Colour);
    Paint_SetPixel(Xstart + 3, Ystart + 1, Colour);

    Paint_SetPixel(Xstart + 2, Ystart + 2, Colour);

    Paint_SetPixel(Xstart + 1, Ystart + 3, Colour);
    Paint_SetPixel(Xstart + 3, Ystart + 3, Colour);

    Paint_SetPixel(Xstart, Ystart + 4, Colour);
    Paint_SetPixel(Xstart + 4, Ystart + 4, Colour);
}

void Screen::drawRightArrow(uint16_t Xstart, uint16_t Ystart, uint16_t Colour){
    Paint_SetPixel(Xstart + 5, Ystart, BLACK);
    Ystart += 1;
    Paint_SetPixel(Xstart + 5, Ystart, BLACK);
    Paint_SetPixel(Xstart + 6, Ystart, BLACK);
    Ystart += 1;
    for(int i = 0; i < 8; i++){
        Paint_SetPixel(Xstart + i, Ystart, BLACK);
    }
    Ystart += 1;
    for(int i = 0; i < 9; i++){
        Paint_SetPixel(Xstart + i, Ystart, BLACK);
    }
    Ystart += 1;
    for(int i = 0; i < 8; i++){
        Paint_SetPixel(Xstart + i, Ystart, BLACK);
    }
    Ystart += 1;
    Paint_SetPixel(Xstart + 5, Ystart, BLACK);
    Paint_SetPixel(Xstart + 6, Ystart, BLACK);
    Ystart += 1;
    Paint_SetPixel(Xstart + 5, Ystart, BLACK);
}

void Screen::drawMatrixResult(Eigen::MatrixXd matrix, uint16_t Xstart, uint16_t Ystart){
    std::string row = "[";
    std::string num = "";
    int offset = 0;
    for (int i = 0; i < matrix.rows(); i++){
        for (int j = 0; j < matrix.cols(); j++){
            //Remove trailing 0's
            num = std::to_string(matrix(i,j));
            if (num.find_last_not_of('0') == num.find('.')){
                offset = 0;
            }
            else{
                offset = 1;
            }
            num.erase(num.find_last_not_of('0') + offset, std::string::npos);
            row += num;
            if (j < matrix.cols()-1){
                row += ", ";
            }
        }
        row += "]";
        drawString(Xstart, Ystart+i*12, row, &Font12, BLACK, WHITE);
        row = "[";
    }
}

void Screen::drawMatrixEditMenu(std::string matrixName, uint8_t rows, uint8_t cols, uint8_t selected, std::string expression){
    drawString(1, 1, "EDIT-" + matrixName, &Font16, BLACK, WHITE);
    drawString(82, 1, "[  ,  ]", &Font16, BLACK, WHITE);
    uint8_t cellSelectLength;
    uint8_t curRow;
    uint8_t curCol;
    uint8_t prevRow;
    uint8_t prevCol;
    if(cols != 0){
        cellSelectLength = 130/cols;
        prevRow = (selected-3) / cols;
        prevCol = (selected-3) % cols;
        curRow = (selected-2) / cols;
        curCol = (selected-2) % cols;
    }

    uint16_t foreground = BLACK;
    uint16_t background = WHITE;

    if(selected == 0){
        Paint_DrawLine(92, 16, 117, 16, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
        drawString(92, 1, expression, &Font16, BLACK, WHITE);
    }
    else if(selected == 1){
        //Remove previous selection line
        Paint_DrawLine(92, 16, 117, 16, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
        //Draw new selection line
        Paint_DrawLine(124, 16, 149, 16, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
        drawString(124, 1, expression, &Font16, BLACK, WHITE);
    }
    else if(selected == 2){
        //Remove previous selection line
        Paint_DrawLine(124, 16, 149, 16, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    }

    for(uint8_t i = 0; i < rows; i++){
        drawString(1, 16 + i*16, "[", &Font12, BLACK, WHITE);
        drawString(146, 16 + i*16, "]", &Font12, BLACK, WHITE);
    }

    //Remove previous matrix selection underline
    if(selected > 2){
        Paint_DrawLine((13 + prevCol*cellSelectLength), (28 + prevRow*16), (13 + (prevCol+1)*cellSelectLength), (28 + prevRow*16), WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    }

    if(selected > 1){
        Paint_DrawLine((13 + curCol*cellSelectLength), (28 + curRow*16), (13 + (curCol+1)*cellSelectLength), (28 + curRow*16), BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
        drawString((13 + curCol*cellSelectLength), (16 + curRow*16), expression, &Font12, BLACK, WHITE);
    }

}

void Screen::cleanup(){
    LCD_1IN8_Clear(BLACK);
    free(image);
    DEV_Module_Exit();
}

void Screen::debug(){
    image[5] = 63;
    image[10] = 63;
    image[2000] = 63;
    image[2030] = 63;
    image[2400] = 63;
    image[2050] = 63;
    image[2060] = 63;
    int LSB = 0;
    /*
    for (int j = 0; j < LCD_1IN8.HEIGHT; j++) {
        for (int i = 0; i < LCD_1IN8.WIDTH; i++){
            //Check bit value
            LSB = image[(j*LCD_1IN8.WIDTH + i)/8] >> (7 - (j*LCD_1IN8.WIDTH + i) % 8) & 1;
            //std::cout << LSB;
            //fflush(stdout);
        }
        //std::cout << std::endl;
        //fflush(stdout);
    }
    */

}
