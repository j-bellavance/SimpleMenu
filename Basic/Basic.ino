/*
 * Basic.ino
 * Author: Jacques Bellavance
 * Released: August 10, 2017
 * 
*/
//==========================================================================================SETUP THE MENU
#include "SimpleMenu.h"
char menuItems[] =
"-READ:000"
"--SENSORS:000"
"---SENSOR A0:101"
"---SENSOR A1:102"
"---SENSOR A2:103"
"---SENSOR A3:104"
"---SENSOR A6:105"
"---SENSOR A7:106"
"--SWITCHES:000"
"---SWITCH PIN 4:204"
"---SWITCH PIN 5:205"
"-SET:000"
"--SERVO ARM:300"
"--SERVO BASE:301"
"-MOVE SERVOS:400";
SimpleMenu menu(menuItems);     //Make it real
bool menuNeedsUpdate = true;    //Flag to update the display only if needed

//===========================================================================================SETUP THE LCD
#include <Wire.h>
#include <LiquidTWI.h>
#define LCD_LINES 4
#define LCD_COL 20
LiquidTWI lcd(0);

//The display routine=======================
void displayMenu() {
  lcd.clear();
  for (int i = 0 ; i < LCD_LINES ; i++) {
    lcd.setCursor(0, i);
    lcd.print(menu.getLine(i));
  }
  menuNeedsUpdate = false;
}//-----------------------------------------

//========================================================================================SETUP THE KEYPAD
#define pinUP 4
#define pinDOWN 3
#define pinLEFT 5
#define pinRIGHT 2

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

#include <EdgeDebounce.h>
EdgeDebounce btnUP(pinUP, PULLUP);
EdgeDebounce btnDOWN(pinDOWN, PULLUP);
EdgeDebounce btnLEFT(pinLEFT, PULLUP);
EdgeDebounce btnRIGHT(pinRIGHT, PULLUP);

//The keypad routine====================================================
int buttonPressed() {
  if (btnUP.fell())    return UP; 
  if (btnDOWN.fell())  return DOWN;
  if (btnLEFT.fell())  return LEFT; 
  if (btnRIGHT.fell()) return RIGHT;
  return 0;
}//---------------------------------------------------------------------

//=======================================================================================SETUP THE ACTIONS

//The action routine======================================
void make(int action) {
  switch (action) {
    case 101: {} // { your code; break; }
    case 102: {} // { your code; break; }
    case 103: {} // { your code; break; }
    case 104: {} // { your code; break; }
    case 105: {} // { your code; break; }
    case 106: {} // { your code; break; }
    case 204: {} // { your code; break; }
    case 205: {} // { your code; break; }
    case 300: {} // { your code; break; }
    case 301: {} // { your code; break; }
    case 400: {} // { your code; break; }
  }
}//-------------------------------------------------------

//===================================================================================================SETUP
void setup() {
  btnUP.begin();
  btnDOWN.begin();
  btnLEFT.begin();
  btnRIGHT.begin();
  Serial.begin(9600);                        //Start Serial
  lcd.begin(LCD_COL, LCD_LINES);             //Start LCD
  menu.setDisplayLines(LCD_LINES);           //Set the number of lines of the display
  menu.setKeypad(UP, DOWN, LEFT, RIGHT);     //Set the values of  the directional keypad
}

//====================================================================================================LOOP
void loop() {
  if (menuNeedsUpdate) {                 //If the menu needs to be updated
    displayMenu();                         //Update it
  }
  byte key = buttonPressed();           //Read the keypad
  if (key > 0) {                        //If a key was pressed
    int action = menu.getAction(key);     //Alert the Library of a keypress and get the action
    Serial.println(action);
    if (action > 0) {                       //If there is an action to undertake
      make(action);                         //Do it
    }  
    menuNeedsUpdate = true;               //Menu needs to be updated
  }
  //You can add more code here.
  //If your keypad gets to be unresponsive,
  //it is probably because you have too much stuff going on here.
  //buttonPress() should be called at least every 500ms to feel responsive.
}
