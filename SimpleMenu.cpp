#include "Arduino.h"
#include "SimpleMenu.h"

//SimpleMenu (Constructor)========================================================
//Find the number of items in the menu (count the colons)
//And allocate memory for the nodes
//nodes[0] is the parent of the root menu items
//Initiates the parsing of the menu items
//--------------------------------------------------------------------------------
SimpleMenu::SimpleMenu(char* items) {
  MYitems = items;
  int count = 0;
  int ptr = 0;
  while (MYitems[ptr] != '\0') { if (MYitems[ptr] == ':') { count++; } ptr++; }
  nodes = (node*) calloc(count + 1, sizeof(node));
  nodes[0].eldest = 1;
  menuParse();
}//SimpleMenu---------------------------------------------------------------------

//setDisplayLines========================================
//Initialises the number of lines of the display
//Defaults to 2
//-------------------------------------------------------
void SimpleMenu::setDisplayLines(byte displayLines) {
  MYdisplayLines = displayLines;
}//setDisplayLines---------------------------------------

//setKeyboard==============================================================
//Sets the values that will be sent by the sketch for the 4 keys
//-------------------------------------------------------------------------
void SimpleMenu::setKeypad(byte UP, byte DOWN, byte LEFT, byte RIGHT) {
  MYup = UP;
  MYdown = DOWN;
  MYleft = LEFT;
  MYright = RIGHT;
}//setKeyboard-------------------------------------------------------------

//menuParse Abstract===========================================================================================================
//The full menu is built in the sketch in the following mannner :
//  char[] menuItems = 
//  "-READ:000"
//  "--SENSORS:000"
//  "---SENSOR A1:101"
//  "---SENSOR A2:102"
//  "--SWITCHES:000"
//  "---SWITCH PIN 4:103"
//  "---SWITCH PIN 5:104"
//  "-SET:000"
//  "--SERVO ARM:105"
//  "--SERVO BASE:106"
//  "-MOVE SERVOS:107";
//The menu items contains 4 parts:
//  a series of dashes that indicate the item's level
//  the label of the item
//  a colon (:) (token)
//  a 3 digits number between "000" and "999" to tag an action to be performed in the sketch ("000" means: I have a submenu)
//
//The goal is to populate for each item in the menu a node containing:
//  A pointer to the start of the item's label
//  The action appointed to the item
//  The parent of the item (Using a LIFO stack)
//  The eldest child of this item
//  The next sibling of this item
//
//menuItems (who's pointer is now MYitems) is modified so that each menu's item label:
//  Has a NULL character added after it to give it its own identity (lcd.print() knows when to stop)
//  Has a space added before it to precede it with a carret if it is the active menu item
//-----------------------------------------------------------------------------------------------------------------------------

//actionToInt====================================
//Translates 3 chars to an integer
//-----------------------------------------------
int SimpleMenu::actionToInt(int pos) {
  return ((int(MYitems[pos + 1]) - 48) * 100) +
         ((int(MYitems[pos + 2]) - 48) * 10)  +
         ((int(MYitems[pos + 3]) - 48));
}//actionToInt-----------------------------------

//menuParse=============================================================================================================================
//See above
//--------------------------------------------------------------------------------------------------------------------------------------
void SimpleMenu::menuParse() {
  byte parentStack[8] = { 0,0,0,0,0,0,0,0 }; //Stack (Last-in/First-out) (parent management)
  byte parentPtr = 0;                        //Pointer to the beginning of the stack
  int pos = 1;                               //The position within MYitems (Start after the mandatory '-')
  byte item = 1;                             //Pointer to the current item
  byte curLevel = 1;                         //Level of the current item
  byte nextLevel = 1;                        //Level of the next item

  MYitems[0] = ' ';                                                //Leave a place for the carret
  while(MYitems[pos] != '\0') {                                    //Parse the whole menu
    nodes[item].next = item;                                         //I am my own next sibling (unless another one gets born later)
    nodes[item].starts = MYitems + pos - 1;                          //The start of the label (with carret space)
    while(MYitems[pos] != ':') pos++;                                //Forward to the ":" token
    MYitems[pos] = '\0';                                             //Replace token with NULL to print only this item's label
    nodes[item].action = actionToInt(pos);                           //The integer associated to the action
    nodes[item].parent = parentStack[parentPtr];                     //The parent of the item (is the current on the stack)
    pos += 4;                                                        //Forward to the next item
 
    if (MYitems[pos] != '\0') {                                      //Set nodes[] only if there is another item
      nextLevel = 0 ;
      while(MYitems[pos] == '-') { pos++; nextLevel++; }               //Find the level of the next item (count dashes)

      if (nextLevel == curLevel) {                                     //If the next item is on the same level
        nodes[item].next = item + 1;                                     //It is the next sibling
      }
      if (nextLevel > curLevel) {                                      //If the next item has a higher level (item is a parent)
        parentPtr++;                                                     //Update the stack pointer
        parentStack[parentPtr] = item;                                   //Push the item as a parent on the stack
        nodes[item].eldest = item + 1;                                   //The next item is the eldest child of the current item
      }
      if (nextLevel < curLevel) {                                       //If the next item has a lower level
        byte levelDif = curLevel - nextLevel - 1;                        //Calculate the offset of the next item's parent
        nodes[parentStack[parentPtr - levelDif]].next = item + 1;        //Set the parent's next sibling to next item
        for (byte i = nextLevel; i < curLevel; i++) {                    //For the number of generations 
          parentStack[parentPtr] = 0;  parentPtr--;                        //Pull as many parents off the stack as needed
        }
      }
      MYitems[pos - 1] = ' ';                                         //Leave a space for the carret (replace the last dash with a space)
      item++; curLevel = nextLevel;                                   //Go to next item
    }
    else nodes[0].starts = MYitems[pos];                            //Ancester has no label
  }
  currentItem = 1;    //First item of main menu is the current Item
  MYitems[0]= '>';    //Place a carret before it's label
}//menuParse------------------------------------------------------------------------------------------------------------------------------


