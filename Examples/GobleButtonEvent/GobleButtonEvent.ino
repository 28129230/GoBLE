/***************************************************
 This library is designed for the Bluno product line and GoBLE iOS and android app from DFRobot.
 
 DFRobot Bluno product line: http://www.dfrobot.com/index.php?route=product/search&description=true&search=bluno
	—— First of its kind in intergrating BT 4.0(BLE) module into Arduino Uno, ideal prototyping platform for both software and hardware developers to go BLE.
 GoBLE iOS application :	 https://itunes.apple.com/us/app/goble-bluetooth-4.0-controller/id950937437?mt=8
	—— A universal Bluetooth remote controller
 
 ***************************************************
 Example introduction:
	This example tells how to use the GoBLE button event when the user press the button on the phone. 
	And custom your event function
	
	Every time you press the button on the phone, it will change the state of the LED on your Arduino main board.
	
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

const int ledPin   = 13;
boolean ledState   = LOW;

void setup(){
  Goble.begin();		// init Goble
  
  pinMode(ledPin,OUTPUT);	// init LED
  digitalWrite(ledPin,LOW);
  
  Goble.switchUpIsPressed(switchUpEvent);  			// call the custom switch event function
  Goble.switchDownIsPressed(switchDownEvent);
  Goble.switchLeftIsPressed(switchLeftEvent);
  Goble.switchRightIsPressed(switchRightEvent);
  Goble.switchSelectIsPressed(switchSelectEvent);
  Goble.switchStartIsPressed(switchStartEvent);
}

void loop() {
  
  // check if any new command from bluetooth communication
  // Note: available function must be used in the loop function to keep the serial data receiver running
  //	   Without this function, whole features will be invalid
  if(Goble.available()){
    joystickX = Goble.readJoystickX();
    joystickY = Goble.readJoystickY();
  }
  
  digitalWrite(ledPin,ledState);	// write the latest status of the LED
}

void switchUpEvent(){
  ledState = !ledState;
  Serial.println("Trigger Switch Up Event!");
}
void switchDownEvent(){
  ledState = !ledState;
  Serial.println("Trigger Switch Down Event!");
}
void switchLeftEvent(){
  ledState = !ledState;
  Serial.println("Trigger Switch Left Event!");
}
void switchRightEvent(){
  ledState = !ledState;
  Serial.println("Trigger Switch Right Event!");
}
void switchSelectEvent(){
  ledState = !ledState;
  Serial.println("Trigger Switch Select Event!");
}
void switchStartEvent(){
  ledState = !ledState;
  Serial.println("Trigger Switch Start Event!");
}
