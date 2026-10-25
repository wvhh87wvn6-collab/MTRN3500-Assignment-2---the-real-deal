#pragma once
// GNSS.h
#include <NetworkedModule.h>

ref class GNSS : public NetworkedModule {
public:
	GNSS(SM_ThreadManagement^ SM_TM_, uint8_t heartbit, SM_GPS^ TMM_SM_GNSS_) : NetworkedModule(SM_TM_, heartbit) {
		this->TMM_SM_GNSS_ = TMM_SM_GNSS_;
		this->name = "[GNSS Module]";
		this->debug_sleep_time = 200;
	}

	error_state connect(String^ hostName, int portNumber) override {
		return error_state::SUCCESS;
	}
	error_state communicate() override {
		return error_state::SUCCESS;
	}


protected:
	SM_GPS^ TMM_SM_GNSS_;
};