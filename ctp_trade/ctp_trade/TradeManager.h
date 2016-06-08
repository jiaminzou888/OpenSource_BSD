#pragma once

#include "RedisWrapper.h"
#include "MdManager.h"
#include "TdManager.h"
#include "Strategy.h"

class CTradeManager
{
public:
	CTradeManager();

	bool initial_trader();
	void release_trader();

	void attach_trade_strategy(CStrategy* stg);
	void dettach_trade_strategy(CStrategy* stg);
	void notify_decision_data(int period, candle_bar& bar);

private:
	void load_all_interested_ins(std::vector<std::string>& vec_strs);

private:
	std::string		root_log_;
	std::list<std::shared_ptr<CStrategy>>	stg_list_;	// Registered Strategies

private:
	CRedisWrapper	redis_;
	CMdManager		md_manager;
	CTdManager		td_manager;
};