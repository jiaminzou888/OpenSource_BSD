#pragma once

#include "ThostFtdcMdApi.h"

#include "PublicDataStruct.h"
#include "CppThread.hpp"


class CMdManager;  // Forward Declaration For Management Pointer

// Only Be Responsible For One Minute KData Calculation.
class CMdBroadCast : private CThostFtdcMdSpi
{
// Observer Pattern Demand
public:
	CMdBroadCast(CMdManager* manage_ptr);

	bool	get_md_connect_flag();

	void	set_intruments(std::vector<std::string>& ins);
	size_t	get_instruments_size();
	void	get_instrument_name(char**& inst_ptr, size_t ins_size);
	std::vector<std::string> get_instruments();

	static void distribute_tick_function(void* data);
	void distribute_mtk_tick();

	static void calculate_min_function(void* data);
	void calculate_min_bar(std::string ins);

	void	get_min_kdata(std::string ins, std::vector<candle_bar>& bars);

private:
	void tick_base_pushback(std::string ins, mtk_data& data);
	void calculate_process(std::string ins, bool is_open);

	size_t  convert_time_str2int(char* update_time);
	bool	check_mtk_time(char* update_time);
	bool	is_minute_tail(char* prev_time, char* next_time, size_t& td_time);
	void	convert_tick2min(std::vector<mtk_data>& tick_vec, candle_bar& min_bar);

	size_t  get_min_size(std::string ins);
	void	push_min_kdata(std::string ins, candle_bar& data);
	void	print_info(const char* ins_str);

private:
	bool	connect_md_flag_{ false };
	
	std::vector<std::string>		subscribe_inst_;	// Subscribed Instruments
	std::map<std::string, bool>		mtk_open_;			// Market Open Flag

	CppThread		distribute_thread_;					// Tick Data Distribution
	std::shared_ptr<std::mutex>		sub_mutex_;			// sub_ticks_ Lock
	std::vector<mtk_data>			sub_ticks_;			// Subscribed MD Data

	CppThread		calculate_thread_;					// Tick Data Calculation Consumer
	std::map<std::string, std::shared_ptr<std::mutex>>		tick_mutex_;		// tick_base_ Lock
	std::map<std::string, std::vector<mtk_data>>			tick_base_;			// Ticks For Base Data Calculation 

	std::shared_ptr<CMdManager>		manager_pointer;	// Store Minute Data Management

#ifndef _TRADE_TIME_
	// Fake Function
	bool initial_fake_tick_data();
	void release_fake_tick_data();

	void transfer_fake_to_tick_data(int& data_index, int& total_volum, mtk_data& tick);
	static void distribute_fake_tick_function(void* data);

	// Fake Data
	std::map<int, std::string> instrument_tick_one_day;
	CppThread	distribute_fake_thread;
#endif	// _TEST_CODE_

// MD Interface Business
public:
	bool initial_md_broadcast(std::vector<std::string>& ins);
	void release_md_broadcast();

	bool get_ready_subscribe();
	bool subscribe_instruments(char* *needed_ins, int ins_count);

private:
	//当客户端与交易后台建立起通信连接时，即还未登录前，该方法被调用
	void OnFrontConnected() override;
	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	void OnFrontDisconnected(int nReason) override;
	//登录请求响应
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
	//行情回报响应(Tick Data Producer)
	void OnRtnDepthMarketData(mtk_data *pDepthMarketData) override;

private:
	CThostFtdcMdApi* md_api_{ nullptr };
	bool is_ready_subscribe_{ false };
};