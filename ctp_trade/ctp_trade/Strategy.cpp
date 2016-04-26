#include "Strategy.h"
#include "EfficientMap.h"
#include "GLogWrapper.h"

#include <iostream>
#include <algorithm>

const int minute_tick_nums = 120;

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

	// Check Out If TurnOver Had Changed
	std::vector<mtk_data>& mtk_vec = tick_base_[ins];
	if (mtk_vec.empty())
	{
		tick_base_[ins].push_back(data);
	}
	else
	{
		if (mtk_vec.back().Turnover < data.Turnover)
		{
			tick_base_[ins].push_back(data);
		}
		else if (mtk_vec.back().Turnover > data.Turnover)
		{
			// Read Demand Tick Data
			CGLog::get_glog()->print_log("What Happened.!?  ");
			CGLog::get_glog()->print_log("TickType\t\t  InstrumentID\t\t  UpdateTime\t\t  TradingDay\t\t  LastPrice\t\t  Turnover\t\t  ");

			char data_info[1024] = { 0 };
			sprintf_s(data_info, sizeof(data_info)-1, "Last:  %s\t\t  %d\t\t  %d\t\t  %lf\t\t  %lf\t\t  ", mtk_vec.back().InstrumentID, mtk_vec.back().UpdateMillisec, atoi(mtk_vec.back().TradingDay), mtk_vec.back().LastPrice, mtk_vec.back().Turnover);
			CGLog::get_glog()->print_log(data_info);
			sprintf_s(data_info, sizeof(data_info)-1, "NewO:  %s\t\t  %d\t\t  %d\t\t  %lf\t\t  %lf\t\t  ", data.InstrumentID, data.UpdateMillisec, atoi(data.TradingDay), data.LastPrice, data.Turnover);
			CGLog::get_glog()->print_log(data_info);

			abort();
		}
	}
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

	// Make Sure That I Have An Integrated Minute Bar At Least
	if (tick_base_[ins].size() > minute_tick_nums)
	{
		// The Basic Elements For Calculation
		char*	update_time			= nullptr;
		int		real_tick_nums		= 0;
		int		current_tick_minute = -1;
		char	current_minute[3]	= { 0 };

		// Guarantee The Current Minute Bar In The Light Of The First Data, But
		// How To Avoid Truncated Two Parts Which Just Make Up One Minute ? 
		std::vector<mtk_data> min_ticks;
		min_ticks.reserve(minute_tick_nums);

		auto tick = tick_base_[ins].begin();
		min_ticks.push_back(*tick);
		real_tick_nums ++;

		update_time = tick->UpdateTime;
		strncpy_s(current_minute, update_time + 3, 2);
		current_tick_minute = atoi(current_minute);
		
		int tmp_current_minute = 0;
		for (auto tick = tick_base_[ins].begin() + 1; tick != tick_base_[ins].end(); ++ tick)
		{
			// Obtain The Current Minute Time
			update_time = tick->UpdateTime;
			strncpy_s(current_minute, update_time + 3, 2);
			tmp_current_minute = atoi(current_minute);

			if (tmp_current_minute == current_tick_minute)
			{
				min_ticks.push_back(*tick);
				real_tick_nums ++;
			}
			else if (tmp_current_minute > current_tick_minute)
			{
				break;
			}
			else
			{
				abort();
			}
		}

		// Accurate Minute Bar Calculation
		candle_bar min1_bar;
		convert_tick2min(min_ticks, min1_bar);
		kdata_[ins][K_MIN].get_candle_container().push_back(min1_bar);
		print_info(ins.c_str());

		// Erase Used Data
		tick_base_[ins].erase(tick_base_[ins].begin(), tick_base_[ins].begin() + real_tick_nums);
	}
}

void CStrategy::convert_tick2min(std::vector<mtk_data>& tick_vec, candle_bar& min_bar)
{
	// Get The Both Ends Data
	mtk_data& first_data	= *(tick_vec.begin());
	mtk_data& last_data		= *(tick_vec.rbegin());

	// Initial Basic Information
	min_bar.bar_type	= K_MIN;
	strncpy_s(min_bar.bar_name, first_data.InstrumentID, sizeof(mtk_data_name));
	strncpy_s(min_bar.bar_ecg,  first_data.ExchangeID, sizeof(mtk_data_exchange));

	min_bar.trade_day	= atoi(first_data.TradingDay);
	min_bar.open_price	= first_data.LastPrice;
	min_bar.close_price = last_data.LastPrice;
	min_bar.turn_over	= last_data.Turnover;

	min_bar.change_point	=  min_bar.close_price  - min_bar.open_price;
	min_bar.change_percent	= (min_bar.change_point / min_bar.open_price) * 100;

	min_bar.high_price = min_bar.open_price;
	min_bar.low_price  = min_bar.open_price;
	
	std::for_each(tick_vec.begin(), tick_vec.end(), [&tick_vec, &min_bar](mtk_data dt)
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
	for (auto data = tick_vec.begin(); data != tick_vec.end(); ++data)
	{
		sprintf_s(data_info, sizeof(data_info)-1, "%d\t\t  %s\t\t  %d\t\t  %d\t\t  %lf\t\t  %lf\t\t  ", K_MIN, data->InstrumentID, data->UpdateMillisec, atoi(data->TradingDay), data->LastPrice, data->Turnover);
		CGLog::get_glog()->print_log(data_info);
	}
	CGLog::get_glog()->print_log("\r\n");

	// Check Out The Synthetic
	sprintf_s(data_info, sizeof(data_info)-1, "%lf\t\t  %lf\t\t  %lf\t\t  %lf\t\t  %lf\t\t  %lf\t\t  %lf\t\t  ", min_bar.open_price, min_bar.close_price, min_bar.high_price, min_bar.low_price, min_bar.change_point, min_bar.change_percent, min_bar.turn_over);
	CGLog::get_glog()->print_log(data_info);
	CGLog::get_glog()->print_log("\r\n");
	//////////////////////////////////////////////////////////////////////////
}

CMAStrategy::CMAStrategy(instr_container instr)
	: CStrategy(instr)
{

}

void CMAStrategy::print_info(const char* ins_str)
{
	std::cout << "Moving Average Strategy:  " << ins_str << std::endl;
}