#include <iostream>
#include "coroutine.h"
#include <thread>
#include <chrono>


using namespace std::chrono_literals;


class ARoutine : public test::Routine
{
public:
    ARoutine(test::Schedule& scheduler)
    :Routine(scheduler){};
    void Run() override
    {
        for(int i = 1;i < 100;++i){
            std::cout << "A: " << i * 2 << std::endl;
            Yield();
            std::this_thread::sleep_for(0.1s);
        }
    }
};

class BRoutine : public test::Routine
{
public:
    BRoutine(test::Schedule& scheduler)
    :Routine(scheduler){};
    void Run() override
    {
        for(int i = 1;i < 50;++i){
            std::cout << "B: " << i * 3 << std::endl;
            Yield();
            std::this_thread::sleep_for(0.1s);
        }
    }
};

class CRoutine : public test::Routine
{
public:
    CRoutine(test::Schedule& scheduler)
    :Routine(scheduler){};
    void Run() override
    {
        for(int i = 1;i < 25;++i){
            std::cout << "C: " << i * 4 << std::endl;
            Yield();
            std::this_thread::sleep_for(0.1s);
        }
    }
};

int main()
{
    test::Schedule scheduler;
    ARoutine aTask(scheduler);
    BRoutine bTask(scheduler);
    CRoutine cTask(scheduler);

    scheduler.Append(&aTask);
    scheduler.Append(&bTask);
    scheduler.Append(&cTask);

    while(!scheduler.Empty()){
        std::cout << "main" << std::endl;
        scheduler.Resume();
    }

}