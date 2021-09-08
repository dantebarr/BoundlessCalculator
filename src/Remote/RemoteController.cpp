#include "Features.h"
#include "sender.h"
#ifndef NO_BUTTONS
#include "RemoteKeyboardInputReceiver.h"
#endif
#include <ncurses.h>
#include <vector>
#include <string>
#include <set>

int LINES = 14;
int COLUMNS = 32;

u8 DURATION = 1;
u64 FEATURES_TO_DISABLE = 0;

std::set<std::string> featuresSet;
int curMins = 1;

#ifndef NO_BUTTONS
RemoteKeyboardInputReceiver remoteKeyboardInputReceiver = RemoteKeyboardInputReceiver();
#endif

int getCharacter(WINDOW* w) {
    #ifndef NO_BUTTONS
    wrefresh(w);
    return remoteKeyboardInputReceiver.getMenuKey();
    #else 
    return wgetch(w);
    #endif
}

void selectDuration() {
    int ch, i = 0;
    char minutes[6][5] = {"+10", "+5", "+1", "-1", "-5", "-10"};
    int listSize = static_cast<int>(sizeof(minutes)/sizeof(minutes)[0]);
    std::string curMinsText = "Current minutes is ";

    // line, column, begin_y, begin_x
    WINDOW *w = newwin(LINES, COLUMNS, 1, 1);
    box(w, 0, 0);
    noecho();
    keypad(w, TRUE);
    curs_set(0);

    for(i=0; i<listSize; i++) {
        if (i == 0)
            wattron(w, A_STANDOUT);
        else
            wattroff(w, A_STANDOUT);
        mvwprintw(w, i+1, 2, minutes[i]);
    }

    mvwprintw(w, i+2, 2,  (curMinsText + std::to_string(curMins) + "  ").c_str());
    mvwprintw(w, i+4, 2, "'<-' to go back");
    mvwprintw(w, i+5, 2, "'Enter' to +/- minutes");
    wrefresh(w);
    
    i = 0;
    ch = getCharacter(w);
    while(ch != KEY_LEFT) {
            mvwprintw(w, i+1, 2, "%s", minutes[i]);  
            switch(ch) {
                case 10: { // key enter
                    std::string x = std::string(minutes[i]);
                    int changeInMinutes = atoi(x.substr(0, x.length()).c_str());
                    if (curMins + changeInMinutes < 256 && curMins + changeInMinutes > 0)
                        curMins+=atoi(x.substr(0, x.length()).c_str());

                    mvwprintw(w, listSize+2, 2, (curMinsText + std::to_string(curMins) + "  ").c_str());
                    break;
                }
                case KEY_UP: {
                    i--;
                    i = (i < 0) ? listSize - 1 : i;
                    break;
                }
                case KEY_DOWN: {
                    i++;
                    i = (i > listSize - 1) ? 0 : i;
                    break;
                }
            }
           
            wattron(w, A_STANDOUT);
            mvwprintw(w, i+1, 2, "%s", minutes[i]);
            wattroff(w, A_STANDOUT);
            ch = getCharacter(w);
    }

    curs_set(1);
    delwin(w);
    DURATION = (unsigned char)curMins;
}


void selectFeaturesToDisable(std::vector<std::string> features) {
    int ch, i = 0;
    int listSize = features.size();
 
    WINDOW *w = newwin(LINES, COLUMNS, 1, 1);
    box(w, 0, 0);
    noecho();
    keypad(w, TRUE);
     
    // print menu items and hightlight the items that the user has already selected
    for(i=0; i<listSize; i++) {
        if(featuresSet.find(features[i]) != featuresSet.end()) 
            wattron(w, A_STANDOUT);
        else
            wattroff(w, A_STANDOUT);
        
        mvwprintw(w, i+1, 2, features[i].c_str());
    }

    wattroff(w, A_STANDOUT);
    mvwprintw(w, i+2, 2, "'<-' to go back");
    mvwprintw(w, i+3, 2, "'Enter' to select");
    wmove(w, 1, 1);
    wrefresh(w);
     
    i = 0;
    ch = getCharacter(w);
    while(ch != KEY_LEFT) { 
            // use a variable to increment or decrement the value based on the input.
            switch(ch) {
                case 10: // key enter
                    if (featuresSet.find(features[i]) == featuresSet.end()) {
                        // user wants to select the feature
                        featuresSet.insert(features[i]);
                        wattron(w, A_STANDOUT);
                    }
                    else {
                        // user wants to deselect the feature
                        featuresSet.erase(features[i]);
                        wattroff(w, A_STANDOUT);
                    }
                    mvwprintw(w, i+1, 2, features[i].c_str());
                    break;
                case KEY_UP:
                    i--;
                    i = (i < 0) ? listSize - 1 : i;
                    break;
                case KEY_DOWN:
                    i++;
                    i = (i > listSize - 1) ? 0 : i;
                    break;
            }
            wmove(w, i+1, 1);
            wrefresh(w);
            ch = getCharacter(w);
    }

    delwin(w);
    if (!featuresSet.empty()) {
        for (std::string feature : featuresSet) {
            if (featuresToBitMap.find(feature) != featuresToBitMap.end())
                FEATURES_TO_DISABLE |= featuresToBitMap.at(feature);
        }
    }
}

