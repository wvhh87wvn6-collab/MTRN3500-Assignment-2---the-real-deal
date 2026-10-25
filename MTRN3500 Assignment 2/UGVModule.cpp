#include <UGVModule.h>

UGVModule::UGVModule() {
	this->TMM_SM_TM_ = gcnew SM_ThreadManagement;
	this->heartbit = 0x01;
	Heartbeat_Stopwatch = gcnew Stopwatch();
	this->name = "[Thread Manager Init (YOU SHOULDN'T SEE THIS)]";
}

UGVModule::UGVModule(SM_ThreadManagement^ TMM_SM_TM_, uint8_t heartbit) {
	this->TMM_SM_TM_ = TMM_SM_TM_;
	this->heartbit = heartbit;
	Heartbeat_Stopwatch = gcnew Stopwatch();
	std::srand(std::time(nullptr));
	this->name = "[UGVModule r" + (std::rand() % 90 + 10).ToString() + " ]";
}

error_state UGVModule::processHeartbeat() {

	if ((TMM_SM_TM_->heartbeat & heartbit) == 0)
	{
		TMM_SM_TM_->heartbeat |= heartbit;
		Heartbeat_Stopwatch->Restart();
	}
	else
	{
		if (crash_limit < Heartbeat_Stopwatch->ElapsedMilliseconds)
		{
			// shutdownThreads();
			TMM_SM_TM_->shutdown = 0xFF; 
			Console::WriteLine(name + " heartbeat_crash_limit exceeded; shutting down ");
			return error_state::ERR_TMM_NO_HEARTBEAT;
		}
	}

	return error_state::SUCCESS;
}

error_state UGVModule::processSharedMemory() {
	return error_state::SUCCESS;
}

bool UGVModule::getShutdownFlag() {
	return (TMM_SM_TM_->shutdown & heartbit);
}


void UGVModule::threadFunction() {
	Console::WriteLine(name + " waiting at barrier");
	TMM_SM_TM_->ThreadBarrier->SignalAndWait();

	while (!getShutdownFlag()) {
		processHeartbeat();
		Console::WriteLine(name + " threadFunction called; sleeping " + debug_sleep_time + "ms");
		Thread::Sleep(debug_sleep_time);
	}
	Console::WriteLine(name + " thread shut down");
}
