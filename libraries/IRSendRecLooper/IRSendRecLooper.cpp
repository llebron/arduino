//
//  IRSendRecLooper.cpp
//  
//
//  Created by Larry LeBron on 11/22/14.
//
//

#include "IRSendRecLooper.h"

IRSendRecLooper::IRSendRecLooper(){
	state = STOPPED;
}

// starts a send/rec loop, which pauses between phases, awaiting update - pass in message to send and message to listen for
// msg to send, how often to send (tracked from end of any send)
void IRSendRecLooper::start(IRMessage newSendMsg, long newSendFrequency) {
	sendMsg = newSendMsg;
	sendFrequency = newSendFrequency;
	// start the first send
	send();
	// Initial declaration, capped at size 10. Fixme: make sizing dynamic or import a dynamic container
	recMsgs = new IRMessage[10];
	numRecMsgs = 0;
	lastRecTime = 0;
}

void IRSendRecLooper::setSendMsg(IRMessage newSendMsg) {
	sendMsg = newSendMsg;
}

bool IRSendRecLooper::listeningForMsg(IRMessage msg) {
	for (int i = 0; i < numRecMsgs; i++) {
		if (recMsgs[i] == msg) {
			return true;
		}
	}
	return false;
}

// add a msg to listen to at end of array, if it's a unique addition
void IRSendRecLooper::addRecMsg(IRMessage msg) {
	if (!listeningForMsg(msg)) {
		recMsgs[numRecMsgs] = msg;
		numRecMsgs++;	
		Serial.print("Adding rec msg "); Serial.println(msg);
	}
    	for (int i = 0; i < numRecMsgs; i++) {
    			Serial.print("Checking for "); Serial.println(recMsgs[i]);
    	}
}

// add a msg to listen to at end of array
void IRSendRecLooper::deleteRecMsg(IRMessage msg) {
	int indexToDel = -1;
	
	for (int i = 0; i < numRecMsgs; i++) {
		if (recMsgs[i] == msg) {
			indexToDel = i;
		}
	}
	
	// Only take action if the value is in the array
	if (indexToDel != -1) {
		// Don't need to do a copy if we're removing the last in the array
		if (indexToDel != (numRecMsgs-1)) {
			// We're removing a mid-array value, need to copy the end val into this slot
			recMsgs[indexToDel] = recMsgs[numRecMsgs-1];
		}
	
		// Decrement the tracking val
		numRecMsgs--;
	}	
}

void IRSendRecLooper::send() {	
	state = SENDING;
	Serial.print("sending"); Serial.println(sendMsg);
	//long beforeSend = millis();
	
	ir.send(sendMsg);
	
	//long afterSend = millis();
	//Serial.println(afterSend-beforeSend);
	
	// set end of send time & new state
	endOfSend = millis();
	
	state = POST_SEND;
	
	// Could be used to force a receiver re-enable right here. Not sure if this is a good idea.
	//ir.checkMessages();
}

void IRSendRecLooper::check() {
	Serial.println("check");
	state = CHECKING;
    receivedMsg = false;
    // A message has been received, so we'll check for a match
    if (ir.checkMessages()) {
    	long lastMsg = ir.getLastMessage();
    	
		/*Serial.print("Got");
		Serial.println(ir.getLastMessage());
		Serial.print("Converted");Serial.println(lastMsg);
		Serial.print("Listening for num msgs: ");
		Serial.println(numRecMsgs);*/

    	for (int i = 0; i < numRecMsgs; i++) {
    		if (lastMsg == recMsgs[i]) {
    		    Serial.print("Received "); Serial.println(recMsgs[i]);
    		    Serial.print("Time since last "); Serial.println(millis()-lastRecTime);

    		    lastRecTime = millis();

            	receivedMsg = true;
            	lastMessageReceived = lastMsg;
            	break;
            } /*else {
            	Serial.print("It isn't "); Serial.println(recMsgs[i]);

            }*/
    	}
    }
    
	// set end of check time & new state
	endOfCheck = millis();
	state = POST_CHECK;
}

// update the send/rec loop, which will cause it to initiate the next phase in its progression if it's time
// return receivedMsg, true if a message was received this update
bool IRSendRecLooper::update() {
	// Always reset the msg state at update
	receivedMsg = false;
	
	// If enough time has passed since send, switch to checking
    if (state == POST_SEND) {
    	long timeSinceSend = millis()-endOfSend;
    	if( timeSinceSend >= POST_IR_SEND_DELAY) {
    		check();
    	} 
    } else if (state == POST_CHECK) {
    	// If enough time has passed since last send, send again
    	long timeSinceSend = millis()-endOfSend;
    	if (timeSinceSend >= sendFrequency) {
    		send();
    	} else {
    		// If enough time has passed since last check, check again
    		long timeSinceCheck = millis()-endOfCheck;
			if (timeSinceCheck >= PER_IR_CHECK_DELAY) {
				check();
			}
    	}
    }
    return receivedMsg;
}

// was the message received this update?
bool IRSendRecLooper::wasLastReceivedMsg(IRMessage msg) {
    return lastMessageReceived == msg;
}