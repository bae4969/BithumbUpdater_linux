
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

						Bithumb Updater ver.1.0.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			IDE : Visual Studio 2019
			Lng : c++
			Inc : bithumb api, MySQL

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Can List 

		1. It gets bithumb public transaction data and save it at MySql
		server.
	
		2. It makes one minute, 10 minute, 30 minute, an hour, four hour,
		one day, week and month candle data from transaction data.

		3. For reduce delay, transaction history data from an hour ago are
		moved to the long-term storage area.


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	How to execute
  
		1. Install MySql.

		2. make sql_info.h and api_info.h whitch saves api key and sql idpw

		ex)

			//sql_info.h

			#define SQL_ADDR (your linux ip)
			#define SQL_ID (your sql id)
			#define SQL_PW (your sql pw)
			#define SQL_DB (your sql main schema)
			#define SQL_SAVE (your sql data save schema)
			#define SQL_LIST (your api update list table)
			#define SQL_PORT (your sql port number)

			//api_info.h

			#define _API_HOST "https://api.bithumb.com"
			#define _API_KEY (api public key)
			#define _API_SECRET (api secret key)

		3. build with visual studio or use Make


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
