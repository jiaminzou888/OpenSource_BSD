#pragma once

#include "TdTradeSpi.h"

class CTradeManager; // Forward Declaration For Management Pointer

// Be Compatible For Different Trade Interface
class CTdManager
{
public:
	CTdManager(CTradeManager* trade_ptr);

	bool initial_td_manager();
	void release_td_manager();

	// login and confirmation
	int  execute_login_confirm();

	// order insert
	int limit_order_insert(const char* ins_id, char action_direct, char open_flag, double price, int quantity);
	
private:
	int	 insert_order(CThostFtdcInputOrderField& order_req);

private:
	CTdTradeSpi td_;
	std::shared_ptr<CTradeManager>		td_trade_pointer;		// Trade Management
};