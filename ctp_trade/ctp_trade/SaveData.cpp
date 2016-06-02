#include "SavaData.h"

#include <algorithm>

void CSaveData::set_root_dir(std::string root)
{
	root_dir_ = root;
}

bool CSaveData::open_save_data(std::string& stg)
{
	time_t now_time = { 0 };
	time(&now_time);
	tm now_date = { 0 };
	localtime_s(&now_date, &now_time);

	int date_day = (1900 + now_date.tm_year) * 10000 + (now_date.tm_mon + 1) * 100 + now_date.tm_mday;

	char data_file[256] = { 0 };
	sprintf_s(data_file, "\\%s_%d", stg.c_str(), date_day);
	std::string data_path = root_dir_ + data_file;
	save_file_.open(data_path, std::ios_base::in | std::ios_base::out | std::ios_base::app);

	return true;
}

void CSaveData::close_save_data()
{
	save_file_.close();
}

void CSaveData::write_save_data(candle_bar& data)
{
	save_file_.write((char*)&data, sizeof(candle_bar));
}