to compile slaves: g++ slave.cpp -o slave.out -lcxxtools -lcxxtools-json -lpthread

to complie master : make

OUTPUT is saved in file output.txt

The heartbeat thread will exit when all jobs have been done and master has no more work for slaves, after that masters and slave will return and exit execution

Stragllers are checked comparing estimated time of complition of task by all threads if it doesn't respond a done signal in time the task will be resheduled, also if a worker has died it won't respond in time but master will check beforehead if any slave has died before providing the result thus if yes reassign that task to free alive clients
