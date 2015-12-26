/*
 * IRSendRecLooper.h
 * 
 * Repeatedly sends and receives messages - stops for delay periods to allow alternating between sending and receiving
 * Requires manual updating from the arduino loop() method so that it properly restarts after delay periods
*/

#ifndef IR_SEND_REC_LOOPER_H
#define IR_SEND_REC_LOOPER_H

#include <iostream>


#include "Arduino.h"
#include "../HeartIRTimer/HeartIRTimer.h"
#include "../HeartMessageEnums/HeartMessages.h"

class IRSendRecLooper {
public:
	
	/**
		modes -- accessible via IRSendRecLooper::modeName
	*/
	enum SendRecLoopState { STOPPED, SENDING, POST_SEND, CHECKING, POST_CHECK };
	
    /*
     IR send attached to pin 9
     IR rec attached to pin 10
     */
	//recPin is the pin for the receiver output
	//starts in send mode -- must checkMessages to start receiving
	IRSendRecLooper();
    
    // starts a send/rec loop, which pauses between phases, awaiting update - pass in message to send and messages to listen for
    void start(IRMessage sendMsg, long sendFrequency);
    
    // set a send msg
    void setSendMsg(IRMessage sendMsg);
    
    // add a msg to listen to, if it's a unique addition
	void addRecMsg(IRMessage msg);
	
	// delete a msg being listened for
	void deleteRecMsg(IRMessage msg);
    
    // update the send/rec loop, which will cause it to initiate the next phase in its progression if it's time
    // returns true if a msg was received this update
    bool update();
	
	// Was the message received in the last update?
	bool wasLastReceivedMsg(IRMessage msg);
	
private:
	// send the sendMsg
	void send();
	
	// check for the recMsg
	void check();
	
	// true if currently listening for this message
	bool listeningForMsg(IRMessage msg);
	
    HeartIRTimer ir;
    
    SendRecLoopState state;
    
	bool sending; //Is the HeartIR in sending mode (disables receiver)
    
    IRMessage sendMsg;
    IRMessage* recMsgs;
    int numRecMsgs;
    long lastMessageReceived;
    bool receivedMsg;
    long endOfSend;
    long endOfCheck;
    // how frequently should an attempt to send be made
    long sendFrequency;
    long lastRecTime;
};

#endif
