#pragma once

#include<stddef.h>
#include<ucontext.h>
#include<list>

namespace test
{

class Schedule;

class Routine
{
    friend class Schedule;
public:
    typedef enum class Status
    {
        RT_READY = 0,
        RT_RUNNING,
        RT_SUSPEND,
        RT_FINISHED
    } STATUS;

    Routine(Schedule& schedule);
    

    virtual ~Routine();
    virtual void Run() = 0;

    void Resume();
    void Yield();
    STATUS Status() const {return m_Status;};
protected:
    static void Func(void* ptr);
private:
    ucontext_t m_Ctx;
    STATUS m_Status;
    char *m_Stack;
    int m_StackSize;
    Schedule& m_Schedule;
};

class Schedule
{
    friend class Routine;
public:
    Schedule(int stackSize = 1024 * 1024):m_StackSize(stackSize){};
    virtual ~Schedule(){}

    Schedule(const Schedule&) = delete;
    Schedule& operator=(const Schedule&) = delete;
    void Append(Routine* task);
    bool Empty() const;
    int Size() const;
    void Resume();
    // void Create(int stackSize = 1024 * 1024);
private:
    ucontext_t m_Main;
    int m_StackSize;
    // int main_StackSize;
    std::list<Routine*> m_Queue;
    // char *m_Stack;
};
}