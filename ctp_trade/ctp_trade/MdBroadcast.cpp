#include "MdBroadcast.h"

#include "EfficientMap.h"
#include "GLogWrapper.h"

#include <iostream>
#include <algorithm>

const int max_minute_ticks = 120;

void CMdBroadCast::distribute_tick_function(void* data)
{
	CppThread*		handle_th = static_cast<CppThread*>(data);
	CMdBroadCast*	handle_md = static_cast<CMdBroadCast*>(handle_th->get_data());

	while (!handle_th->is_stop())
	{
		handle_md->distribute_mtk_tick();
	}
}

void CMdBroadCast::calculate_min_function(void* data)
{
	CppThread*		handle_th = static_cast<CppThread*>(data);
	CMdBroadCast*	handle_md = static_cast<CMdBroadCast*>(handle_th->get_data());

	while (!handle_th->is_stop())
	{
		std::for_each(handle_md->subscribe_inst_.begin(), handle_md->subscribe_inst_.end(), [&handle_md](std::string& ins)
		{
			handle_md->calculate_min_bar(ins);
		});
	}
}

void CMdBroadCast::notify_min_data(candle_bar& data)
{
	std::for_each(stg_list_.begin(), stg_list_.end(), [&data](std::shared_ptr<CStrategy>& stg)
	{
		stg->update(data);
	});
}

void CMdBroadCast::attach_observer(CStrategy* stg)
{
	stg_list_.emplace_back(std::shared_ptr<CStrategy>(stg));
}

void CMdBroadCast::dettach_observer(CStrategy* stg)
{
	auto list_it = std::find(stg_list_.begin(), stg_list_.end(), std::shared_ptr<CStrategy>(stg));
	if (list_it != stg_list_.end())
	{
		stg_list_.erase(list_it);
	}
}

bool CMdBroadCast::get_connect_flag()
{
	return connect_flag_;
}

void CMdBroadCast::set_intruments(std::vector<std::string>& ins)
{
	// Initialize Focused Instruments
	subscribe_inst_ = ins;

	// Initialize Map
	for (auto ins : subscribe_inst_)
	{
		// Initial Tick Data Map
		std::vector<mtk_data> tick_vec;
		tick_vec.reserve(256);
		efficient_map_operation(tick_base_, ins, tick_vec);

		// Initial Mutex Lock
		efficient_map_operation(tick_mutex_, ins, std::shared_ptr<std::mutex>(new std::mutex));

		// Initial Market Close Flag
		efficient_map_operation(mtk_open_, ins, false);
	}

	// Initialize Vector
	sub_ticks_.reserve(256);
	sub_mutex_ = std::move(std::shared_ptr<std::mutex>(new std::mutex));
}

size_t	CMdBroadCast::get_instruments_size()
{
	return subscribe_inst_.size();
}

void CMdBroadCast::get_instrument_name(char**& inst_ptr, size_t ins_size)
{
	for (size_t i = 0; i < ins_size; ++i)
	{
		inst_ptr[i] = const_cast<char*>(subscribe_inst_[i].c_str());
	}
}

void CMdBroadCast::distribute_mtk_tick()
{
	std::lock_guard<std::mutex> lck(*sub_mutex_);

	if (!sub_ticks_.empty())
	{
		std::for_each(sub_ticks_.begin(), sub_ticks_.end(), [this](mtk_data& dt)
		{
			tick_base_pushback(dt.InstrumentID, dt);
		});

		sub_ticks_.clear();
	}
}

void CMdBroadCast::calculate_min_bar(std::string ins)
{
	std::lock_guard<std::mutex> lck(*tick_mutex_[ins]);

	bool is_open = mtk_open_[ins];
	if (is_open)
	{
		if (tick_base_[ins].size() > (max_minute_ticks / 2))
		{
			calculate_process(ins, is_open);
		}
	}
	else
	{
		if (!tick_base_[ins].empty())
		{
			calculate_process(ins, is_open);
		}
	}
}

