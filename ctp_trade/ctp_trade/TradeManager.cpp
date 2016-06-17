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
#ifdef _TRADE_TIME_
	trade_ret &= md_manager.subscribe_market();
#endif	// _TRADE_TIME_

	// td
	trade_ret &= td_manager.initial_td_manager();
#ifdef _TRADE_TIME_
	if (0 == td_manager.execute_login_confirm())
	{
		trade_ret &= true;
	}
	else
	{
		trade_ret &= false;
	}
#endif	// _TRADE_TIME_

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
	// Attach A New Strategy
	stg->set_td_module(&td_manager);
	stg_container_.emplace_back(std::shared_ptr<CStrategy>(stg));
	/*
	// Open The Corresponding Thread
	CppThread* stg_thread = new CppThread;
	stg_thread->set_data(this);
	stg_thread->set_thread_index(stg_container_.size()+1);
	stg_thread->create_thread(trade_process_function);
	stg_threads_.push_back(std::shared_ptr<CppThread>(stg_thread));
	*/
}

void CTradeManager::detach_trade_strategy(CStrategy* stg)
{
	auto stg_itera = std::find(stg_container_.begin(), stg_container_.end(), std::shared_ptr<CStrategy>(stg));
	if (stg_itera != stg_container_.end())
	{
		stg_container_.erase(stg_itera);
	}

	/*
	// Find The Strategy That Will Be Detached
	int	   stg_index  = 0;
	auto   stg_iteta  = stg_container_.begin();
	for (; stg_iteta != stg_container_.end(); ++stg_iteta)
	{
		if (stg == stg_iteta->get())
		{
			break;
		}
		else
		{
			++stg_index;
		}
	}
	
	// If stg Existed,
	if (stg_index != stg_container_.size())
	{
		// Close And Erase The Thread,
		stg_threads_[stg_index]->close_thread();
		stg_threads_.erase(stg_threads_.begin() + stg_index);
		// Update The Rest Of 'thread_id',
		for (auto thread_id_it = stg_threads_.begin() + stg_index; thread_id_it != stg_threads_.end(); ++thread_id_it)
		{
			size_t old_id = thread_id_it->get()->get_thread_index();
			thread_id_it->get()->set_thread_index(old_id - 1);
		}

		// Detach The Corresponding Strategy.
		stg_container_.erase(stg_iteta);
	}
	*/
}

void CTradeManager::notify_decision_data(int period, candle_bar& bar)
{
	for_each(stg_container_.begin(), stg_container_.end(), [period, &bar](std::shared_ptr<CStrategy>& stg)
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