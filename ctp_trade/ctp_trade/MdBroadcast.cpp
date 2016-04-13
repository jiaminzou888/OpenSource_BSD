#include "MdBroadcast.h"


void	CMdBroadCast::notify_mtk_data(mtk_data* data)
{
	for (auto strategy : list_)
	{
		instr_container ins = strategy->get_focused_instr();
		auto ins_it = std::find(ins.begin(), ins.end(), data->InstrumentID);
		if (ins_it != ins.end())
		{
			strategy->update(data);
		}
	}
}

void	CMdBroadCast::attach_observer(CStrategy* stg)
{
	list_.emplace_back(std::shared_ptr<CStrategy>(stg));
}

void	CMdBroadCast::dettach_observer(CStrategy* stg)
{
	auto list_it = std::find(list_.begin(), list_.end(), std::shared_ptr<CStrategy>(stg));
	if (list_it != list_.end())
	{
		list_.erase(list_it);
	}
}

const  strategy_list& CMdBroadCast::get_strategy_list() const
{
	return list_;
}

bool CMdBroadCast::initial_md_broadcast()
{
	bool md_ret = true;

	md_api_ = CThostFtdcMdApi::CreateFtdcMdApi("./spi_con/"); // 'spi_con' has to already been prepared in advance
	if (md_api_ != nullptr)
	{
		md_api_->RegisterSpi(this);
		md_api_->RegisterFront("tcp://180.168.146.187:10010");
		md_api_->Init();
	}
	else
	{
		md_ret = false;
	}

	return md_ret;
}

void CMdBroadCast::release_md_broadcast()
{
	if (md_api_ != nullptr)
	{
		md_api_->RegisterSpi(nullptr);
		md_api_->Release();
		md_api_ = nullptr;
	}
}

bool CMdBroadCast::get_ready_subscribe()
{
	return is_ready_subscribe_;
}

bool CMdBroadCast::subscribe_instruments(char* *needed_ins, int ins_count)
{
	bool sub_ret = true;

	if (is_ready_subscribe_)
	{
		md_api_->SubscribeMarketData(needed_ins, ins_count);
	}
	else
	{
		sub_ret = false;
	}

	return sub_ret;
}


/*****************************spi»Øµ÷º¯Êý\***********************************************/
void CMdBroadCast::OnFrontConnected() 
{
	CThostFtdcReqUserLoginField loginField;
	strncpy_s(loginField.BrokerID,	"", sizeof(loginField.BrokerID) - 1);
	strncpy_s(loginField.UserID,	"",	sizeof(loginField.UserID) - 1);
	strncpy_s(loginField.Password,	"",	sizeof(loginField.Password) - 1);

	md_api_->ReqUserLogin(&loginField, 0);
}


void CMdBroadCast::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	if (pRspInfo->ErrorID == 0)
	{
		is_ready_subscribe_ = true;
	}
	else
	{
		abort();
	}
}


void CMdBroadCast::OnRtnDepthMarketData(mtk_data *pDepthMarketData)
{
	// I  Prefer To Store Market Data Directly Here Than To Do Any Process, Especially Time-Consuming One.
	// Using Producer And Consumer Pattern and Do As The Only One Producer.
	notify_mtk_data(pDepthMarketData);
}