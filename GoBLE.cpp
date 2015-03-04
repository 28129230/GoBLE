/***************************************************
 This library is designed for the Bluno product line and GoBLE iOS and android app from DFRobot.
 
 DFRobot Bluno product line: http://www.dfrobot.com/index.php?route=product/search&description=true&search=bluno
	—— First of its kind in intergrating BT 4.0(BLE) module into Arduino Uno, ideal prototyping platform for both software and hardware developers to go BLE.
 GoBLE iOS application :	 https://itunes.apple.com/us/app/goble-bluetooth-4.0-controller/id950937437?mt=8
	—— A universal Bluetooth remote controller
 ***************************************************
 This library decode the GoBLE bluetooth communication protocol. And supplies the sample codes for the user to read the joystick value and 
 button state(button event) easily instead of handling with complicated protocol layer issues.
  
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
#include <Arduino.h>

/*
	Call Constructor
 */
 
GoBLE::GoBLE(HardwareSerial& theSerial):bleSerial(theSerial){
  
}

// Initialize Class Variables //////////////////////////////////////////////////
void (*GoBLE::user_switchUpCallback)(void);
void (*GoBLE::user_switchDownCallback)(void);
void (*GoBLE::user_switchLeftCallback)(void);
void (*GoBLE::user_switchRightCallback)(void);
void (*GoBLE::user_switchSelectCallback)(void);
void (*GoBLE::user_switchStartCallback)(void);

void GoBLE::begin() {
  
  debugSerial.begin(9600);	// config the default debug Serial port
  bleSerial.begin(9600);	// config the default baudrate of the Serial port
  initRecvDataPack();		// clear buffer
  
  // init variables
  _joystickX = 127;
  _joystickY = 127;
  for (int i = 0; i < MAXBUTTONID; i++) {
    _button[i] = RELEASED;
  }
  
  for (int i = 0; i < 20; i++) bleQueue.push(0x00);
  for (int i = 0; i < 20; i++) bleQueue.pop();
  
  
}


// User configuration for the baudrate of the BLE communication
void GoBLE::begin(unsigned int baudrate) {

  debugSerial.begin(9600);	// config the default debug Serial port
  bleSerial.begin(baudrate);
  initRecvDataPack();
  
  _joystickX = 127;
  _joystickY = 127;
  for (int i = 0; i < MAXBUTTONID; i++) {
    _button[i] = RELEASED;
  }
}

/*
  Function: Check if any new commands valid from BLE communication
	True 	- new command received
	False	- no new command
  */
  
boolean GoBLE::available() {
  /*
  function introduction:
  	* push the new valid data to the data buffer package
  	* throw away the invalid byte
  	* parse the data package when the command length is matching the protocol
  */

  if (bleSerial.available())  bleDataReceiver();

  // print the raw data package pushed to the queue.
  // It will disable the later functions!!! So please don't print it if it's not necessary for you
  if (DEBUGDATARAW) {
    debugSerial.println("GoBLE availalbe -> new data package!");
    for (int i = 0; i < rDataPack.commandLength; i++) {
      debugSerial.print(bleQueue.pop(), HEX);
    }
    debugSerial.println();
  }
  
  // print the data package length 
  if (DEBUGPARSER) {
    debugSerial.print("GoBLE availalbe -> bleQueue Counter: ");
    debugSerial.print(bleQueue.count());
    debugSerial.println();
  }

  // check if the length and command Flag is valid, then parse the new command package received
  if (rDataPack.commandFlag && bleQueue.count() == rDataPack.commandLength) {
    
    rDataPack.parseState = bleDataPackageParser();	// parse the command package
    
    if(rDataPack.parseState == PARSESUCCESS){		// if parse successfully 
      updateJoystickVal();  	// update the variables
      updateButtonState();		
	  
      // if press the button, trigger the user button event functions
      if(_prevButton[SWITCH_UP] == RELEASED     && _button[SWITCH_UP] == PRESSED && user_switchUpCallback)     user_switchUpCallback();		
      if(_prevButton[SWITCH_DOWN] == RELEASED   && _button[SWITCH_DOWN] == PRESSED && user_switchDownCallback)   user_switchDownCallback();
      if(_prevButton[SWITCH_LEFT] == RELEASED   && _button[SWITCH_LEFT] == PRESSED && user_switchLeftCallback)   user_switchLeftCallback();
      if(_prevButton[SWITCH_RIGHT] == RELEASED  && _button[SWITCH_RIGHT] == PRESSED && user_switchRightCallback)  user_switchRightCallback();
      if(_prevButton[SWITCH_SELECT] == RELEASED && _button[SWITCH_SELECT] == PRESSED && user_switchSelectCallback) user_switchSelectCallback();
      if(_prevButton[SWITCH_START] == RELEASED  && _button[SWITCH_START] == PRESSED && user_switchStartCallback)  user_switchStartCallback();
	  /*
      */
      return true;
    }
  }
  return false;
}


