/***************************************************
 This library is designed for the Bluno product line and GoBLE iOS and android app from DFRobot.

 DFRobot Bluno product line: http://www.dfrobot.com/index.php?route=product/search&description=true&search=bluno
	—— First of its kind in intergrating BT 4.0(BLE) module into Arduino Uno, ideal prototyping platform for both software and hardware developers to go BLE.
 GoBLE iOS application :	 https://itunes.apple.com/us/app/goble-bluetooth-4.0-controller/id950937437?mt=8
	—— A universal Bluetooth remote controller

 ***************************************************
 Example introduction:
	Get the button state and the joystick on your phone.
	Open the Serail port and check the real time value via the Serial monitor.

 Created 2015-03-04
 By Lauren <lauren.pan@hotmail.com>


 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution
 ****************************************************/

/***********Notice and Trouble shooting***************

1. Highly recommend you to config the bluno controllers to work at 9600 baudrate before running the code!
	—— More details about how to config the baudrate: please check the link below：
		http://www.dfrobot.com/wiki/index.php/Bluno_SKU:DFR0267#Configure_the_BLE_through_AT_command
	Please use "AT+UART=9600" command!

	****************************************************/
#include "GoBLE.h"

GoBLE Goble(Serial);	// init the bluetooth Serial port
// Bluno default port - Serial

int joystickX, joystickY;
int buttonState[7];

const int ledPin   = 13;

void setup() {
  Goble.begin();		// init Goble and default baudrate is 9600 bps!!!
  pinMode(ledPin, OUTPUT);	// init LED
  digitalWrite(ledPin, LOW);
}

void loop() {

  // check if any new command from bluetooth communication
  // Note: available function must be used in the loop function to keep the serial data receiver running
  //	   Without this function, whole features will be invalid
  if (Goble.available()) {

    // read joystick value when there's valid command from bluetooth
    joystickX = Goble.readJoystickX();
    joystickY = Goble.readJoystickY();


    // read button state when there's valid command from bluetooth
    buttonState[SWITCH_UP]     = Goble.readSwitchUp();
    buttonState[SWITCH_DOWN]   = Goble.readSwitchDown();
    buttonState[SWITCH_LEFT]   = Goble.readSwitchLeft();
    buttonState[SWITCH_RIGHT]  = Goble.readSwitchRight();
    buttonState[SWITCH_SELECT] = Goble.readSwitchSelect();
    buttonState[SWITCH_START]  = Goble.readSwitchStart();

    // Debug bluetooth data received
    Serial.print("Joystick Value: ");
    Serial.print(joystickX);
    Serial.print("  ");
    Serial.print(joystickY);
    for (int i = 1; i <= 6; i++) {
      if (buttonState[i] == PRESSED) {
        Serial.print(" ID: ");
        Serial.print(i);
        Serial.print("\t ");
        Serial.print("Pressed!");
      }
    /*
      if (buttonState[i] == RELEASED){
        Serial.print("ID: ");
        Serial.print(i);
        Serial.print("\t ");
        Serial.println("Pressed!");Serial.println("Released!");
      }
    */
    }
    Serial.println("");
    
    // When 4 buttons of the control panel is pressed, illuminate the onboard LED of the Arduino!
    // Turn off the LED when none button pressed!
    if (buttonState[SWITCH_UP] == PRESSED || buttonState[SWITCH_DOWN] == PRESSED || buttonState[SWITCH_LEFT] == PRESSED || buttonState[SWITCH_RIGHT] == PRESSED) {
      digitalWrite(ledPin, HIGH);
    }
    else digitalWrite(ledPin, LOW);
  }
}

