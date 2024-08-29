/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad_I2C.h>
#include <Keypad.h>

// LCD configuration
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

// Keypad configuration
#define I2CADDR 0x20
const byte ROWS = 4; // four rows
const byte COLS = 3; // three columns
String inputNum = "";
// Define the keymap for the 3x4 keypad
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

// Define pin numbers for rows and columns
int PIN_1 = 1;
int PIN_2 = 2;
int PIN_3 = 3;
int PIN_4 = 4;
int PIN_5 = 5;
int PIN_6 = 6;
int PIN_7 = 7;

byte rowPins[ROWS] = {PIN_2, PIN_7, PIN_6, PIN_4}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {PIN_3, PIN_1, PIN_5};        // Connect to the column pinouts of the keypad

// Initialize an instance of class Keypad_I2C
Keypad_I2C customKeypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR);

void setupKeypad(){
  // Serial.begin(115200);  // Initialising Serial Monitor
  // Initialize LCD
  lcd.init();
  lcd.backlight();  
  // Initialize Keypad
  Wire.begin();
  customKeypad.begin();
}



void display(String s) {
  lcd.clear();
  lcd.setCursor(0, 0);  
  lcd.print(s);
}

void loopKeypad() {
  char customKey = customKeypad.getKey();
  
  if (customKey != NO_KEY) {
    Serial.println(customKey); // Print key press to Serial Monitor
    
    // Check if the '#' key is pressed
    if (customKey == '#') {
      display("Hello, World!"); // Display "Hello, World!" on the LCD
      delay(2000);              // Wait for 2 seconds
      lcd.clear();              // Clear the LCD after the message
      inputNum = "";
    } else {
      inputNum +=customKey;
      display(String(inputNum));  // Display the pressed key on the LCD
    }
  }
}