int GoBLE::readJoystickX() {
  return  _joystickX;
}
int GoBLE::readJoystickY() {
  return  _joystickY;
}

boolean GoBLE::readSwitchUp() {
  return _button[SWITCH_UP];
}

boolean GoBLE::readSwitchDown() {
  return _button[SWITCH_DOWN];
}

boolean GoBLE::readSwitchLeft() {
  return _button[SWITCH_LEFT];
}

boolean GoBLE::readSwitchRight() {
  return _button[SWITCH_RIGHT];
}

boolean GoBLE::readSwitchSelect() {
  return _button[SWITCH_SELECT];
}

boolean GoBLE::readSwitchStart() {
  return _button[SWITCH_START];
}



// sets function called on Switch UP pressed
void GoBLE::switchUpIsPressed(void (*function)(void)){
  user_switchUpCallback = function;  
}

void GoBLE::switchDownIsPressed(void (*function)(void)){
  user_switchDownCallback = function;  
}
void GoBLE::switchLeftIsPressed(void (*function)(void)){
  user_switchLeftCallback = function;  
}
void GoBLE::switchRightIsPressed(void (*function)(void)){
  user_switchRightCallback = function;  
}
void GoBLE::switchSelectIsPressed(void (*function)(void)){
  user_switchSelectCallback = function;  
}
void GoBLE::switchStartIsPressed(void (*function)(void)){
  user_switchStartCallback = function;  
}

// Private functions

int GoBLE::bleDataPackageParser() {
  /*
    0x10  - Parse success
    0x11  - Wrong header charactors
    0x12  - Wrong button number
    0x13  - Check Sum Error
  */
  byte calculateSum = 0;

  rDataPack.header1 = bleQueue.pop(), calculateSum +=  rDataPack.header1;	// get the data from queue
  rDataPack.header2 = bleQueue.pop(), calculateSum +=  rDataPack.header2;
  
  if(rDataPack.header1 != DEFAULTHEADER1)     return 0x11;					// check 1st and 2nd header byte
  if(rDataPack.header2 != DEFAULTHEADER2)     return 0x11;
  
  rDataPack.address = bleQueue.pop(), calculateSum +=  rDataPack.address;	// get address byte
  
  // count the command length based on the button command byte content
  rDataPack.latestDigitalButtonNumber = rDataPack.digitalButtonNumber;		
  rDataPack.digitalButtonNumber = bleQueue.pop(), calculateSum +=  rDataPack.digitalButtonNumber;

  int digitalButtonLength = rDataPack.digitalButtonNumber;
  
  // check if the length is valid
  if (DEBUGCHECKSUM) {
    debugSerial.print("Parser -> digitalButtonLength: ");
    debugSerial.println(digitalButtonLength);
  }
  if(digitalButtonLength > MAXBUTTONNUMBER)   return 0x12;
  
  // get the raw joystick value
  rDataPack.joystickPosition = bleQueue.pop(), calculateSum +=  rDataPack.joystickPosition;

  // read button data package - dynamic button payload length
  for (int buttonPayloadPointer = 0; buttonPayloadPointer < digitalButtonLength; buttonPayloadPointer++) {
    rDataPack.buttonPayload[buttonPayloadPointer] = bleQueue.pop();
    calculateSum +=  rDataPack.buttonPayload[buttonPayloadPointer];
  }
  // read 4 byte joystick data package
  for (int i = 0; i < 4; i++)  rDataPack.joystickPayload[i] = bleQueue.pop(), calculateSum +=  rDataPack.joystickPayload[i];

  rDataPack.checkSum = bleQueue.pop();			// get the check sum byte

  if (DEBUGCHECKSUM) {
    debugSerial.print("Parser -> sum calculation: ");
    debugSerial.println(calculateSum);

    debugSerial.print("Parser -> checkSum byte value: ");
    debugSerial.println(rDataPack.checkSum);
  }

  // check sum and update the parse state value
  // if the checksum byte is not correct, return 0x12
  rDataPack.commandFlag = false;				// clear the command flag to finish the parse step and wait for the next data package
  if (rDataPack.checkSum == calculateSum)     return PARSESUCCESS;
  else                                        return 0x13;
}

