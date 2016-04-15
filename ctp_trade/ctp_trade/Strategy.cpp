#include "Strategy.h"
#include "EfficientMap.h"
#include "GLogWrapper.h"

#include <iostream>
#include <algorithm>

const size_t minute_tick_num = 20;

CStrategy::CStrategy(instr_container instr)
{
	focused_inst_ = instr;

	for (auto ins : focused_inst_)
	{
		// Initial Tick Data Map
		std::vector<mtk_data> tick_vec;
		tick_vec.reserve(256);
		efficient_map_operation(tick_base_, ins, tick_vec);

		// Initial 1Min KData Map
		CCandleBar minute_bar;
		std::map<k_types, CCandleBar> kdata_bar;
		efficient_map_operation(kdata_bar, K_MIN, minute_bar);
		efficient_map_operation(kdata_, ins, kdata_bar);

		// Initial mutex lock
		efficient_map_operation(tick_mutex_, ins, std::shared_ptr<std::mutex>(new std::mutex));
	}
}

void CStrategy::update(mtk_data* data)
{
	tick_base_pushback(data->InstrumentID, *data);
}

void CStrategy::print_info(const char* ins_str)
{

}

size_t CStrategy::tick_base_getsize(std::string ins)
{
	std::lock_guard<std::mutex> lck(*tick_mutex_[ins]);
	return tick_base_[ins].size();
}

void  CStrategy::tick_base_pushback(std::string ins, mtk_data& data)
{
	std::lock_guard<std::mutex> lck(*tick_mutex_[ins]);
	tick_base_[ins].push_back(data);
}

const instr_container& CStrategy::get_focused_instr() const
{
	return focused_inst_;
}

void CStrategy::calculate_min_bar(std::string ins)
{
	for (auto ins : focused_inst_)
	{
		calculate_process(ins);
	}
}

void CStrategy::calculate_process(std::string ins)
{
	std::lock_guard<std::mutex> lck(*tick_mutex_[ins]);
	if (tick_base_[ins].size() > minute_tick_num)
	{
		candle_bar min1_bar;
		convert_tick2min(ins, min1_bar);
		kdata_[ins][K_MIN].get_candle_container().push_back(min1_bar);
		print_info(ins.c_str());
	}
}

void CStrategy::convert_tick2min(std::string ins, candle_bar& min_bar)
{
	// Get The Both Ends Data
	std::vector<mtk_data>& tick_vec = tick_base_[ins];

	mtk_data& first_data	= tick_vec[0];
	mtk_data& last_data		= tick_vec[minute_tick_num - 1];

	// Initial Basic Information
	min_bar.bar_type	= K_MIN;
	strncpy_s(min_bar.bar_name, first_data.InstrumentID, sizeof(mtk_data_name));
	strncpy_s(min_bar.bar_ecg,  first_data.ExchangeID, sizeof(mtk_data_exchange));

	min_bar.trade_day	= atoi(first_data.TradingDay);
	min_bar.open_price	= first_data.LastPrice;
	min_bar.close_price = last_data.LastPrice;
	min_bar.turn_over	= last_data.Turnover;

	min_bar.change_point	= min_bar.close_price  - min_bar.open_price;
	min_bar.change_percent	= min_bar.change_point / min_bar.open_price;

	min_bar.high_price = min_bar.open_price;
	min_bar.low_price  = min_bar.open_price;
	
	std::for_each(tick_vec.begin(), tick_vec.begin() + minute_tick_num, [&tick_vec, &min_bar](mtk_data dt)
	{
		if (dt.LastPrice > min_bar.high_price)
		{
			min_bar.high_price = dt.LastPrice;
		}

		if (dt.LastPrice < min_bar.low_price)
		{
			min_bar.low_price = dt.LastPrice;
		}
	});

	//////////////////////////////////////////////////////////////////////////
	// Read Demand Tick Data
	CGLog::get_glog()->print_log("bar_type\t\t  InstrumentID\t\t  UpdateTime\t\t  TradingDay\t\t  LastPrice\t\t  Turnover\t\t  ");

	char data_info[1024] = { 0 };
	for (auto data : tick_vec)
	{
		sprintf_s(data_info, sizeof(data_info)-1, "%d\t\t  %s\t\t  %d\t\t  %d\t\t  %lf\t\t  %lf\t\t  ", K_MIN, data.InstrumentID, data.UpdateMillisec, atoi(data.TradingDay), data.LastPrice, data.Turnover);
		CGLog::get_glog()->print_log(data_info);
	}
	CGLog::get_glog()->print_log("\r\n");

	// Check Out The Synthetic
	sprintf_s(data_info, sizeof(data_info)-1, "%lf\t\t  %lf\t\t  %lf\t\t  %lf\t\t  %lf\t\t  %lf\t\t  %lf\t\t  ", min_bar.open_price, min_bar.close_price, min_bar.high_price, min_bar.low_price, min_bar.change_point, min_bar.change_percent, min_bar.turn_over);
	CGLog::get_glog()->print_log(data_info);
	CGLog::get_glog()->print_log("\r\n");
	//////////////////////////////////////////////////////////////////////////

	// Erase Used Data
	tick_vec.erase(tick_vec.begin(), tick_vec.begin() + minute_tick_num);
}

CMAStrategy::CMAStrategy(instr_container instr)
	: CStrategy(instr)
{

}

void CMAStrategy::print_info(const char* ins_str)
{
	std::cout << "Moving Average Strategy:  " << ins_str << std::endl;
}