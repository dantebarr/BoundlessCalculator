#include "Lepton.h"
#include "KeyboardInputReceiver.h"
#include "pico/stdlib.h"
#include <iostream>
//Screen Stuff
#include "Screen.h"


Screen::Screen() {
    /* Might want to catch this
    if(DEV_Module_Init()!=0){
        return -1;
    }
    */
    imageWidth = LCD_1IN8_WIDTH;
    imageHeight = LCD_1IN8_HEIGHT;
    imageSize = imageWidth*imageHeight/8 + 1;
    DEV_Module_Init();
    LCD_1IN8_Init(HORIZONTAL);
    LCD_1IN8_Clear(WHITE);
    if((image = (UBYTE *)malloc(imageSize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        exit(0);
    }
    Paint_NewImage(image, imageHeight, imageWidth, 0, WHITE);
    Paint_SetScale(65);
    clearImage();
}

void Screen::clearImage(){
    for(int i = 0; i < imageSize; i++){
        image[i] = 0;
    }
}

void Screen::drawString(UWORD Xstart, UWORD Ystart, std::string pString, sFONT* Font, UWORD Foreground, UWORD Background){
    const char * c = pString.c_str();
    Paint_DrawString_EN(Xstart, Ystart, c, Font, BLACK, WHITE);
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

void Screen::printInitialRemoteMenu(std::string menuName, std::string menuList[], uint8_t selected){
    clearImage();
    if (menuName.length() > 13){
        menuName = "TitleTooLong";
    }
    uint16_t titleStartPixX = imageWidth/2 - (menuName.length() * 8 / 2);
    drawString(titleStartPixX, 1, menuName, &Font16, BLACK, WHITE);
    

    //Draw arrow beside the currently selected function
    drawRightArrow(4, 22 + selected*27, BLACK);

    drawString(15, 20, menuList[0], &Font12, BLACK, WHITE);
    drawString(15, 47, menuList[1], &Font12, BLACK, WHITE);
    drawString(15, 74, menuList[2], &Font12, BLACK, WHITE);
    drawString(15, 101, menuList[3], &Font12, BLACK, WHITE);   

    printImage();
}

void Screen::printRemoteFunctionsMenu(std::string menuName, std::string menuList[], uint8_t menuLength, uint8_t selected) {
    clearImage();
    if (menuName.length() > 13){
        menuName = "TitleTooLong";
    }
    uint16_t titleStartPixX = imageWidth/2 - (menuName.length() * 8 / 2);
    drawString(titleStartPixX, 1, menuName, &Font16, BLACK, WHITE);

    //Draw arrow beside the currently selected function
    drawRightArrow(4, 22 + selected%4*27, BLACK);
    uint8_t page = selected / 4 ;
    if (4*page < menuLength)
        drawString(15, 20, menuList[4*page], &Font12, BLACK, WHITE);
    if (4*page+1 < menuLength)
        drawString(15, 47, menuList[4*page+1], &Font12, BLACK, WHITE);
    if (4*page+2 < menuLength)
        drawString(15, 74, menuList[4*page+2], &Font12, BLACK, WHITE);
    if (4*page+3 < menuLength)
        drawString(15, 101, menuList[4*page+3], &Font12, BLACK, WHITE);

    printImage();
}

void Screen::drawListMenu(std::string title, std::string options[], uint8_t numOptions, uint8_t selected){
    clearImage();
    if (title.length() > 13){
        title = "TitleTooLong";
    }
    uint16_t titleStartPixX = imageWidth/2 - (title.length() * 8 / 2);
    drawString(titleStartPixX, 1, title, &Font16, BLACK, WHITE);

    //Draw arrow beside the currently selected function
    drawRightArrow(4, 22 + selected*15, BLACK);

    //TODO handle more than 8 options
    for (int i = 0; i < numOptions; i++){
        drawString(15, 20 + i*15, options[i], &Font12, BLACK, WHITE);
    }
}

void Screen::drawCheckListMenu(std::string title, std::string options[], uint8_t numOptions, uint8_t selected, bool checked[]){
    clearImage();
    uint8_t maxOptionsPerScreen = 7;
    uint8_t boxSide = 6;
    if (title.length() > 13){
        title = "TitleTooLong";
    }
    uint16_t titleStartPixX = imageWidth/2 - (title.length() * 8 / 2);
    drawString(titleStartPixX, 1, title, &Font16, BLACK, WHITE);

    //Draw arrow beside the currently selected function
    drawRightArrow(4, 22 + selected%maxOptionsPerScreen*15, BLACK);
    uint8_t page = selected / 7;
    for (int i = page*maxOptionsPerScreen; i < page*maxOptionsPerScreen+maxOptionsPerScreen; i++){
        if (i < numOptions) {
            Paint_DrawRectangle(18, 23 + i%maxOptionsPerScreen*15, 18 + boxSide, 23 + i%maxOptionsPerScreen*15 + boxSide, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
            if (checked[i]) {
                drawX5x5(18, 23 + i%maxOptionsPerScreen*15, BLACK);
            }

            drawString(30, 20 + i%maxOptionsPerScreen*15, options[i], &Font12, BLACK, WHITE);
        }
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

void Screen::cleanup(){
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

