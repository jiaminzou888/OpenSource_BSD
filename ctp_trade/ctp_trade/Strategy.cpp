#include "Strategy.h"

#include <string>
#include <algorithm>

#include "EfficientMap.h"

bool CStrategy::initial_stg(std::string config_path, std::string config_head, int k_type)
{
	bool init_ret = true;

	// Read Interested Instruments From Config File
	init_ret &= load_focused_inst(config_path, config_head, k_type);
	
	save_min_data.set_root_dir(config_path.substr(0, config_path.find_last_of("\\")));
	init_ret &= save_min_data.open_save_data(config_head);

	// Other Operation
	// .

	return init_ret;
}

void  CStrategy::release_stg()
{
	save_min_data.close_save_data();
}

void CStrategy::update(candle_bar& stg_data)
{
	std::string ins = stg_data.bar_name;
	if (decision_data_[CCandleBar::MIN_ONE].find(ins) != decision_data_[CCandleBar::MIN_ONE].end())
	{
		save_min_data.write_save_data(stg_data);
		decision_data_[CCandleBar::MIN_ONE][ins].push_bar(stg_data);
	}
}

bool CStrategy::load_focused_inst(std::string& config_path, std::string& config_head, int k_type)
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
				// Trim Space
				string_line.erase(std::remove_if(string_line.begin(), string_line.end(), [](char c){return c == ' '; }), string_line.end());

				// Position Instruments
				size_t ins_pos = string_line.find_first_of("=");

				size_t beg_pos = ins_pos + 1;
				ins_pos = string_line.find_first_of("|", beg_pos);
				while (std::string::npos != ins_pos)
				{
					construct_decision_data(string_line.substr(beg_pos, ins_pos - beg_pos), k_type);
					
					beg_pos = ins_pos + 1;
					ins_pos = string_line.find_first_of("|", beg_pos);
				}
				if (ins_pos > beg_pos)
				{
					construct_decision_data(string_line.substr(beg_pos, ins_pos - beg_pos), k_type);
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

void CStrategy::construct_decision_data(std::string ins, int k_type)
{
	if (k_type & CCandleBar::MIN_ONE)
	{
		CCandleBar k_data;
		k_data.set_candle_type(CCandleBar::MIN_ONE);

		std::map<std::string, CCandleBar> instr_kdata;
		if (!decision_data_[CCandleBar::MIN_ONE].empty())
		{
			instr_kdata = decision_data_[CCandleBar::MIN_ONE];
		}

		efficient_map_operation(instr_kdata, ins, k_data);
		efficient_map_operation(decision_data_, CCandleBar::MIN_ONE, instr_kdata);
	}

	if (k_type & CCandleBar::MIN_FIVE)
	{
		CCandleBar k_data;
		k_data.set_candle_type(CCandleBar::MIN_FIVE);

		std::map<std::string, CCandleBar> instr_kdata;
		if (!decision_data_[CCandleBar::MIN_FIVE].empty())
		{
			instr_kdata = decision_data_[CCandleBar::MIN_FIVE];
		}

		efficient_map_operation(instr_kdata, ins, k_data);
		efficient_map_operation(decision_data_, CCandleBar::MIN_FIVE, instr_kdata);
	}

	if (k_type & CCandleBar::MIN_FIVETEEN)
	{
		CCandleBar k_data;
		k_data.set_candle_type(CCandleBar::MIN_FIVETEEN);

		std::map<std::string, CCandleBar> instr_kdata;
		if (!decision_data_[CCandleBar::MIN_FIVETEEN].empty())
		{
			instr_kdata = decision_data_[CCandleBar::MIN_FIVETEEN];
		}

		efficient_map_operation(instr_kdata, ins, k_data);
		efficient_map_operation(decision_data_, CCandleBar::MIN_FIVETEEN, instr_kdata);
	}

	if (k_type & CCandleBar::MIN_THIRTY)
	{
		CCandleBar k_data;
		k_data.set_candle_type(CCandleBar::MIN_THIRTY);

		std::map<std::string, CCandleBar> instr_kdata;
		if (!decision_data_[CCandleBar::MIN_THIRTY].empty())
		{
			instr_kdata = decision_data_[CCandleBar::MIN_THIRTY];
		}

		efficient_map_operation(instr_kdata, ins, k_data);
		efficient_map_operation(decision_data_, CCandleBar::MIN_THIRTY, instr_kdata);
	}
}

//////////////////////////////////////////////////////////////////////////
bool CMAStrategy::initial_ma_stg(std::string config_path, std::string config_head, int k_type)
{
	// Initial Instrument Candles On Different KType
	initial_stg(config_path, config_head, k_type);

	// Initial Moving Average Indicator On Different KType
	for_each(decision_data_.begin(), decision_data_.end(), [this](std::map<int, std::map<std::string, CCandleBar>>::reference& data_node)
	{
		int candle_type = data_node.first;
		std::map<std::string, CCandleBar>& ins_candles = data_node.second;

		for_each(ins_candles.begin(), ins_candles.end(), [this, &candle_type](std::map<std::string, CCandleBar>::reference& candle_node)
		{
			CMovingAverage ma(candle_node.second);

			std::map<std::string, CMovingAverage> ma_map;
			if (!decision_tech_ma_[candle_type].empty())
			{
				ma_map = decision_tech_ma_[candle_type];
			}

			efficient_map_operation(ma_map, candle_node.first, ma);
			efficient_map_operation(decision_tech_ma_, candle_type, ma_map);
		});
	});

	return true;
}

void CMAStrategy::release_ma_stg()
{
	release_stg();
}