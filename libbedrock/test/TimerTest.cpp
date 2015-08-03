#include "Timer.hpp"
#include <iostream>
#include <unistd.h>
using namespace bedrock;

class TimerTest{
public:
	// Test functionality
	void construct(void);
	void schedule(void);
	void unschedule(void);
};

void TimerTest::construct(void)
{
    Timer t(1);
	sleep(1);
}

int G_testCounter;
static
void testFunction(void)
{
    G_testCounter++;
}

void TimerTest::schedule(void)
{
    G_testCounter = 0;

    Timer t(1);
    ThreadPool tpool(3);

    int key = t.schedule(1, &tpool, 0, callback::wrap(testFunction), 2);
	sleep(4);

	G_testCounter = 0;
	key = t.schedule(1, &tpool, 0, callback::wrap(testFunction));
	sleep(5);

	t.unschedule(key);
}

void TimerTest::unschedule(void)
{
    Timer t(1);
    ThreadPool tpool(3);
	int key;
	int firstVal;
	key = t.schedule(1, &tpool, 0, callback::wrap(testFunction));
	sleep(2);
    t.unschedule(key);
    sleep(1);
    firstVal = G_testCounter;
	sleep(3);
}

int main(void){
	TimerTest t;
	t.schedule();
	std::cout<<G_testCounter<<std::endl;
	return 0;
}
