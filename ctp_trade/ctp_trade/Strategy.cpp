#include "Strategy.h"

#include <string>
#include <fstream>
#include <algorithm>

#include "EfficientMap.h"

bool CStrategy::init(std::string config_path, std::string config_head)
{
	bool init_ret = true;

	// Read Interested Instruments From Config File
	init_ret &= load_focused_inst(config_path, config_head);
	
	// Other Operation
	// .

	return init_ret;
}

void CStrategy::update(candle_bar& data)
{
	std::string ins = data.bar_name;
	if (min_base_.find(ins) != min_base_.end())
	{
		min_base_[ins].push_bar(data);
	}
}

bool CStrategy::load_focused_inst(std::string& config_path, std::string& config_head)
{
	bool cfg_ret = false;

	std::ifstream config_file(config_path);

	if (!config_file.is_open())
	{
		return cfg_ret;
	}

	std::string string_line;
	while (getline(config_file, string_line))
	{
		if (std::string::npos != string_line.find(config_head))
		{
			if (std::string::npos != string_line.find_first_of("="))
			{
				// Initial One Minute Data Map
				CCandleBar min_bar;

				// Trim Space
				string_line.erase(std::remove_if(string_line.begin(), string_line.end(), [](char c){return c == ' '; }), string_line.end());

				// Position Instruments
				size_t ins_pos = string_line.find_first_of("=");

				size_t beg_pos = ins_pos + 1;
				ins_pos = string_line.find_first_of("|", beg_pos);
				while (std::string::npos != ins_pos)
				{
					efficient_map_operation(min_base_, string_line.substr(beg_pos, ins_pos - beg_pos), min_bar);
					
					beg_pos = ins_pos + 1;
					ins_pos = string_line.find_first_of("|", beg_pos);
				}
				if (ins_pos > beg_pos)
				{
					efficient_map_operation(min_base_, string_line.substr(beg_pos, ins_pos - beg_pos), min_bar);
				}

				cfg_ret = true;
				break;
			}
			else
			{
				break;
			}
		}
		else
		{
			continue;
		}
	}

	return cfg_ret;
}

//////////////////////////////////////////////////////////////////////////
bool CMAStrategy::init_ma_stg()
{
	for_each(min_base_.begin(), min_base_.end(), [this](std::map<std::string, CCandleBar>::reference& node)
	{
		CMovingAverage ma(node.second);
		efficient_map_operation(data_ma_, node.first, ma);
	});

	return true;
}
