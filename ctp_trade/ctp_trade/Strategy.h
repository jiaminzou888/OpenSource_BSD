#pragma once

#include "TechniqueIndicator.h"
#include "SavaData.h"
#include "CppThread.hpp"

class CTdManager; // Forward Declaration For Management Pointer

// Every Strategy Has Its Own Copy Of Focused Instruments Min KData As The Base Data, 
// Which Is Used To Calculate Indicators That Is For Trading In Real-time.  

class CStrategy
{
public:
	CStrategy() = default;
	virtual ~CStrategy() = default;
	
	bool initial_stg(std::string config_path, std::string config_head, int k_type);
	void release_stg();

	void set_td_module(CTdManager* td_ptr);
	void update(int period, candle_bar& stg_data);

	virtual void trigger_signal_action(std::string ins, int action, int action_reason, double action_price) = 0;	// Pure Virtual Mechanism That Notify TradeManager To Insert Specified Order

protected:
	int	decision_data_type{ 0 };

	std::string config_head_;
	std::map<int, std::map<std::string, CCandleBar>> decision_data_;// Period-Cycle Data That The Strategy Needs. 
																	// int: Period-Cycle, map: Focused Instruments That On 'int' Periods
																	// 'int' Are Corresponding To CCandleBar::Min_ONE etc.

	std::shared_ptr<CTdManager>		trade_pointer_;	// Trade Management

	CSaveData save_min_data_;						// Data Save Module

private:
	bool load_focused_inst(std::string& config_path, std::string& config_head, int k_type);
	void construct_decision_data(std::string ins, int k_type);
};

// Moving Average Strategy
class CMAStrategy : public CStrategy	
{
public:
	static void strategic_decision_function(void* data);

	bool initial_ma_stg(std::string config_path, std::string config_head, int k_type);
	void release_ma_stg();

	void trigger_signal_action(std::string ins, int action, int action_reason, double action_price);

private:
	std::map<int, std::map<std::string, CMovingAverage>> decision_tech_ma_;		// Moving Average Base On CandleBar

	CppThread		indicator_thread_;
};