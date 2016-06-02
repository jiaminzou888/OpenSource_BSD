#pragma once

#include "TdTradeSpi.h"

class CTradeManager; // Forward Declaration For Management Pointer

class CTdManager
{
public:
	CTdManager(CTradeManager* trade_ptr);

	bool initial_td_manager();
	void release_td_manager();

	int  execute_login_confirm();

private:
	CTdTradeSpi td_;
	std::shared_ptr<CTradeManager>		td_trade_pointer;		// Trade Management
};