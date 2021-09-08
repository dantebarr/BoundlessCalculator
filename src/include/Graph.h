#pragma once

#include <string>
#include <cstring>
#include "lepton/ParsedExpression.h"
#include "GUI_Paint.h"
#include "LCD_1in8.h"

//lol
#define SCREEN_WIDTH LCD_1IN8_HEIGHT
#define SCREEN_HEIGHT LCD_1IN8_WIDTH

#define Y_INTERVAL SCREEN_HEIGHT/25 //Interval for how many pixels things move when going up/down
#define X_INTERVAL SCREEN_WIDTH/30 //Interval for how many pixels things move when going left/right

#define TOO_LOW (unsigned char) -1
#define TOO_HIGH (unsigned char) -2

enum GraphState {
    XHAIR, LEFT_LINE, RIGHT_LINE
};

enum GraphFunctions {
    NONE, MAX, MIN, ZERO, INTERSECT, INTEGRAL
};

enum Direction {
    UP, DOWN, LEFT, RIGHT
};

class Graph {
    Args functions[4];
    Args fnArg; //The one that actually goes into the operation

    const std::map<std::string, double>& globalVariables;//References to globals allow us up-to date info when evaulating
    Lepton::Parser* parser;

    UWORD xXhair, yXhair; //The crosshair showing the current position
    UWORD xLineLeft, xLineRight; //The movable lines used for input for graph functions

    GraphFunctions function = NONE;
    bool shadeIntegral;
    unsigned int whichFn;

    unsigned char graphBuffer[4][SCREEN_WIDTH];

public:
    double xLeft, xRight, yBottom, yTop; //Limits of the screen
    bool dirty;
    GraphState state;

    //We probably need layers for caching

