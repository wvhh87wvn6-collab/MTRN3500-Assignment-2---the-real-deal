#include "TMM.h"
#include "LiDAR.h"
#include "GNSS.h"

error_state ThreadManagement::setupSharedMemory() {
	TMM_SM_TM_ =  gcnew SM_ThreadManagement;
	TMM_SM_LiDAR_ = gcnew SM_Laser;
	TMM_SM_GNSS_ = gcnew SM_GPS;
	return error_state::SUCCESS;
}

error_state ThreadManagement::processSharedMemory() {
	//  no clue what this is for
	return error_state::SUCCESS;
}

void ThreadManagement::shutdownModules() {
	TMM_SM_TM_->shutdown = 0xFF;
	Console::WriteLine(name + " shutting all down");
	// This seems like a flawed system -> if a thread stalls and triggers shutdown then it won't see the flag and suicide
	// add a timeout to force shutdown?
	// use Thread::Abort() -> look into later?
}

bool ThreadManagement::getShutdownFlag() {
	return (TMM_SM_TM_->shutdown & bit_TM);
}

void ThreadManagement::threadFunction() {
	Console::WriteLine(name + " TMM Thread is starting");

	ThreadPropertiesList = gcnew array<ThreadProperties^>{
		gcnew ThreadProperties(gcnew ThreadStart(gcnew LiDAR(TMM_SM_TM_, bit_LASER,  TMM_SM_LiDAR_), &LiDAR::threadFunction), true, bit_LASER, "LiDAR Thread"),
		gcnew ThreadProperties(gcnew ThreadStart(gcnew GNSS(TMM_SM_TM_, bit_GPS, TMM_SM_GNSS_), &GNSS::threadFunction), false, bit_GPS, "GNSS Thread"),
	};

	//make a list of thread
	ThreadList = gcnew array<Thread^>(ThreadPropertiesList->Length);

	//make the stopwatch list
	TMM_StopwatchList = gcnew array<Stopwatch^>(ThreadPropertiesList->Length);

	//make thread barriers
	TMM_SM_TM_->ThreadBarrier = gcnew Barrier(ThreadPropertiesList->Length + 1);

	//start all the threads
	for (int i = 0; i < ThreadPropertiesList->Length; i++)
	{
		TMM_StopwatchList[i] = gcnew Stopwatch;
		ThreadList[i] = gcnew Thread(ThreadPropertiesList[i]->ThreadStart_);
		ThreadList[i]->Start();
	}

	//wait at the TMT thread barrier
	Console::WriteLine(name + " waiting at barrier");
	TMM_SM_TM_->ThreadBarrier->SignalAndWait();

	Console::WriteLine(name + " starting stopwatches");
	for (int i = 0; i < ThreadList->Length; i++)
	{
		TMM_StopwatchList[i]->Start();
	}

	//start the thread loop + keep checking the heartbeats
	int i = 0;
	while (!Console::KeyAvailable && !getShutdownFlag())
	{
		processHeartbeat();
		Thread::Sleep(50);

		if (i++ > 100) break;
	}

	//end the thread loop + shut down all threads
	shutdownModules();

	//join all threads
	for (int i = 0; i < ThreadPropertiesList->Length; i++)
	{
		ThreadList[i]->Join();
	}

	Console::WriteLine(name + " TMM Thread is terminated");
}

error_state ThreadManagement::processHeartbeat() {
	//check the heartbeat flag of the ith thread (is it high?)
	for (int i = 0; i < ThreadList->Length; i++)
	{
		if (TMM_SM_TM_->heartbeat & ThreadPropertiesList[i]->BitID)
		{
			Console::WriteLine(name + " heartbeat " + ThreadPropertiesList[i]->ThreadName + " detected at " + TMM_StopwatchList[i]->ElapsedMilliseconds + "ms");
			TMM_SM_TM_->heartbeat &= ~ThreadPropertiesList[i]->BitID;
			TMM_StopwatchList[i]->Restart();
		}

		else
		{
			//check the stopwatch, is the time exceed the crash limit?
			if (crash_limit < TMM_StopwatchList[i]->ElapsedMilliseconds)
			{
				//is the ith thread a critical one?
				if (ThreadPropertiesList[i]->Critical)
				{
					Console::WriteLine(name + " " + ThreadPropertiesList[i]->ThreadName + " (critical) failure. Shutting down all threads");
					shutdownModules();//shut down all
					return error_state::ERR_CRITICAL_THREAD_FAILURE;
				}

				else
				{
					Console::WriteLine(name + " " + ThreadPropertiesList[i]->ThreadName + " failed...Restarting");
					ThreadList[i]->Abort();
					ThreadList[i] = gcnew Thread(ThreadPropertiesList[i]->ThreadStart_);
					TMM_SM_TM_->ThreadBarrier = gcnew Barrier(1);
					ThreadList[i]->Start();//try to restart
				}
			}
		}
	}

	return error_state::SUCCESS;
}