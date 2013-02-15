#include "menu.h"

HMENU menu::menuBar = NULL;

// constructor
menu::menu()
{

}


void menu::checkOption(int option)
{	
    // check one option in a group but uncheck the rest
    
    int group = option & 0xFF00;
    
    int curItem = group;
    
    
    if (group > 0) {
        // this isnt ideal, we are just unchecking 16 items in the group
        // checkMenuItem is supposed to return -1 if the item doesnt exist but im not sure if it does
        for (int curItem = group; curItem < group + 16; curItem++) {
            CheckMenuItem(menuBar,curItem,MF_UNCHECKED);   
       }
    }
    
    CheckMenuItem(menuBar,option,MF_CHECKED);   
}

void menu::uncheckOption(int option) 
{
    CheckMenuItem(menuBar,option,MF_UNCHECKED);
}
