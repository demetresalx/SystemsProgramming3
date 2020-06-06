CC=g++  -std=c++11
CFLAGS=-c -Wpedantic #-Ofast
LDFLAGS=
SOURCES_COMMON=utils.cpp record.cpp
SOURCES_MASTER=master.cpp master_boss.cpp worker.cpp record_HT.cpp cdHashTable.cpp topk.cpp bbst.cpp
SOURCES_SERVER=whoServer.cpp
OBJECTS_COMMON=$(SOURCES_COMMON:.cpp=.o)
OBJECTS_MASTER=$(SOURCES_MASTER:.cpp=.o)
OBJECTS_SERVER=$(SOURCES_SERVER:.cpp=.o)
EXEC_MASTER=master
EXEC_SERVER=whoServer

#syspro
#kuriws
all: Master Server

Master: $(EXEC_MASTER)

Server: $(EXEC_SERVER)

$(EXEC_MASTER): $(OBJECTS_COMMON) $(OBJECTS_MASTER)
	$(CC) $(LDFLAGS) $^ -o $@

$(EXEC_SERVER): $(OBJECTS_COMMON) $(OBJECTS_SERVER)
	$(CC) $(LDFLAGS) $^ -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS_MASTER) $(OBJECTS_SERVER) $(OBJECTS_COMMON) $(EXEC_MASTER) $(EXEC_SERVER) $(EXEC_CLIENT)

#gia master programma:
#./master -w 3 -b 128 -s serverIP -p 4056 -i ../inputs/ass3/input_dir
#valgrind --leak-check=full ./master -w 3 -b 128 -s serverIP -p 4056 -i ../inputs/ass3/input_dir

#gia server programma
#./whoServer -q 7777 -s 4056 -w 5 -b 6
#valgrind --leak-check=full ./whoServer -q 7777 -s 4056 -w 5 -b 6
