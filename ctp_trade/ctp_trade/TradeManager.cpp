#include "TradeManager.h"

#include "GLogWrapper.h"

#include <algorithm>

CTradeManager::CTradeManager()
: md_manager(this)
, td_manager(this)
{
	char module_dir[MAX_PATH + 1] = { 0 };
	::GetModuleFileName(nullptr, module_dir, MAX_PATH);
	root_log_ = module_dir;

	root_log_ = root_log_.substr(0, root_log_.find_last_of("\\")) + "\\glog_log";
}

bool CTradeManager::initial_trader()
{
	bool trade_ret = true;

	// log
	CGLog::get_glog()->init_log(root_log_.c_str());
	// redis
	trade_ret &= redis_.connect_redis_instance();
	// md
	std::vector<std::string> vect;
	load_all_interested_ins(vect);
	trade_ret &= md_manager.initial_md_manager(vect);
	trade_ret &= md_manager.subscribe_market();

	// td
	trade_ret &= td_manager.initial_td_manager();
	if (0 == td_manager.execute_login_confirm())
	{
		trade_ret &= true;
	}
	else
	{
		trade_ret &= false;
	}

	return trade_ret;
}

void CTradeManager::release_trader()
{
	// td
	td_manager.release_td_manager();
	// md
	md_manager.release_md_manager();
	// redis
	redis_.free_redis_instance();
	// log
	CGLog::get_glog()->release_log();
}

void CTradeManager::attach_trade_strategy(CStrategy* stg)
{
	stg_list_.emplace_back(std::shared_ptr<CStrategy>(stg));
}

void CTradeManager::dettach_trade_strategy(CStrategy* stg)
{
	auto list_it = std::find(stg_list_.begin(), stg_list_.end(), std::shared_ptr<CStrategy>(stg));
	if (list_it != stg_list_.end())
	{
		stg_list_.remove(*list_it);
	}
}

void CTradeManager::notify_decision_data(int period, candle_bar& bar)
{
	for_each(stg_list_.begin(), stg_list_.end(), [period, &bar](std::shared_ptr<CStrategy>& stg)
	{
		stg->update(period, bar);
	});
}

void CTradeManager::load_all_interested_ins(std::vector<std::string>& vec_strs)
{
	vec_strs.push_back("IF1606");
	vec_strs.push_back("j1701");
	vec_strs.push_back("rb1610");
	vec_strs.push_back("CF609");
}