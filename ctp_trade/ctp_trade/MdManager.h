#pragma once

#include "MdBroadCast.h"
#include "CppThread.hpp"
#include "CandleBar.h"

class CTradeManager; // Forward Declaration For Management Pointer

// All Different Period-Circle KData's Management
class CMdManager
{
public:
	CMdManager(CTradeManager* trade_ptr);

	bool initial_md_manager(std::vector<std::string>& ins);
	void release_md_manager();

	bool get_md_conncet_status();
	bool subscribe_market();

	void push_min_data(std::string ins, candle_bar& bar);

	bool calculate_decision_kdata(int type);

private:
	void set_instruments(std::vector<std::string>& ins);

private:
	std::map<std::string, CCandleBar>	min_1_base_;			// Original One Minute K Data
	std::map<std::string, CCandleBar>	min_5_base_;				

	std::shared_ptr<CTradeManager>		md_trade_pointer;		// Trade Management

	CMdBroadCast	md_;
};