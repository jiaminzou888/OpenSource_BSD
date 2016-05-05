#include "MdManager.h"
#include "EfficientMap.h"
#include "GLogWrapper.h"

#include <algorithm>


CMdManager::CMdManager()
{
	char module_dir[MAX_PATH + 1] = { 0 };
	::GetModuleFileName(nullptr, module_dir, MAX_PATH);
	root_log_ = module_dir;

	root_log_ = root_log_.substr(0, root_log_.find_last_of("\\")) + "\\glog_log";
}

bool CMdManager::initial_md_manager(std::vector<std::string>& ins)
{
	// initial capacity of vector
	bool md_ret = true;
	
	CGLog::get_glog()->init_log(root_log_.c_str());
	md_ret &= redis_.connect_redis_instance();
	md_ret &= md_.initial_md_broadcast(ins);

	return md_ret;
}

void CMdManager::release_md_manager()
{
	md_.release_md_broadcast();
	redis_.free_redis_instance();
	CGLog::get_glog()->release_log();
}

bool CMdManager::get_md_conncet_status()
{
	return md_.get_connect_flag();
}

bool CMdManager::subscribe_market()
{
	bool sub_ret = false;

	size_t ins_size = md_.get_instruments_size();

	if (0 == ins_size)
	{
		return sub_ret;
	}

	// Subscribe MD Data
	char** instr	= new char*[ins_size];
	md_.get_instrument_name(instr, ins_size);

	// Waiting For Login Successfully At First Forever, I'll Optimize It Sooner Or Later.
	while (true)
	{
		if (md_.get_ready_subscribe())
		{
			sub_ret = md_.subscribe_instruments(instr, (int)ins_size);
			break;
		}
	}
	
	delete[] instr;
	instr = nullptr;

	return sub_ret;
}

void CMdManager::attach_md_strategy(CStrategy* stg)
{
	md_.attach_observer(stg);
}