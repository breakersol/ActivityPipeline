#ifndef TA_THREADPOOL_H
#define TA_THREADPOOL_H

#include "Components/TA_BasicActivity.h"
#include "TA_MetaObject.h"
#include "TA_ActivityQueue.h"

#include <thread>
#include <vector>
#include <semaphore>

namespace CoreAsync {
    class TA_ThreadPool : public TA_MetaObject
    {
        using HighPriorityQueue = TA_ActivityQueue<TA_BasicActivity *, 128>;

        struct ThreadState
        {
            std::atomic_bool m_isBusy {false};
            std::binary_semaphore resource {0};
        };

    public:
        explicit TA_ThreadPool(std::size_t size = std::thread::hardware_concurrency()) : m_states(size), m_threads(size)
        {
            init();
        }

        ~TA_ThreadPool()
        {
            shutDown();
        }

        TA_ThreadPool(const TA_ThreadPool &pool) = delete;
        TA_ThreadPool(TA_ThreadPool &&pool) = delete;

        TA_ThreadPool & operator = (const TA_ThreadPool &pool) = delete;
        TA_ThreadPool & operator = (TA_ThreadPool &&pool) = delete;

        void shutDown()
        {
            for(std::size_t idx = 0; idx < m_threads.size();++idx)
            {
                m_threads[idx].request_stop();
                m_states[idx].resource.release();
            }
        }

        bool postActivity(TA_BasicActivity *&pActivity)
        {
            if(!pActivity)
                return false;
            if(!m_activityQueue.push(pActivity))
                return false;
            for(std::size_t i = 0;i < m_states.size() - 1;++i)
            {
                if(!m_states[i].m_isBusy.load(std::memory_order_acquire))
                {
                    m_states[i].resource.release();
                    return true;
                }
            }
            return false;
        }

        bool sendActivity(TA_BasicActivity *&pActivity)
        {
            if(!pActivity)
                return false;
            if(!m_highPriorityQueue.push(pActivity))
                return false;
            if(!m_states.back().m_isBusy.load(std::memory_order_acquire))
            {
                m_states.back().resource.release();
                return true;
            }
            return false;
        }

    private:
        void init()
        {
            for(std::size_t idx = 0; idx < m_threads.size();++idx)
            {
                if(idx == m_threads.size() - 1)
                {
                    m_threads.emplace_back(
                        [&](const std::stop_token &st) {
                            TA_BasicActivity *pActivity {nullptr};
                            while (!st.stop_requested()) {
                                m_states[idx].resource.acquire();
                                m_states[idx].m_isBusy.store(true, std::memory_order_release);
                                while (!m_highPriorityQueue.isEmpty())
                                {
                                    if(m_highPriorityQueue.pop(pActivity) && pActivity)
                                    {
                                        (*pActivity)();
                                        pActivity = nullptr;
                                    }
                                }
                                m_states[idx].m_isBusy.store(false, std::memory_order_release);
                            }
                        }
                    );
                }
                else
                {
                    m_threads.emplace_back(
                        [&](const std::stop_token &st) {
                            TA_BasicActivity *pActivity {nullptr};
                            while (!st.stop_requested()) {
                                m_states[idx].resource.acquire();
                                m_states[idx].m_isBusy.store(true, std::memory_order_release);
                                while (!m_activityQueue.isEmpty())
                                {
                                    if(m_activityQueue.pop(pActivity) && pActivity)
                                    {
                                        (*pActivity)();
                                        pActivity = nullptr;
                                    }
                                }
                                m_states[idx].m_isBusy.store(false, std::memory_order_release);
                            }
                        }
                    );
                }
            }
        }

    private:
        std::vector<ThreadState> m_states;;
        std::vector<std::jthread> m_threads;
        ActivityQueue m_activityQueue;
        HighPriorityQueue m_highPriorityQueue;
    };
}

#endif // TA_THREADPOOL_H