void  GoBLE::bleDataReceiver() {

  byte inputByte = bleSerial.read();

  if (DEBUGDATARECEIVER) {
    debugSerial.print("bleDataReceiver -> new data:");
    debugSerial.println(inputByte, HEX);
  }

  // throw the trash data and restore the useful data to the queue buffer
  if (inputByte == DEFAULTHEADER1 || rDataPack.commandFlag == true) {
    bleQueue.push(inputByte);
    rDataPack.commandFlag = true;

    // auto adjust the command length based on the button command value
    if (bleQueue.count() == PACKBUTTONSIGN) {
      // max button input at one moment should less than 6 buttons
      if (inputByte > 0 && inputByte < MAXBUTTONNUMBER) {

        // default command length + button number
        rDataPack.commandLength = DEFAULTPACKLENGTH + inputByte;
        if (DEBUGDATARECEIVER)	debugSerial.print("bleDataReceiver -> Command Length:"), debugSerial.println(rDataPack.commandLength);
      }
      else	rDataPack.commandLength = DEFAULTPACKLENGTH;
    }
  }

}

// init the variables
void  GoBLE::initRecvDataPack() {
  rDataPack.commandFlag         = false;
  rDataPack.commandLength       = DEFAULTPACKLENGTH;
  rDataPack.parseState          = PARSESUCCESS;
  
  rDataPack.digitalButtonNumber = 0;
  rDataPack.latestDigitalButtonNumber = 0;
}

void GoBLE::updateJoystickVal(){
  _joystickX = rDataPack.joystickPayload[0];
  _joystickY = rDataPack.joystickPayload[1];
}

void GoBLE::updateButtonState(){
  
  for (int i = 0; i < MAXBUTTONID; i++) {
    _prevButton[i] = _button[i];
  }
  
  if (rDataPack.digitalButtonNumber == 0 && rDataPack.latestDigitalButtonNumber != 0) {
    for (int i = 0; i < MAXBUTTONID; i++) {
      if (_button[i] == PRESSED) {
        if (DEBUGUPDATEBUTTON) {
          debugSerial.print("updateButtonState -> clear Pressed button number: ");
          debugSerial.println(i);
        }
        _button[i] = RELEASED;
      }
    }
  }

  for (int i = 0; i < rDataPack.digitalButtonNumber; i++)   _button[rDataPack.buttonPayload[i]] = PRESSED;
  
  if (DEBUGUPDATEBUTTON) {
    for (int i = 0; i < MAXBUTTONID; i++) {
      debugSerial.print("updateButtonState -> prevButton ID: ");
      debugSerial.print(i);
      debugSerial.print(" val: ");
      if(_prevButton[i] == PRESSED)  debugSerial.println("PRESSED");
      if(_prevButton[i] == RELEASED)  debugSerial.println("RELEASED");
      debugSerial.print("updateButtonState -> Button ID: ");
      debugSerial.print(i);
      debugSerial.print(" val: ");
      if(_button[i] == PRESSED)  debugSerial.println("PRESSED");
      if(_button[i] == RELEASED)  debugSerial.println("RELEASED");
    }  
  }
}