//fillSiblings===============================================================================================
//Will be used to display the menu
//Returns the index of the line where is the current item
//-----------------------------------------------------------------------------------------------------------
byte SimpleMenu::fillSiblings(byte self) {
  byte index;
  siblings[0] = nodes[nodes[self].parent].eldest;          //Start with the eldest of my parent
  if (siblings[0] == currentItem) index = 0;               //If the eldest is the currentItem index it
  byte i = 0;                                              //Initialise the pointer   
  while(nodes[siblings[i]].next != siblings[i]) {          //As long as there is a younger sibling
    i++;                                                     //Increase pointer
    siblings[i] = nodes[siblings[i-1]].next;                 //Add sibling to the list.
    if (siblings[i] == currentItem) index = i;               //If this sibling is the currentItem, index it
  }
  if (i < 15) {                                            //If there are more siblings[] left,
    for (byte j = i + 1 ; j < 16 ; j++) siblings[j] = 0;     //Fill the rest of siblings[] with zeroes
  }
  return index;
}//fillSiblings----------------------------------------------------------------------------------------------

//getLine========================================================================================================
//Returns a pointer to the item's label
//---------------------------------------------------------------------------------------------------------------
char* SimpleMenu::getLine(byte line) {
  byte currentItemIndex = fillSiblings(currentItem);   //Prepare siblings[] and get the current item's index
  if (currentItemIndex >= MYdisplayLines)              //If the item is off the display
    line += currentItemIndex - MYdisplayLines + 1;       //Scroll to make current item display on the last line
  return nodes[siblings[line]].starts;                 //Return the pointer to the sketch
}//getLine-------------------------------------------------------------------------------------------------------

//elder================================================================================================================
//Returns the sibling just before the current item
//It is the only parental relationship that is calculated. All the other ones are in nodes[]
//---------------------------------------------------------------------------------------------------------------------
byte SimpleMenu::older(byte self) {
  byte ptr = nodes[nodes[self].parent].eldest;            //Start with the eldest of my parent (My oldest sibling)
  while(nodes[ptr].next != self) ptr = nodes[ptr].next;   //As long as the next sibling is not me, go to next sibling
  return ptr;
}//elder---------------------------------------------------------------------------------------------------------------

//getAction==================================================================================
//Move the current item according to the key that was passed by the sketch
//Handle the carret placement in MYitems
//Return the current item's action (0:branches, 1..999 :leaves)
//If the current item points to a leaf and MYright is pressed, send its action
//-------------------------------------------------------------------------------------------
int SimpleMenu::getAction(byte key) {
  if (nodes[currentItem].action > 0 && key == MYright)             //On a leaf and RIGHT
    return nodes[currentItem].action;                                //Return action

  *nodes[currentItem].starts = ' ';                                //Clear carret 
  if (key == MYup) currentItem = older(currentItem);               //UP: Older sibling
  if (key == MYdown) currentItem = nodes[currentItem].next;        //DOWN: Younger sibling
  if (key == MYleft) currentItem = nodes[currentItem].parent;      //LEFT: Parent
  if (key == MYright) currentItem = nodes[currentItem].eldest;     //RIGHT: Eldest child

  *nodes[currentItem].starts = '>';                                //Set carret

   return 0;                                                       //No action
}//getAction---------------------------------------------------------------------------------

