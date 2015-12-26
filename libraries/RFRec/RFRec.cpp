#include "RFRec.h"

RFRec::RFRec(uint8_t recPin) {
	pin = recPin;
	//initialize msg to invalid values
	msg[0] = -1;
	msg[1] = -1;
	msg[2] = -1;
	msgCopy[0] = -1;
	msgCopy[1] = -1;
	msgCopy[2] = -1;
	msgLength = BYTES_PER_MESSAGE;
}

void RFRec::init() {
	vw_set_rx_pin(pin);
	vw_setup(BITS_PER_SECOND);
	vw_rx_start();       // Start the receiver PLL running
}

bool RFRec::checkMessages() {

	//copies last message into msg[] and returns true if checksum was good
	return vw_get_message(msg, &msgLength) ;
}

uint8_t* RFRec::getLastMessage() {
	for (int i = 0; i < BYTES_PER_MESSAGE; i++) {
		msgCopy[i] = msg[i];
	}
	return msgCopy;
}