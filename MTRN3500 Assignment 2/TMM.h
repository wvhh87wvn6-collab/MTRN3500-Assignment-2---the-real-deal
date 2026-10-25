#pragma once

#include <UGVModule.h>
#include <SMObjects.h>

ref class ThreadManagement : public UGVModule {
public:
    ThreadManagement() {
        this->name = "[TMM]";
    };
    error_state setupSharedMemory();

    void shutdownModules();

    error_state processSharedMemory() override;
    bool getShutdownFlag() override;
    void threadFunction() override;
    error_state processHeartbeat() override;

private:
    // Add any additional data members or helper functions here
    SM_ThreadManagement^ TMM_SM_TM_;
    SM_Laser^ TMM_SM_LiDAR_;
    SM_GPS^ TMM_SM_GNSS_;

    array<Stopwatch^>^ TMM_StopwatchList;
    array<Thread^>^ ThreadList;
    array<ThreadProperties^>^ ThreadPropertiesList;
};
