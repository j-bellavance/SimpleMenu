#ifndef SimpleMenu_h
#define SimpleMenu_h

#include "Arduino.h"

class SimpleMenu {

  public:
  //Methods
    SimpleMenu(char* items);                                       //Constructor
    void setDisplayLines(byte displayLines);                       //Obtain the number of lines of the display (defaults to 2)
    void setKeypad(byte UP, byte DOWN, byte LEFT, byte RIGHT);     //Obtain the codes for the four directional keys
    char* getLine(byte line);                                      //Returns the item to be displayed at "line"
    int getAction(byte key);                                       //Returns the action of the new item (after key press)
    byte getCurrentItem();                                         //Returns the id of the current item

  //Attributes
    //None
    
  private:
  //Methods:
    int actionToInt(int pos);      //Returns the integer value of the action (3 digit number in a char[])
    void menuParse();              //Parses the items in the menu
    byte fillSiblings(byte self);  //Prepares the siblings[] table to display the submenu that contains "self"
    byte older(byte self);         //Returns the sibling just before "self"

  //Attributes:
    struct node {             //For each item :
      char* starts = 0;         //the pointer to the start of the label in MYitems
      byte parent = 0;          //the node number of the parent of this item
      byte eldest = 0;          //the node number of the eldest child of this item
      byte next = 1;            //the node number of the next sibling of this item
      int action = 0;           //the action associated to this item
    };  
    node *nodes;              //Pointer to the table that holds the node's elements

    char* MYitems;            //Pointer to the items
    byte siblings[16];        //Table to hold the items to display
    byte currentItem = 1;     //Pointer to the current item
    byte MYdisplayLines = 2;  //Number of lines of the display
    byte MYup;                //Id of the UP key
    byte MYdown;              //Id of the DOWN key
    byte MYleft;              //Id of the LEFT key
    byte MYright;             //Id of the RIGHT key
};

#endif

