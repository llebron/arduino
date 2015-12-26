	#ifndef GAME_MESSAGES_H
	#define GAME_MESSAGES_H
	
	//Holds the Game Messages for common use
	
	
	/**
		The IR messages that will be recognized
		rayGun: The guard's raygun
		prison: The normal prison message
	*/
	enum IRMessage { IRRayGun, IRPrison, IRJailbreak, IRGuardStation, IRAirlock };
	
	//The RF messages that will be recognized
	enum RFMessage {RFJailbreak, RFThiefScan, RFGotData, RFCapturedWithData, RFThievesWin, };
	
	#endif