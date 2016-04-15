#pragma once

#include "SolveReDefinition.h"
// Has To Be Behind 'SolveReDefinition.h'
#include <vector>
#include <string>
#include <map>
#include <mutex>
#include <memory>

#include "ThostFtdcUserApiStruct.h"
#include "CandleBar.h"

using instr_container = std::vector<std::string>;

class CMdBroadCast;	// Prerequisite Declaration

class CStrategy
{
public:
	CStrategy() = default;
	CStrategy(instr_container instr);

	void	update(mtk_data* data);
	size_t	tick_base_getsize(std::string ins);
	void	tick_base_pushback(std::string ins, mtk_data& data);
	void	calculate_min_bar(std::string ins);
	const instr_container& get_focused_instr() const;

	virtual ~CStrategy() = default;
	virtual void print_info(const char* ins_str);

protected:
	instr_container	focused_inst_;
	std::map<std::string, std::shared_ptr<std::mutex>>		tick_mutex_;
	std::map<std::string, std::vector<mtk_data>>			tick_base_;	// Tick Data For kdata_ Calculation 
	std::map<std::string, std::map<k_types, CCandleBar>>	kdata_;

private:
	void calculate_process(std::string ins);
	void convert_tick2min(std::string ins, candle_bar& min_bar);
};

// Moving Average Strategy
class CMAStrategy : public CStrategy	
{
public:
	CMAStrategy(instr_container instr);

	void print_info(const char* ins_str) override;
	
};