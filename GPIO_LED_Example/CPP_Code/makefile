OBJS = main.o GPIO.o

executable : $(OBJS)
	g++ -o RUN_ME $(OBJS)

main.o : main.cpp GPIO.h
	g++ -std=c++11 -c main.cpp

GPIO.o : GPIO.h GPIO.cpp
	g++ -std=c++11 -c GPIO.cpp

.PHONY : clean
clean :
	rm $(OBJS) ./RUN_ME
