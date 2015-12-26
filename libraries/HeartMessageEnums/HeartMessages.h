	#ifndef HEART_CONSTS_H
	#define HEART_CONSTS_H

    const int POST_IR_SEND_DELAY =  40;
    const int PER_IR_CHECK_DELAY =  100;

	/**
		The IR messages that will be recognized
	*/
	enum IRMessage { IRMyHeartPump, IRYourHeartPump, IRTouching };

    /** 
        The Heart State
     */
    enum HeartState { HeartSearching, HeartFound, HeartTouching };
	
	#endif