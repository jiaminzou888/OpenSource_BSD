#pragma once

#include "CandleBar.h"

#include <string>
#include <map>



// Every Strategy Has Its Own Copy Of Focused Instruments Min KData As The Base Data, 
// Which Is Used To Calculate Indicators That Is For Trading In Real-time.  

class CStrategy
{
public:
	CStrategy();
	virtual ~CStrategy() = default;
	
	bool init_stg(std::string config_path, std::string config_head);
	void update(candle_bar& min_data);

protected:
	std::string config_head_;
	std::vector<std::string>	focused_inst_;		// Target Instruments
	std::map<std::string, CCandleBar>	min_data_;	// One Minute K Data

private:
	bool load_focused_inst(std::string& config_path, std::string& config_head);
};

// Moving Average Strategy
class CMAStrategy : public CStrategy	
{
public:
	
private:
	
};