    Graph(const std::map<std::string, double>& vars, Lepton::Parser* Parser) : globalVariables(vars), parser(Parser) {
        xLeft = yBottom = -10.0;
        xRight = yTop = 10.0;
        xXhair = xLineLeft = xLineRight = SCREEN_WIDTH/2;
        yXhair = SCREEN_HEIGHT/2;
        state = XHAIR;
        shadeIntegral = false;
        whichFn = 0;
        dirty = true;
    }

private:
    //TODO: Axis ticks, gridlines?
    void drawAxis(){
        //X
        if(yBottom < 0 && yTop > 0){
            UWORD centre = yTop/(yTop - yBottom)*SCREEN_HEIGHT;
            Paint_DrawLine(0, centre, SCREEN_WIDTH-1, centre, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
        }

        //Y
        if(xLeft < 0 && xRight > 0){
            UWORD centre = -xLeft/(xRight-xLeft)*SCREEN_WIDTH;
            Paint_DrawLine(centre, 0, centre, SCREEN_HEIGHT-1, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
        }
    }

    //Returns the equivalent coordinate for a y value clamped to the screen dimensions
    UWORD yToStorage(double y){
        long yCoord = (yTop-y)/(yTop-yBottom) * SCREEN_HEIGHT;
        if(yCoord < 0)
            yCoord = TOO_LOW;
        else if(yCoord >= SCREEN_HEIGHT)
            yCoord = TOO_HIGH;
        return yCoord;
    }

    //Convert from the buffer value to the equivalent location
    UWORD storageToY(unsigned char y){
        if(y == TOO_LOW)
            y = 0;
        else if(y == TOO_HIGH)
            y = SCREEN_HEIGHT-1;
        return y;
    }

    //Draws a vertical line on the screen connecting the three points at x
    void connectPoints(UWORD x, UWORD y1, UWORD y2, UWORD y3){
        UWORD min = y1;
        UWORD max = y1;
        if(y2 < min)
            min = y2;
        else if(y2 > max)
            max = y2;
        if(y3 < min)
            min = y3;
        else if(y3 > max)
            max = y3;
        connectPoints(x, min, max);
    }

    //Draws a vertical line connecting the two points at x
    void connectPoints(UWORD x, UWORD y1, UWORD y2){
        int dir = y1 < y2 ? 1 : -1;

        Paint_SetPixel(x, y1, BLACK);

        if(y1 == y2){
            return;
        }

        y1 += dir;

        while(y1 != y2){
            Paint_SetPixel(x, y1, BLACK);
            y1 += dir;
        }
    }

    //Draws the specified function for each x on the screen
    void drawnFn(unsigned char fnToDraw){
        Args* args = &functions[fnToDraw];
        if(args->variableName.empty())//Signifies invalid/unset function
            return;
        
        std::map<std::string, double> tempVariables;
        tempVariables.insert(globalVariables.begin(), globalVariables.end());
        tempVariables[args->variableName] = xLeft;

        unsigned char workingY;
        UWORD eqvYCoord;
        double yDiff = yTop-yBottom;
        double stepSize = (xRight-xLeft)/SCREEN_WIDTH;
        UWORD leftYCoord; //Keep track of the height of the pixel to our left so we can join them, this is clamped

        UWORD xAxis;
        bool weShade = false;
        if(shadeIntegral && fnToDraw == whichFn){
            weShade = true;
            shadeIntegral = false;
            xAxis = storageToY(yToStorage(0.0));
        }

        for(UWORD i = 0; i < SCREEN_WIDTH; i++){
            if(dirty){
                workingY = yToStorage((Lepton::ParsedExpression::publicEvaluate(args->node, tempVariables)).getReal());
                graphBuffer[fnToDraw][i] = workingY;
            }
            else
                workingY = graphBuffer[fnToDraw][i];

            eqvYCoord = storageToY(workingY);

            if(weShade && i >= xLineLeft && i <= xLineRight){//Integral
                if(i != 0)
                    connectPoints(i, leftYCoord, eqvYCoord, xAxis);
                else
                    connectPoints(i, eqvYCoord, xAxis);
            }
            else{//Regular point
                if(workingY != TOO_HIGH && workingY != TOO_LOW){
                    if(i != 0)
                        connectPoints(i, leftYCoord, workingY);
                    else
                        Paint_SetPixel(i, workingY, BLACK);
                }
            }
            leftYCoord = eqvYCoord;
            tempVariables[args->variableName] += stepSize;
        }
    }

    //Moves the crosshair only if the move is valid
    void moveXhair(Direction dir){
        if(dir == LEFT && xXhair >= X_INTERVAL)
            xXhair -= X_INTERVAL;
        else if(dir == DOWN && yXhair < SCREEN_HEIGHT-Y_INTERVAL)
            yXhair += Y_INTERVAL;
        else if(dir == RIGHT && xXhair < SCREEN_WIDTH-X_INTERVAL)
            xXhair += X_INTERVAL;
        else if(dir == UP && yXhair >= Y_INTERVAL)
            yXhair -= Y_INTERVAL;
    }

    //Moves a bounding line only if the move is valid
    void moveLine(Direction dir, UWORD* which){
        if(dir == LEFT && *which >= X_INTERVAL)
            *which -= X_INTERVAL;
        else if(dir == RIGHT && *which < SCREEN_WIDTH-X_INTERVAL)
            *which += X_INTERVAL;
    }

    //Actually draws the lines if the current state permits
    void drawLines(){
        if(state == XHAIR)
            return;
        
        char buf[30]; //Hope this is enough!
        if(state == LEFT_LINE){
            Paint_DrawLine(xLineLeft, 0, xLineLeft, SCREEN_HEIGHT-1, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
            snprintf(buf, sizeof(buf), "x1 = %.5g", xToDouble(xLineLeft));
        }

        else if(state == RIGHT_LINE){
            Paint_DrawLine(xLineLeft, 0, xLineLeft, SCREEN_HEIGHT-1, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
            Paint_DrawLine(xLineRight, 0, xLineRight, SCREEN_HEIGHT-1, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
            snprintf(buf, sizeof(buf), "x2 = %.5g", xToDouble(xLineRight));
        }

        drawString(buf, true);
    }

    //Draws the label string in the bottom corner, only if we really mean it
    inline void drawString(char* buf, bool forReal){
        if(forReal)
            Paint_DrawString_EN(1, SCREEN_HEIGHT-13, buf, &Font12, BLACK, WHITE);
    }

    //Optionally update the crosshair x and y, or draw in current position
    void drawXhair(double x, double y, bool update){
        if(state != XHAIR)
            return;
        if(update){
            xXhair = (x-xLeft)/(xRight-xLeft) * SCREEN_WIDTH;
            yXhair = (yTop-y)/(yTop-yBottom) * SCREEN_HEIGHT;
        }
        else{
            x = xToDouble(xXhair);
            y = yToDouble(yXhair);
        }

        Paint_SetPixel(xXhair-1, yXhair, BLACK);
        Paint_SetPixel(xXhair+1, yXhair, BLACK);
        Paint_SetPixel(xXhair, yXhair-1, BLACK);
        Paint_SetPixel(xXhair, yXhair+1, BLACK);

        char buf[30]; //Hope this is enough!
        snprintf(buf, sizeof(buf), "(%.5g, %.5g)", x, y);
        drawString(buf, true);
    }

    //Redraws everything on the screen
    //Draw the marker at the specified location if update is true
    void drawGraphs(double x, double y, bool update, bool showString, char* str){
        drawAxis();
        drawnFn(0);
        drawnFn(1);
        drawnFn(2);
        drawnFn(3);
        dirty = false;
        if (!showString)
            drawXhair(x, y, update);
        drawLines();
        drawString(str, showString);
    }

    inline double xToDouble(UWORD x){
        return x*(xRight-xLeft)/SCREEN_WIDTH + xLeft;
    }

    inline double yToDouble(UWORD y){
        return yTop - y*(yTop-yBottom)/SCREEN_HEIGHT;
    }

public:
    //Redraws with new view
    void update(){
        drawGraphs(0.0, 0.0, false, false, NULL);
    }

    //General catch-all function handling the arrow keys
    void handleMovement(Direction dir){
        if(state == XHAIR)
            moveXhair(dir);
        else if(state == LEFT_LINE)
            moveLine(dir, &xLineLeft);
        else
            moveLine(dir, &xLineRight);
        
        drawGraphs(0.0, 0.0, false, false, NULL);
    }

    //Converts the strings to internal arg classes
    void drawGraphs(std::string strings[]){
        dirty = true;

        for(int i = 0; i < 4; i++){
            if(!strings[i].empty()){
                functions[i].node = parser->parse(strings[i]).getRootNode();
                functions[i].variableName = "x";
            }
        }
        drawGraphs((xRight-xLeft)/2+xLeft, (yTop-yBottom)/2+yBottom, true, false, NULL);
    }

    //Changes the state to allow entry of the markers, the two parameters indicate which index to perform on
    void beginFunction(GraphFunctions fn, unsigned int fn1, unsigned int fn2){
        function = fn;
        state = LEFT_LINE;
        whichFn = fn1;

        if(fn == INTERSECT){
            fnArg.node = Lepton::ExpressionTreeNode(new Lepton::Operation::Subtract(), functions[fn1].node, functions[fn2].node);
            fnArg.variableName = "x";
        }
        else
            fnArg = functions[fn1];
            

        drawGraphs(0.0, 0.0, false, false, NULL);
    }

    //Handles the enter click and redraws if needed
    void handleEnter(){
        if(state == XHAIR)
            return;
        
        double x = 0.0;
        double y = 0.0;
        bool shouldUpdate = false;
        bool showString = false;
        char buf[30];

        if(state == LEFT_LINE)
            state = RIGHT_LINE;
        else if(state == RIGHT_LINE){//Execute function
            state = XHAIR;
            
            //Need to get the arguments in order
            double argLeft = xToDouble(xLineLeft);
            double argRight = xToDouble(xLineRight);
            if(argRight < argLeft){
                double temp = argLeft;
                argLeft = argRight;
                argRight = temp;
            }
            
            try{
                if(function == MAX)
                    x = Lepton::getExtrema(fnArg, globalVariables, argLeft, argRight, true);
                else if(function == MIN)
                    x = Lepton::getExtrema(fnArg, globalVariables, argLeft, argRight, false);
                else if(function == ZERO || function == INTERSECT){ //The nodes were joined via subtraction previously
                    x = Lepton::getZero(fnArg, globalVariables, argLeft, argRight);
                    fnArg = functions[whichFn];//Next evaluation will be at the first function, not the difference function
                }
                else if(function == INTEGRAL){
                    Lepton::Operation::FnInt tempInt;
                    if (fnArg.inputs.size() == 0){
                        fnArg.inputs.push_back(argLeft);
                        fnArg.inputs.push_back(argRight);
                    }
                    else{
                        fnArg.inputs[0] = argLeft;
                        fnArg.inputs[1] = argRight;
                    }  
                    x = tempInt.evaluate(fnArg, globalVariables).getReal();
                }
            } catch(...){//TODO: figure out what went wrong
                x = nan(NULL);
            }
            
            if(std::isnan(x)){
                strcpy(buf, "NaN");
                showString = true;
            }
            else if(function == INTEGRAL){//Draw answer
                showString = true;
                snprintf(buf, sizeof(buf), "= %.5g", x);
                shadeIntegral = true;
            }
            else{//find y
                std::map<std::string, double> tempVariables;
                tempVariables.insert(globalVariables.begin(), globalVariables.end());
                tempVariables[fnArg.variableName] = x;
                y = (Lepton::ParsedExpression::publicEvaluate(fnArg.node, tempVariables)).getReal();
                shouldUpdate = true;
            }
        }

        drawGraphs(x, y, shouldUpdate, showString, buf);
    }
};