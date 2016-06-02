#include "MdManager.h"

#include "EfficientMap.h"
#include "TradeManager.h"

#include <algorithm>

CMdManager::CMdManager(CTradeManager* trade_ptr)
: md_trade_pointer(trade_ptr)
, md_(this)
{

}

bool CMdManager::initial_md_manager(std::vector<std::string>& ins)
{
	// initial capacity of vector
	bool md_ret = true;
	
	// Md Module
	set_instruments(ins);
	md_ret &= md_.initial_md_broadcast(ins);
	

	return md_ret;
}

void CMdManager::release_md_manager()
{
	md_.release_md_broadcast();
}

bool CMdManager::get_md_conncet_status()
{
	return md_.get_md_connect_flag();
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

void CMdManager::push_min_data(std::string ins, candle_bar& bar)
{
	min_1_base_[ins].push_bar(bar);
}

bool CMdManager::calculate_decision_kdata(int type)
{
	bool cal_ret = true;

	switch (type)
	{
	case CCandleBar::MIN_ONE:
		break;
	}

	return cal_ret;
}

void CMdManager::set_instruments(std::vector<std::string>& ins)
{
	for (std::string in_ : ins)
	{
		CCandleBar data_one;
		data_one.set_candle_type(CCandleBar::MIN_ONE);
		efficient_map_operation(min_1_base_, in_, data_one);

		CCandleBar data_five;
		data_five.set_candle_type(CCandleBar::MIN_FIVE);
		efficient_map_operation(min_5_base_, in_, data_five);
	}
}