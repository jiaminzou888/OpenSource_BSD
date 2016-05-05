// ctp_trade.cpp : 定义控制台应用程序的入口点。

#include "MdManager.h"
#include <time.h>

int main(int argc, char* argv[])
{
	// Load Interested Instruments
	std::vector<std::string> vect;
	vect.push_back("IF1605");
	vect.push_back("j1701");
	vect.push_back("rb1610");
	vect.push_back("CF609");

	CMdManager mana;

	mana.initial_md_manager(vect);
	mana.subscribe_market();

	CMAStrategy mas;

	mas.init_stg("E:/OpenSource_BSD/ctp_trade/x64/Debug/inst_config.ini", "MA_STG");

	mana.attach_md_strategy(&mas);

	while (mana.get_md_conncet_status())
	{
		int i = 0;
	}

	mana.release_md_manager();

	return 0;
}

