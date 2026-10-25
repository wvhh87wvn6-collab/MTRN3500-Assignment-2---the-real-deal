#pragma once
/*****************
UGVModule.h
ALEXANDER CUNIO 2024
*****************/

/*
This file defines the base class for ALL UGV modules (TMM, Laser, GPS, Vehicle Control, Display, Controller).
Basic classes (TMM, Controller) will be derived directly from this class, whereas those that require networking
to external devices (GPS, Laser, VC, Display) will derive from the networked module class (which itself inherits from this class).
For clarification of inheritance requirements see diagram in the assignment specification.
*/

#include <iostream>
#include <SMObjects.h>
#include <random>

// ERROR HANDLING. Use this as the return value in your functions
enum error_state {
	SUCCESS,
	ERR_STARTUP,
	ERR_NO_DATA,
	ERR_INVALID_DATA,
	ERR_SM,
	ERR_CONNECTION,
	ERR_TMM_NO_HEARTBEAT,
	ERR_CRITICAL_THREAD_FAILURE
	// Define your own additional error types as needed
};


ref class UGVModule // abstract
{
public:
	UGVModule();
	UGVModule(SM_ThreadManagement^ SM_TM_, uint8_t heartbit);
	virtual error_state processSharedMemory();// = 0;
	virtual bool getShutdownFlag();// = 0;
	virtual void threadFunction();// = 0;

	static void printError(error_state error)
	{
		switch (error)
		{
		case SUCCESS:
			std::cout << "Success." << std::endl;
			break;
		case ERR_NO_DATA:
			std::cout << "ERROR: No Data Available." << std::endl;
			break;
		case ERR_INVALID_DATA:
			std::cout << "ERROR: Invalid Data Received." << std::endl;
			break;
			// ADD PRINTOUTS FOR OTHER ERROR TYPES
		default:
			std::cout << "ERROR: Some other error occurred lol ( #" << error  << " )" << std::endl;
			break;
		}
	}

protected:
	virtual error_state  processHeartbeat();// = 0;
	SM_ThreadManagement^ TMM_SM_TM_;
	uint8_t heartbit;
	int crash_limit = 400; // always define this in inherited classes
	Stopwatch^ Heartbeat_Stopwatch;
	String^ name;

	int debug_sleep_time = 100;
};
