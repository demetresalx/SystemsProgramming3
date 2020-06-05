CC=g++  -std=c++11
CFLAGS=-c -Wpedantic #-Ofast
LDFLAGS=
SOURCES_COMMON=utils.cpp record.cpp
SOURCES_MASTER=master.cpp master_boss.cpp worker.cpp record_HT.cpp cdHashTable.cpp topk.cpp bbst.cpp
OBJECTS_COMMON=$(SOURCES_COMMON:.cpp=.o)
OBJECTS_MASTER=$(SOURCES_MASTER:.cpp=.o)
EXEC_MASTER=master

#syspro
#kuriws
all: Master

Master: $(EXEC_MASTER)

$(EXEC_MASTER): $(OBJECTS_COMMON) $(OBJECTS_MASTER)
	$(CC) $(LDFLAGS) $^ -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS_MASTER) $(OBJECTS_COMMON) $(EXEC_MASTER) $(EXEC_SERVER) $(EXEC_CLIENT)

#./master -w 3 -b 128 -s serverIP -p 4056 -i ../inputs/ass3/input_dir
