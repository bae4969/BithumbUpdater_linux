OUT = BUd.out

DIR = BithumbUpdater
OBJS = $(DIR)/obj/main.o $(DIR)/obj/api.o $(DIR)/obj/sql.o $(DIR)/obj/converter.o

INC = -I/usr/include -I$(shell pwd)/$(DIR)/inc
LIB64 = -L/usr/lib64 -L/usr/lib64/mysql
BIN = -pthread -lmysqlclient -lcurl -lcrypto



all : build
rebuild : clean build


build : api.o sql.o converter.o main.o
	g++ $(INC) $(LIB64) $(BIN) -o $(OUT) $(OBJS)
	
clean :
	rm -rf obj/*.o
	rm -rf $(OUT)


main.o :
	g++ $(INC) $(LIB64) -c -o $(DIR)/obj/main.o $(DIR)/src/main.cpp
	
api.o :
	g++ $(INC) $(LIB64) -c -o $(DIR)/obj/api.o $(DIR)/src/api.cpp

sql.o :
	g++ $(INC) $(LIB64) -c -o $(DIR)/obj/sql.o $(DIR)/src/sql.cpp

converter.o :
	g++ $(INC) $(LIB64) -c -o $(DIR)/obj/converter.o $(DIR)/src/converter.cpp
