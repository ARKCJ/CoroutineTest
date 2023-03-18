#pragma once

#include "coroutine.h"
#include <assert.h>
#include <exception>

namespace test
{
    Routine::Routine(Schedule& schedule)
    :m_Schedule(schedule), 
    m_StackSize(schedule.m_StackSize),
    m_Status(STATUS::RT_READY)
    {}

    void Routine::Resume()
    {
        if(Status() == STATUS::RT_READY){
            m_Stack = reinterpret_cast<char*>(
                ::operator new(m_StackSize * sizeof(char))
            );
            getcontext(&m_Ctx);

            m_Ctx.uc_stack.ss_sp = m_Stack;
            m_Ctx.uc_stack.ss_size = m_StackSize;
            // sucessor context
            m_Ctx.uc_link = &m_Schedule.m_Main;
            m_Status = STATUS::RT_RUNNING;

            makecontext(&m_Ctx, reinterpret_cast<void(*)()>(Func), 1, this);
            // getcontext(&m_Schedule.m_Main);
            // setcontext(&m_Ctx);
            swapcontext(&m_Schedule.m_Main, &m_Ctx);
            // break;
        }else if(Status() == STATUS::RT_SUSPEND){
            m_Status = STATUS::RT_RUNNING;
            swapcontext(&m_Schedule.m_Main, &m_Ctx);
            // break;
        }else{
            assert(0);
        }
    }
    
    void Routine::Yield()
    {
        if(Status() != STATUS::RT_RUNNING){
            assert(0);
            return;
        }
        m_Status = STATUS::RT_SUSPEND;
        swapcontext(&m_Ctx, &m_Schedule.m_Main);
    }


    // static
    void Routine::Func(void* ptr)
    {
        Routine* r = reinterpret_cast<Routine*>(ptr);
        r -> Run();
        r -> m_Status = STATUS::RT_FINISHED;
    }

    Routine::~Routine()
    {
        if(Status() != STATUS::RT_FINISHED){
            assert(0);
        }else{
            ::operator delete(m_Stack);
        }
    }
    // Routine

    void Schedule::Append(Routine* task)
    {
        m_Queue.push_back(task);
    }

    bool Schedule::Empty() const{ return m_Queue.empty(); }

    int Schedule::Size() const{ return m_Queue.size(); }


    void Schedule::Resume()
    {
        auto iter = m_Queue.begin();
        auto pre_iter = iter;
        while(iter != m_Queue.end()){
            if((*iter) -> Status() == Routine::STATUS::RT_FINISHED){
                if(iter != m_Queue.begin()){
                    m_Queue.erase(iter);
                    auto cur = pre_iter;
                    iter = ++cur;
                }else{
                    m_Queue.erase(iter);
                    pre_iter = m_Queue.begin();
                    iter = pre_iter;
                }
            }else{
                (*iter)->Resume();
                pre_iter = iter;
                ++iter;
            }
        }
    }

}