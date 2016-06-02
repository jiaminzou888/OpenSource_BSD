#pragma once

#include "TechniqueIndicator.h"
#include "SavaData.h"


// Every Strategy Has Its Own Copy Of Focused Instruments Min KData As The Base Data, 
// Which Is Used To Calculate Indicators That Is For Trading In Real-time.  

class CStrategy
{
public:
	CStrategy() = default;
	virtual ~CStrategy()= default;
	
	bool initial_stg(std::string config_path, std::string config_head, int k_type);
	void release_stg();

	void update(candle_bar& stg_data);

protected:
	int	decision_data_type{ 0 };

	std::string config_head_;
	std::map<int, std::map<std::string, CCandleBar>> decision_data_;			// Period-Cycle Data That The Strategy Needs

	CSaveData save_min_data;

private:
	bool load_focused_inst(std::string& config_path, std::string& config_head, int k_type);
	void construct_decision_data(std::string ins, int k_type);
};

// Moving Average Strategy
class CMAStrategy : public CStrategy	
{
public:
	bool initial_ma_stg(std::string config_path, std::string config_head, int k_type);
	void release_ma_stg();

private:
	std::map<int, std::map<std::string, CMovingAverage>> decision_tech_ma_;		// Moving Average Base On CandleBar
};