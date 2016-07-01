#! Users\Administrator\Documents\Python Scripts
#  coding = utf-8

import pytz
from datetime import datetime

from zipline.algorithm import TradingAlgorithm
from zipline.finance.trading import TradingEnvironment
from zipline.api import order, record, symbol
from zipline.finance import trading
from zipline.utils.factory import create_simulation_parameters
from zipline.assets.synthetic import make_simple_equity_info

import pandas as pd
import numpy as np

# Define algorithm
def initialize(context):
    pass


def handle_data(context, data):
    order(symbol('AAPL'), 10)
    record(AAPL=data.current(symbol('AAPL'), 'Close'))


# This is needed to handle the correct calendar. Assume that market data has the right index for trade able days.
# Passing in env_trading_calendar=trading calendar_lse doesn't appear to work, as it doesn't implement open_and_closes
def load_t(trading_day, trading_days, bm_symbol):
    # dates = pd.date_range('2001-01-01 00:00:00', periods=365, tz="Asia/Shanghai")
    bm = pd.Series(data=np.random.random_sample(len(trading_days)), index=trading_days)
    tr = pd.DataFrame(data=np.random.random_sample((len(trading_days), 7)), index=trading_days,
                      columns=['1month', '3month', '6month', '1year', '2year', '3year', '10year'])
    return bm, tr
'''
equities_t = make_simple_equity_info(sids=[6001, 6002], start_date=pd.to_datetime("2001-01-01 00:00:00"),
                                     end_date=pd.to_datetime("2002-01-01 00:00:00"), symbols=['AA', 'CC'])
exchanges_t = pd.DataFrame({'exchange': ['HS_SZ'], 'timezone': ['Asia/Shanghai']})
'''
trading.environment = TradingEnvironment(load=load_t, bm_symbol='^HSI', exchange_tz='Asia/Shanghai')
'''trading.environment.write_data(equities=equities_t, exchanges=exchanges_t)'''

# Bug in code doesn't set tz if these are not specified
# (finance/trading.py:SimulationParameters.calculate_first_open[close])
# .tz_localize("Asia/Shanghai").tz_convert("UTC")
a = pd.to_datetime("2001-01-01 00:00:00").tz_localize("Asia/Shanghai")
sim_params = create_simulation_parameters(
    start=pd.to_datetime("2001-01-01 00:00:00").tz_localize("Asia/Shanghai"),
    end=pd.to_datetime("2001-09-21 00:00:00").tz_localize("Asia/Shanghai"),
    data_frequency="daily", emission_rate="daily", env=trading.environment)

algor_obj = TradingAlgorithm(initialize=initialize, handle_data=handle_data,
                             sim_params=sim_params, env=trading.environment)

# Run algorithm
parse = lambda x: pytz.utc.localize(datetime.strptime(x, '%Y/%m/%d'))
data_s = pd.read_csv('AAPL.csv', parse_dates=['Date'], index_col=0, date_parser=parse)
data_c = pd.Panel({'AAPL': data_s})

perf_manual = algor_obj.run(data_c)
# Print
perf_manual.to_csv('output.csv')
