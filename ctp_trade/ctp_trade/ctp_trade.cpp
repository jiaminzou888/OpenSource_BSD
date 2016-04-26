// ctp_trade.cpp : 定义控制台应用程序的入口点。

#include "MdManager.h"
#include <time.h>

int main(int argc, char* argv[])
{
	CMdManager mana;
	mana.initial_md_manager();

	std::vector<std::string> vect;
	vect.push_back("IF1605");
	vect.push_back("TF1606");

	mana.subscribe_market(vect);
	mana.open_consumer_thread();

	CMAStrategy mas(vect);
	mana.attach_md_strategy(&mas);

	while (true)
	{
		int i = 0;
	}

	mana.close_consumer_thread();
	mana.release_md_manager();

	return 0;
}

