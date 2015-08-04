#include "Timer.hpp"
#include <iostream>
#include <unistd.h>

using namespace bedrock;

class TimerTest{
public:
	// Test functionality
	void construct(void);
	void schedule(ThreadPool*);
	void unschedule(ThreadPool*);
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

void TimerTest::schedule(ThreadPool* p)
{
    G_testCounter = 0;


    Timer t(1);
    int key = t.schedule(1, p, 0, callback::wrap(testFunction), 2);
	sleep(4);

	std::cout<<G_testCounter<<std::endl;
	G_testCounter = 0;
	key = t.schedule(1, p, 0, callback::wrap(testFunction));
	sleep(5);

	t.unschedule(key);
	std::cout<<G_testCounter<<std::endl;
}

void TimerTest::unschedule(ThreadPool* p)
{
    Timer t(1);
	int key;
	int firstVal;
	key = t.schedule(1, p, 0, callback::wrap(testFunction));
	sleep(2);
	std::cout<<G_testCounter<<std::endl;
    t.unschedule(key);
    sleep(1);
    firstVal = G_testCounter;
	sleep(3);
}

int main(void){
	TimerTest t;
    ThreadPool tpool(3);
	for(int i=0;i<4;i++){
	t.schedule(&tpool);
	std::cout<<"Finish schedule"<<std::endl;
	t.unschedule(&tpool);
	std::cout<<G_testCounter<<std::endl;
	std::cout<<"Finish unschedule"<<std::endl;
	}
	return 0;
}
