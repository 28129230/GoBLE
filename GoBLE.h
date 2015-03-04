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
 

#ifndef GOBLE_H_
#define GOBLE_H_

#include <Arduino.h>
#include "QueueArray.h"	

/**************** Debugger Configuration of the library decoding protocol ******************/

#define DEBUGDATARECEIVER	false
#define DEBUGDATARAW     	false
#define DEBUGPARSER      	false
#define DEBUGCHECKSUM       false
#define DEBUGUPDATEBUTTON	false

#define debugSerial			Serial

/**************** Switch value definition compatible with Arduino Esplora setting******************/

const byte SWITCH_1       = 1;
const byte SWITCH_2       = 2;
const byte SWITCH_3       = 3;
const byte SWITCH_4       = 4;
const byte SWITCH_5       = 5;
const byte SWITCH_6       = 6;

const byte SWITCH_UP    	= SWITCH_1;
const byte SWITCH_RIGHT 	= SWITCH_2;
const byte SWITCH_DOWN  	= SWITCH_3;
const byte SWITCH_LEFT 		= SWITCH_4;

const byte SWITCH_SELECT 	= SWITCH_5;
const byte SWITCH_START 	= SWITCH_6;

/*
 * These constants can be use for comparison with the value returned
 * by the readButton() method.
 */
const boolean PRESSED   = LOW;
const boolean RELEASED  = HIGH;



/**************** Data structure for the command buffer ******************/

// Package protocol configuration
#define PACKHEADER	        1
#define PACKHEADER2			2
#define PACKADDRESS			3
#define PACKBUTTONSIGN		4
#define PACKJOYSTICKSIGN	5
#define PACKPAYLOAD         6


#define DEFAULTHEADER1      0x55
#define DEFAULTHEADER2      0xAA
#define DEFAULTADDRESS      0x11
#define DEFAULTPACKLENGTH	10

#define MAXBUTTONNUMBER     6 
#define MAXBUTTONID         7

#define PARSESUCCESS        0x10
 
//Data Link package
#pragma pack(1)
typedef struct
{
  byte  header1;          // 0x55
  byte  header2;          // 0xAA
  byte  address;          // 0x11
  
  byte  latestDigitalButtonNumber;
  byte  digitalButtonNumber;
  
  byte  joystickPosition;
  byte	buttonPayload[MAXBUTTONNUMBER];
  byte	joystickPayload[4];
  byte  checkSum;

  byte  commandLength;
  byte  parseState;
  boolean commandFlag;
} sDataLink;
#pragma pack()


/**************** Main class ******************/


class GoBLE {
    
public:  

  // Constructor: the Serial port BLE connected
  // Default setting for Bluno using Serial
  GoBLE(HardwareSerial& theSerial);	
  
  void begin();
  void begin(unsigned int baudrate);	// init the baudrate of the bluetooth
  
  boolean available();	// check the valid command package

  int readJoystickX();	// read the joystick value from command package
  int readJoystickY();
  
  /*
   * Reads the current state of a button. It will return
   * LOW if the button is pressed, and HIGH otherwise.
   */
  boolean readSwitchUp();
  boolean readSwitchDown();
  boolean readSwitchLeft();
  boolean readSwitchRight();
  boolean readSwitchSelect();
  boolean readSwitchStart();
  
  /*
   * Call back functions for switchs pressed event
   */
  void switchUpIsPressed(void (*)(void) );
  void switchDownIsPressed(void (*)(void) );
  void switchLeftIsPressed(void (*)(void) );
  void switchRightIsPressed(void (*)(void) );
  void switchSelectIsPressed(void (*)(void) );
  void switchStartIsPressed(void (*)(void) );
  
private:

  HardwareSerial& bleSerial;
  sDataLink rDataPack;
  // create a queue of characters for handling with the bluetooth Serial data
  QueueArray <byte> bleQueue;
  
  int _joystickX,_joystickY;	// joystick raw data
  int _button[MAXBUTTONID];		// button value from application
  int _prevButton[MAXBUTTONID];	// previous button value

  void updateJoystickVal();		// update variables 
  void updateButtonState();

  void initRecvDataPack();		
  int bleDataPackageParser();
  void bleDataReceiver();		// read Serial data and push them to the queue

  static void (*user_switchUpCallback)(void);
  static void (*user_switchDownCallback)(void);
  static void (*user_switchLeftCallback)(void);
  static void (*user_switchRightCallback)(void);
  static void (*user_switchSelectCallback)(void);
  static void (*user_switchStartCallback)(void);
  
};

#endif // GOBLE_H_
