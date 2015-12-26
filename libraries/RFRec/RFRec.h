/**
RFRec.h
Larry LeBron

A simple library for an RF receiver to receive messages

Messages take the form of [senderId][Message][MessageID]

MessageID is just a count, which will give a unique value each time

Probably won't play nicely with an RFSend in the same program

Uses Timer1, which disables pwm on 9 & 10

A wrapper for Mike McCauley's Virtual Wire
http://www.open.com.au/mikem/arduino/VirtualWire.pdf
*/

#ifndef RF_REC_H
#define RF_REC_H

#include "Arduino.h"
#include "VirtualWire.h"
#include "../GameMessageEnums/GameMessages.h"

//default used in VirtualWire
const uint16_t BITS_PER_SECOND = 2000;

//only sending 3 byte messages for this application
const int BYTES_PER_MESSAGE = 3;

class RFRec {
public:

	/**
		Initializes the RFRec
		recPin: the pin for the antenna
	*/
	RFRec(uint8_t recPin);
	
	//Must be called during setup() in order for RFRec to function
	void init();
	
	/**
		check for receiver messages -- returns true if a new message has been received since the last check
		loads that message into msg
	*/
	bool checkMessages();
	
	//returns the last message received in the form of a uint_t[BYTES_PER_MESSAGE]
	uint8_t* getLastMessage();
	
private:
	uint8_t pin; //the receiver
	uint8_t msgLength; //the message length
	uint8_t msg[BYTES_PER_MESSAGE]; //contains the message received
	
	//contains a copy of the last message received for public use -- accessible through getLastMessage
	uint8_t msgCopy[BYTES_PER_MESSAGE];
};

#endif