void CMdBroadCast::tick_base_pushback(std::string ins, mtk_data& data)
{
	std::lock_guard<std::mutex> lck(*tick_mutex_[ins]);

	// Check Out If TurnOver Had Changed
	std::vector<mtk_data>& mtk_vec = tick_base_[ins];
	if (mtk_vec.empty())
	{
		mtk_vec.push_back(data);
	}
	else
	{
		if (mtk_vec.back().Volume < data.Volume)
		{
			mtk_vec.push_back(data);
		}
		else if (mtk_vec.back().Volume > data.Volume)
		{
			abort();
		}
	}

	// Set Market Open Flag
	mtk_open_[ins] = check_mtk_time(data.UpdateTime);
}

void CMdBroadCast::calculate_process(std::string ins, bool is_open)
{
	// Guarantee How Many Ticks Should Be Erased.
	int real_tick_nums = 0;

	// Container Of Actual One Minute Ticks
	std::vector<mtk_data> min_ticks;

	// Finding Tail Flag
	bool has_tail = false;

	auto tick_tail = tick_base_[ins].end() - 1;
	for (auto tick = tick_base_[ins].begin(); tick != tick_tail; ++tick)
	{
		real_tick_nums++;

		auto prev_iter = tick;
		auto next_iter = tick + 1;

		has_tail = is_minute_tail(prev_iter->UpdateTime, next_iter->UpdateTime);
		if (has_tail)
		{
			min_ticks.assign(tick_base_[ins].begin(), tick_base_[ins].begin() + real_tick_nums);
			break;
		}
	}

	// Calculate The Integrated Minute Bar
	if (has_tail)
	{
		candle_bar min_bar;
		convert_tick2min(min_ticks, min_bar);
		notify_min_data(min_bar);	// Every Strategy Maintains One Minute Base Data By Itself
		print_info(ins.c_str());

		// Erase Used Data
		tick_base_[ins].erase(tick_base_[ins].begin(), tick_base_[ins].begin() + real_tick_nums);
	}
	// Dump Residuary Half-Baked Ticks After Market Closed
	else if (!has_tail && !is_open)
	{
		tick_base_[ins].clear();
	}
}

size_t  CMdBroadCast::convert_time_str2int(char* update_time)
{
	// Check Out If Market Opened (Opened Time Range Are Gotten From Config File Later)
	std::string market_time_str = update_time;
	market_time_str.erase(std::remove_if(market_time_str.begin(), market_time_str.end(), [](char c){ return c == ':'; }), market_time_str.end());

	return atoi(market_time_str.c_str());
}

bool CMdBroadCast::check_mtk_time(char* update_time)
{
	bool open_flag = false;

	// Check Out If Market Opened (Opened Time Range Are Gotten From Config File Later)
	size_t market_time_int = convert_time_str2int(update_time);
	// IF16XX Open Time Range As An Example
	if ((market_time_int >= 93000 && market_time_int <= 113000) || (market_time_int >= 130000 && market_time_int <= 150000))
	{
		open_flag = true;
	}
	else
	{
		open_flag = false;
	}

	return open_flag;
}

bool CMdBroadCast::is_minute_tail(char* prev_time, char* next_time)
{
	// Verdict Head Or Tail In Data Stream
	size_t prev_t = convert_time_str2int(prev_time) / 100;
	size_t next_t = convert_time_str2int(next_time) / 100;

	if (prev_t < next_t)
	{
		return true;
	}
	else if (prev_t > next_t)
	{
		abort();
	}

	return false;
}

