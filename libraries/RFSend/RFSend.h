/**
RFSend.h
Larry LeBron

A simple library for an RF antenna to transmit 3 byte messages

Messages take the form of [senderId][Message][MessageID]

MessageID is just a count, which will give a unique value each time

Probably won't play nicely with an RFRec in the same program

Uses Timer1, which disables pwm on 9 & 10

A wrapper for Mike McCauley's Virtual Wire
http://www.open.com.au/mikem/arduino/VirtualWire.pdf
*/

#ifndef RF_SEND_H
#define RF_SEND_H

#include "Arduino.h"
#include "VirtualWire.h"
#include "../GameMessageEnums/GameMessages.h"

//default used in VirtualWire
const uint16_t BITS_PER_SECOND = 2000;

//only sending 3 byte messages for this application
const int BYTES_PER_MESSAGE = 3;

class RFSend {
public:

	/**
		Initializes the RFSend
		sendPin: the pin for the antenna
		sID: the sender ID
	*/
	RFSend(uint8_t sendPin, uint8_t sendID);
	
	//Must be called during setup() in order for RFSend to function
	void init();
	
	//Send a message a number of times (defaults to 1)
	void send(RFMessage m, int numSends=1);
	
private:
	uint8_t pin; //the transmitter pin
	uint8_t msgCount;//number of messages sent -- used for unique identification - wraps around after 255
	uint8_t msg[BYTES_PER_MESSAGE];//the message to transmit
};

#endif
