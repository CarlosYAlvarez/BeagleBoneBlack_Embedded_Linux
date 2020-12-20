OBJS = main.o GPIO.o
GCC = g++ -std=c++11

executable : $(OBJS)
	$(GCC) -o RUN_ME $(OBJS) -pthread

main.o : main.cpp GPIO.h
	$(GCC) -c main.cpp

GPIO.o : GPIO.h GPIO.cpp
	$(GCC) -c GPIO.cpp

.PHONY : clean
clean :
	rm $(OBJS) ./RUN_ME
