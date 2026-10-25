#pragma once
// LiDAR.h
#include <NetworkedModule.h>

ref class LiDAR : public NetworkedModule {
public:
	LiDAR(SM_ThreadManagement^ SM_TM_, uint8_t heartbit, SM_Laser^ TMM_SM_LiDAR_) : NetworkedModule(SM_TM_, heartbit) {
		this->TMM_SM_LiDAR_ = TMM_SM_LiDAR_;
		this->name = "[LiDAR]";
		this->debug_sleep_time = 80;
	}

	error_state connect(String^ hostName, int portNumber) override {
		return error_state::SUCCESS;
	}
	error_state communicate() override {
		return error_state::SUCCESS;
	}

protected:
	SM_Laser^ TMM_SM_LiDAR_;
};