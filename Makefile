export LC_ALL=C
CXXFLAGS= -Wall -pedantic -g -ggdb
LDFLAGS= -lpthread

OBJS=main.o simu.o basetask.o tasks.o sched_rr.o sched_fcfs.o sched_rsd.o sched_rr2.o
MAIN=simusched

.PHONY: all
all: $(MAIN)

$(MAIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

basetask.o: basetask.cpp basetask.h
main.o: main.cpp simu.h basetask.h basesched.h tasks.h sched_fcfs.h \
 sched_rr.h
sched_fcfs.o: sched_fcfs.cpp sched_fcfs.h basesched.h
sched_rr.o: sched_rr.cpp sched_rr.h basesched.h
simu.o: simu.cpp simu.h basetask.h basesched.h
simusched.o: simusched.cpp basetask.h basesched.h tasks.h
tasks.o: tasks.cpp tasks.h basetask.h
sched_rsd.o: sched_rsd.cpp sched_rsd.h basesched.h
sched_rr2.o: sched_rr2.cpp sched_rr2.h basesched.h

.PHONY: clean
clean:
	rm -f $(OBJS) $(MAIN)

.PHONY: new
new: clean all
