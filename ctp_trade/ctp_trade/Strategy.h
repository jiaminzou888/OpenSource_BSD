#pragma once

#include "CandleBar.h"
#include "TechniqueIndicator.h"

// Every Strategy Has Its Own Copy Of Focused Instruments Min KData As The Base Data, 
// Which Is Used To Calculate Indicators That Is For Trading In Real-time.  

class CStrategy
{
public:
	CStrategy() = default;
	virtual ~CStrategy() = default;
	
	bool init(std::string config_path, std::string config_head);
	void update(candle_bar& min_data);

protected:
	std::string config_head_;
	std::map<std::string, CCandleBar>	min_base_;		// One Minute K Data

private:
	bool load_focused_inst(std::string& config_path, std::string& config_head);
};

// Moving Average Strategy
class CMAStrategy : public CStrategy	
{
public:
	bool init_ma_stg();


private:
	std::map<std::string, CMovingAverage>	data_ma_;	// Moving Average Base On CandleBar
};