void CMdBroadCast::convert_tick2min(std::vector<mtk_data>& tick_vec, candle_bar& min_bar)
{
	// Get The Both Ends Data
	mtk_data& first_data = tick_vec.front();
	mtk_data& last_data = tick_vec.back();

	// Initial Basic Information
	strncpy_s(min_bar.bar_name, first_data.InstrumentID, sizeof(mtk_data_name));
	strncpy_s(min_bar.bar_ecg, first_data.ExchangeID, sizeof(mtk_data_exchange));

	min_bar.trade_day = atoi(first_data.TradingDay);
	min_bar.open_price = first_data.LastPrice;
	min_bar.close_price = last_data.LastPrice;

	min_bar.volume_size = last_data.Volume - first_data.Volume;

	min_bar.change_point = min_bar.close_price - min_bar.open_price;
	min_bar.change_percent = (min_bar.change_point / min_bar.open_price) * 100;

	min_bar.high_price = min_bar.open_price;
	min_bar.low_price = min_bar.open_price;

	std::for_each(tick_vec.begin(), tick_vec.end(), [&tick_vec, &min_bar](mtk_data& dt)
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
	CGLog::get_glog()->print_log("InstrumentID\t\t  UpdateTime\t\t  TradingDay\t\t  LastPrice\t\t  Volume\t\t  ");

	char data_info[1024] = { 0 };
	for (auto data = tick_vec.begin(); data != tick_vec.end(); ++data)
	{
		sprintf_s(data_info, sizeof(data_info)-1, "%s\t\t  %s\t\t  %s\t\t  %lf\t\t  %d\t\t  ", data->InstrumentID, data->UpdateTime, data->TradingDay, data->LastPrice, data->Volume);
		CGLog::get_glog()->print_log(data_info);
	}
	CGLog::get_glog()->print_log("\r\n");

	// Check Out The Synthetic
	sprintf_s(data_info, sizeof(data_info)-1, "%lf\t\t  %lf\t\t  %lf\t\t  %lf\t\t  %lf\t\t  %lf\t\t  %d\t\t  ", min_bar.open_price, min_bar.close_price, min_bar.high_price, min_bar.low_price, min_bar.change_point, min_bar.change_percent, min_bar.volume_size);
	CGLog::get_glog()->print_log(data_info);
	CGLog::get_glog()->print_log("\r\n");
	//////////////////////////////////////////////////////////////////////////
}

void CMdBroadCast::print_info(const char* ins_str)
{
	std::cout << ins_str << std::endl;
}

bool CMdBroadCast::initial_md_broadcast(std::vector<std::string>& ins)
{
	bool md_ret = true;

	// Initialize Interested Instruments
	set_intruments(ins);

	// Start Distribution Thread
	distribute_thread_.set_data(this);
	md_ret = distribute_thread_.create_thread(distribute_tick_function);

	// Start Calculation Thread
	calculate_thread_.set_data(this);
	md_ret = calculate_thread_.create_thread(calculate_min_function);

	// Start MD Front-End Interface
	md_api_ = CThostFtdcMdApi::CreateFtdcMdApi("./spi_con/"); // 'spi_con' has to already been prepared in advance
	if (md_api_ != nullptr)
	{
		md_api_->RegisterSpi(this);
		md_api_->RegisterFront("tcp://180.168.146.187:10010");
		md_api_->Init();
	}
	else
	{
		md_ret = false;
	}

	return md_ret;
}

void CMdBroadCast::release_md_broadcast()
{
	// Release MD Front-End Interface
	if (md_api_ != nullptr)
	{
		md_api_->RegisterSpi(nullptr);
		md_api_->Release();
		md_api_ = nullptr;
	}

	// Close Distribution Thread
	distribute_thread_.set_stop(true);
	distribute_thread_.close_thread();

	// Close Calculation Thread
	calculate_thread_.set_stop(true);
	calculate_thread_.close_thread();
}

bool CMdBroadCast::get_ready_subscribe()
{
	return is_ready_subscribe_;
}

bool CMdBroadCast::subscribe_instruments(char* *needed_ins, int ins_count)
{
	bool sub_ret = true;

	if (is_ready_subscribe_)
	{
		md_api_->SubscribeMarketData(needed_ins, ins_count);
	}
	else
	{
		sub_ret = false;
	}

	return sub_ret;
}

/*****************************spi»Øµ÷º¯Êý\***********************************************/
void CMdBroadCast::OnFrontConnected() 
{
	CThostFtdcReqUserLoginField loginField;
	strncpy_s(loginField.BrokerID,	"", sizeof(loginField.BrokerID) - 1);
	strncpy_s(loginField.UserID,	"",	sizeof(loginField.UserID) - 1);
	strncpy_s(loginField.Password,	"",	sizeof(loginField.Password) - 1);

	md_api_->ReqUserLogin(&loginField, 0);

	connect_flag_ = true;
}

void CMdBroadCast::OnFrontDisconnected(int nReason)
{
	connect_flag_ = false;
}

void CMdBroadCast::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	if (pRspInfo->ErrorID == 0)
	{
		is_ready_subscribe_ = true;
	}
	else
	{
		abort();
	}
}


void CMdBroadCast::OnRtnDepthMarketData(mtk_data *pDepthMarketData)
{
	// Refuse To Block Interface CallBack
	std::lock_guard<std::mutex> lck(*sub_mutex_);
	sub_ticks_.emplace_back(*pDepthMarketData);
}