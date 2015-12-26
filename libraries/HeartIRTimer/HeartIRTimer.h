/*
 * HeartIRTimer.h
 * 
 * A simple library that allows IR sending and receiving for a game
 *
 * HeartIRRec is a stand-alone receiver
 * HeartIRSend is a stand-alone tramsitter(sends on dig pin 9)
 * HeartIR contains both send and receive functionality, though beware that there is a 
 *	short (<10ms) lag when switching from sending to receiving
 * decode_results is a helper class used in decoding, by HeartIRRec
 *
 * !!!IR led must be connected to transmit from digital pin 9!!!
 *
 * !!!Uses Timer 1 and disables PWM on pins 9 and 10!!!
 * 
 * Timer and pin can be changed by modifying irRemoteInt.h
 *
 *
 * Adapted from Ken Shirriff's IRremote Library
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * For details, see http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.htm http://arcfn.com
*/

#ifndef HEART_IR_TIMER_H
#define HEART_IR_TIMER_H

#include "Arduino.h"

#include "../HeartMessageEnums/HeartMessages.h"

// Some useful constants
#define USECPERTICK 50  // microseconds per clock interrupt tick
#define RAWBUF 100 // Length of raw duration buffer

// Marks tend to be 100us too long, and spaces 100us too short
// when received due to sensor lag.
#define MARK_EXCESS 100

// Values for decode_type
#define SONY 1
#define SANYO 2
#define UNKNOWN -1

// Decoded value for NEC when a repeat code is received
#define REPEAT 0xffffffff

//number of bits to use for message encoding -- taken from Ken's example code
const int NUM_BITS = 12;

//There seems to be a need for a delay after a send for it to transmit successfully
const int POST_SEND_DELAY = 0;

class HeartIRSend {
public:
  
	//Sends data using the sony protocol
	void send(unsigned long data);
  
private:
	void enableIROut(int khz);
	void mark(int usec);
	void space(int usec);
};

// Results returned from the decoder
class decode_results {
public:
	int decode_type; // NEC, SONY, RC5, UNKNOWN
	unsigned int panasonicAddress; // This is only used for decoding Panasonic data
	unsigned long value; // Decoded value
	int bits; // Number of bits in decoded value
	volatile unsigned int *rawbuf; // Raw intervals in .5 us ticks
	int rawlen; // Number of records in rawbuf.
};

// main class for receiving IR
class HeartIRRec
{
public:
	
	/**
		Creates the receiver without initializing receivePin, etc
		requires a call to init() to function
		Only created for external HeartIRRec creation through the HeartIR class
	*/
	HeartIRRec(){}
	
	//Used to initialize a null receiver -- used by HeartIR class
	void init(int recvPin);
	
	HeartIRRec(int recvpin);
	
	//call every loop to check for received messages and update message received
	void update();

	//returns true if message received this update cycle
	bool gotNewMessage();
	
	//returns the last message received
	long getLastMessage();
		
	//!!!MUST BE CALLED TO INITIALIZE RECEIVER!!! 
	//enable the receiver
	void enable();
  
private:

	void blink13(int blinkflag);
	
	//decode a message using a decode_results
	int decode(decode_results *results);
		
	//resume receiving messages
	void resume();
	
	//the results decoder
	decode_results results;
	
	//decode a sony message
	long decodeSony(decode_results *results);
	
	//true if got a message this update cycle
	bool gotMessage;
	
	//last message received
	long lastMessage;
};

//contains a send and receive if you need to have both in one file
//hard-wired to use pin 10 for receiving IR, sends on 9
class HeartIRTimer {
public:

	HeartIRTimer();
		
	//Sends data using the sony protocol 
	void send(IRMessage data);
	
	/**
		check for receiver messages -- returns true if a new message has been received since the last check
		if you have been sending, will reinitialize the receiver
		!!!If you run this in the same loop as a send, may cause some lag/jitter !!!
		This still needs some testing
	*/
	bool checkMessages();
		
	//returns the last message received
	long getLastMessage();
	
private:
	bool sending; //Is the HeartIR in sending mode (disables receiver)
	HeartIRRec r; //the receiver
	HeartIRSend s; //the transmitter
};

#endif
