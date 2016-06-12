#pragma once

#include "RedisWrapper.h"
#include "MdManager.h"
#include "TdManager.h"
#include "Strategy.h"

class CTradeManager
{
public:
	CTradeManager();

	static void trade_process_function(void* data);

	bool initial_trader();
	void release_trader();

	void attach_trade_strategy(CStrategy* stg);
	void detach_trade_strategy(CStrategy* stg);
	void notify_decision_data(int period, candle_bar& bar);

private:
	void load_all_interested_ins(std::vector<std::string>& vec_strs);

private:
	std::string		root_log_;

	std::vector<std::shared_ptr<CppThread>>	stg_threads_;				// Strategies Thread
	std::vector<std::shared_ptr<CStrategy>>	stg_container_;	// Registered Strategies
	

private:
	CRedisWrapper	redis_;
	CMdManager		md_manager;
	CTdManager		td_manager;
};