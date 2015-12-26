#include "RFSend.h"

RFSend::RFSend(uint8_t sendPin, uint8_t sendID) {
	pin = sendPin;
	msgCount = 0;
	
	//Initialize the message sender id
	msg[0] = sendID;//always send id as the first part of the message
}

void RFSend::init() {
	vw_set_tx_pin(pin);
	vw_setup(BITS_PER_SECOND);
}

void RFSend::send(RFMessage m, int numSends) {
	msg[1] = uint8_t(m); //convert from enum type
	msg[2] = msgCount;
	
	for (int i = 0; i < numSends; i++) {
		vw_wait_tx(); //wait for transmitter to become available
		vw_send(msg, BYTES_PER_MESSAGE);
	}
	if(msgCount==255) msgCount = 0;
	else msgCount++;
}
