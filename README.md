# OpenSource_BSD
OpenSource_BSD Is Some Of My Part-time Projects Which Based On MS-VS2013. All Projects Follow BSD License.
## Introduction To The Repository:
###1. ctp_trade: 
The Project Intends To Became A Minimal Platform That Would Support Everyone Who Wants To Test Their Own Trade Strategies Temporarily And The First Version Was Released On April 8th. 2016. I Am Hardly Looking Forward Everybody, Who Plans To Devote To Quant Career But Now Has No Proper Opportunity To Make It Come True, To Join Me As Soon As Possible. I Awfully Believe That We Could Help All Guys To Achieve Their Dreams In The Future.
#####Release Note:
1. Set Up REDIS 3.0 Database Environment In MS-VS2013 And Connected CTP-MD Interfaces To It Successfully. I Have To Say "Calling REDIS Interfaces In Windows Platform, Especially In MS-VS2013, Is One Of The Most Trivial Jobs I've Done Before!". —— **April 8th. 2016.**
2. Completed One Minute K-Line Calculation Without Consideration Of Missing Market Data, And I've Given Up REDIS Cache Method Temporarily Because The Small Data Volume I Just Need To Focus On Now.  —— **April 13th. 2016.**
3. Add GLog Module For Supporting Debug Log Output to Check Out KData Calculation Temporarily.  —— **April 15th. 2016.**
4. Modified And Completed The One Minute Bar As The Basis Of All Strategies' Data.  —— **May 5th. 2016.**
5. Completed The First Edition Of Moving Average Indicator Calculation Class For The MA-Strategy.  —— **May 28th. 2016.**
6. Reconstruct And Optimize The Trade Architecture; Access To TD Interface.  —— **JUN 2nd. 2016.**
7. Complete All Different Minute's Period KData Calculation And Push To Registered Strategies, Then Combine MA_Indicator With MA_Strategy And Try To Do Corresponding Action Based On It's Reason.  —— **JUN 8th. 2016.**
8. Complete Faked rb1610 Data Push And Solved The Problem Of Daytime Function Test Only In Trading Day And Could Send Order Correctly Now.   —— **JUN 14th. 2016.**
9. **Pause This Public Project To Try To Construct A Python Backward Test Application By Zipline Frame .**  ——  **JUN 23th. 2016.**

###2. zipline_application: 
The Project Aims To Figure Out How To Apply Zipline To Chinese Stock Market And Construct An Useful Small Zipline Application.
#####Release Note:
1. Downloaded And Installed Anaconda_64bit And CMD "conda install -c Quantopian zipline" To Install Zipline Package. ——  **JUN 28th. 2016.**
2. Complicated A Zipline's Demo Sample For Localizing Its Trade Simulation System By Hard Disk Data.**  ——  **JUL 1st. 2016.**

###3. main_trade:
The Project Intends To Rewrite VNPY In Python To Main_Trade In C++ Which Just Support CTP Temporarily, And We Plans To Open It Later When Finishing The CTA Engine.
#####Release Note:
1. Completed The First Version Trading System, In Which You Can Trade CTP's Instruments Manually. A little Bit Of Exciting Actually, AHa! **  ——  **JUL 26st. 2016.**

