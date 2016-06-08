#pragma once

#include "PublicDataStruct.h"

struct archive_data
{
	int period{ 0 };
	candle_bar data;
};

class CSaveData
{
public:
	void set_root_dir(std::string root);

	bool open_save_data(std::string& stg);
	void close_save_data();

	void write_save_data(archive_data& data);

private:
	std::string  root_dir_;
	std::fstream save_file_;
};