void paintMainWindow(WINDOW* mainWindow) {
    box(mainWindow, 0, 0);
    mvwprintw(mainWindow, 1, 2, "Choose features to disable");
    mvwprintw(mainWindow, 2, 2, "Choose disable duration");
    mvwprintw(mainWindow, 4, 2, "'Power Button' to exit");
    mvwprintw(mainWindow, 5, 2, "'->' to send");
    wmove(mainWindow, 1, 1);
    wrefresh(mainWindow);
}

void paintFeauturesByTopicsMenu(WINDOW* w) {
    box(w, 0, 0);
    mvwprintw(w, 1, 2, "Trigonometric functions");
    mvwprintw(w, 2, 2, "Operators");
    mvwprintw(w, 3, 2, "User variables and functions");
    mvwprintw(w, 4, 2, "Menu operations");
    mvwprintw(w, 6, 2, "'<-' to go back");
    mvwprintw(w, 7, 2, "'Enter' to select");
    wmove(w, 1, 1);
    wrefresh(w);
}

void feauturesByTopicsMenu() {
    WINDOW *w = newwin(LINES, COLUMNS, 1, 1);
    noecho();
    keypad(w, TRUE);
    paintFeauturesByTopicsMenu(w);
    int i = 0;    
    int ch = getCharacter(w);
    while (ch != KEY_LEFT) {
        // TO DO: optimize switch statement if later
        switch(ch) {
            case 10: // key enter
                switch (i) {
                    case 0: {
                        std::vector<std::string> features{"sin", "cos", "tan", "asin", "acos", "atan"};
                        selectFeaturesToDisable(features);
                        break;
                    }
                    case 1: {
                        std::vector<std::string> features{"boolean AND", "boolan OR", "boolean NOT", "addition +", "subtraction -", "division /", "power ^"};
                        selectFeaturesToDisable(features);;
                        break;
                    }
                    case 2: {
                        std::vector<std::string> features{"Predefined constants", "User variables", "User functions"};
                        selectFeaturesToDisable(features);
                        break;
                    }
                    case 3: {
                        std::vector<std::string> features{"MODE menu", "Graphing window", "Advanced trigonometry"};
                        selectFeaturesToDisable(features);
                        break;
                    }
                }
                refresh();
                paintFeauturesByTopicsMenu(w);
                break;
            case KEY_UP:
                i--;
                i = (i < 0) ? 3 : i;
                break;
            case KEY_DOWN:
                i++;
                i = (i > 3) ? 0 : i;
                break;
        }
        wmove(w, i+1, 1);
        ch = getCharacter(w);
    }

    delwin(w);
}

int main() {
    if(initSender() != 0) {
	    printf("Error initializing sender\n");
        return 1;
    }

    initscr();
    WINDOW *mainWindow = newwin(LINES, COLUMNS, 1, 1);
    noecho();
    keypad(mainWindow, TRUE);
    paintMainWindow(mainWindow);

    int i = 0;    
    int ch = getCharacter(mainWindow);
    while(ch != 'q' && ch != KEY_CLOSE) {
        if (ch == KEY_RIGHT) {
            sendMessage(DURATION, FEATURES_TO_DISABLE);
            mvwprintw(mainWindow, 7, 2, "Sent request to disable");
	    } else {
            	switch(ch) {
                    case 10: // key enter
                        switch (i) {
                            case 0:
                                //selectFeaturesToDisable();
                                feauturesByTopicsMenu();
                                break;
                            case 1:
                                selectDuration();
                                break;
                        }
                        refresh();
                        paintMainWindow(mainWindow);
                        break;
                    case KEY_UP:
                        i--;
                        i = (i < 0) ? 1 : i;
                        break;
                    case KEY_DOWN:
                        i++;
                        i = (i > 1) ? 0 : i;
                        break;
                }
            wmove(mainWindow, 7, 1);
            wclrtoeol(mainWindow);
            box(mainWindow, 0, 0);
            wmove(mainWindow, i+1, 1);
	    }
           
        wmove(mainWindow, i+1, 1);
        ch = getCharacter(mainWindow);
    }
    
    delwin(mainWindow);
    endwin();
}
