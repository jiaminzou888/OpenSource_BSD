#pragma once

#include "TechniqueIndicator.h"
#include "SavaData.h"
#include "CppThread.hpp"


// Every Strategy Has Its Own Copy Of Focused Instruments Min KData As The Base Data, 
// Which Is Used To Calculate Indicators That Is For Trading In Real-time.  

class CStrategy
{
public:
	CStrategy() = default;
	virtual ~CStrategy()= default;
	
	bool initial_stg(std::string config_path, std::string config_head, int k_type);
	void release_stg();

	void update(int period, candle_bar& stg_data);

protected:
	int	decision_data_type{ 0 };

	std::string config_head_;
	std::map<int, std::map<std::string, CCandleBar>> decision_data_;			// Period-Cycle Data That The Strategy Needs. 
																				// int: Period-Cycle, map: Focused Instruments That On 'int' Periods
																				// 'int' are corresponding to CCandleBar::Min_ONE etc.
	CSaveData save_min_data;

private:
	bool load_focused_inst(std::string& config_path, std::string& config_head, int k_type);
	void construct_decision_data(std::string ins, int k_type);
};

// Moving Average Strategy
class CMAStrategy : public CStrategy	
{
public:
	static void calculate_indicator_function(void* data);

	bool initial_ma_stg(std::string config_path, std::string config_head, int k_type);
	void release_ma_stg();

private:
	void trigger_signal_action(int action, int action_reason);

private:
	std::map<int, std::map<std::string, int>> calculate_position_flag_;			// Moving Average 'date_index' Flag	
	std::map<int, std::map<std::string, CMovingAverage>> decision_tech_ma_;		// Moving Average Base On CandleBar

	CppThread indicator_thread_;
};