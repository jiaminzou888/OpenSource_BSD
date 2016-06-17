#pragma once

#include "CandleBar.h"

class CTechniqueIndicator
{
public:
	CTechniqueIndicator(CCandleBar& data);
	virtual ~CTechniqueIndicator();

	// basic
	bool valid_tech_signal(int signal);
	bool access_calculate_parameters(int date_index, int periods);
	
	// utility
	int get_fork_signal(int date_index, std::vector<int> period_cycle, int type, int gold_fork, int dead_fork, int& action_reason);
	int get_trend_signal(int date_index, std::vector<int> period_cycle, int type, int dir_long, int dir_short, int& action_reason);

	// attribute
	virtual bool is_valid_parameters();
	virtual void set_default_parameters();
	virtual void attach_parameters(tech_attribute* attr);

	// operation
	virtual int  get_signal(int type, int& reason);
	virtual bool get_minmax_info(int beg_date, int end_date, double& min_data, double& max_data);
	

	// calculation
	virtual bool calculate(int date_index, int periods, int type, bool use_last, double& data); // Moving Average

protected:
	std::shared_ptr<CCandleBar> base_kdata;
};


class CMovingAverage : public CTechniqueIndicator
{
public:
	CMovingAverage(CCandleBar& data);
	~CMovingAverage();

	enum
	{
		TYPE_MA		= 0x01,
		TYPE_MA_EXP = 0x02
	};

	// attributes
	bool is_valid_parameters()					 override;
	void set_default_parameters()				 override;
	void attach_parameters(tech_attribute* attr) override;
	
	// operation
	int  get_signal(int type, int& reason)									override;
	bool get_minmax_info(int beg_date, int end_date, double& min_data, double& max_data)	override;
	
	// calculation
	bool calculate(int date_index, int periods, int type, bool use_last, double& data)      override;
	
	// utility
	bool is_basic_data_ready();
	double get_limit_price(int type);

private:
	bool	initial_read_pos_flag{ false };
	int		candle_bar_read_pos{ 0 };

	ma_attribute attribute;
};

