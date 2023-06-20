#ifndef TA_THREADPOOL_H
#define TA_THREADPOOL_H

#include "TA_MetaObject.h"
#include "TA_ActivityQueue.h"
#include "TA_LinkedActivity.h"

#include <thread>
#include <vector>
#include <semaphore>
#include <future>

namespace CoreAsync {
    class TA_ThreadPool : public TA_MetaObject
    {
        using HighPriorityQueue = TA_ActivityQueue<TA_BasicActivity *, 1024>;
        using SharedPromise = std::shared_ptr<std::promise<TA_Variant> >;

        struct ThreadState
        {
            std::atomic_bool m_isBusy {false};
            std::binary_semaphore resource {0};
        };

    public:
        explicit TA_ThreadPool(std::size_t size = std::thread::hardware_concurrency()) : m_states(size)
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

        auto postActivity(TA_BasicActivity *pActivity, bool autoDelete = false)
        {
            if(!pActivity)
                return std::future<TA_Variant> {};
            auto func = [pActivity, autoDelete](SharedPromise pr)->void {
                pr->set_value((*pActivity)());
                if(autoDelete)
                {
                    delete pActivity;
                }
            };
            SharedPromise pr {std::make_shared<std::promise<TA_Variant>>()};
            std::future<TA_Variant> ft {pr->get_future()};
            if(!m_activityQueue.push(new TA_LinkedActivity<LambdaType<void,SharedPromise>,INVALID_INS,void,SharedPromise>(func, std::move(pr))))
                return std::future<TA_Variant> {};
            for(std::size_t i = 0;i < m_states.size() - 1;++i)
            {
                if(!m_states[i].m_isBusy.load(std::memory_order_acquire))
                {
                    m_states[i].resource.release();
                    break;
                }
            }
            return ft;
        }

        auto sendActivity(TA_BasicActivity *pActivity, bool autoDelete = false)
        {
            if(!pActivity)
                return std::future<TA_Variant> {};
            auto func = [pActivity, autoDelete](SharedPromise pr)->void {
                TA_Variant var = (*pActivity)();
                pr->set_value(var);
                if(autoDelete)
                {
                    delete pActivity;
                }
            };
            SharedPromise pr {std::make_shared<std::promise<TA_Variant>>()};
            std::future<TA_Variant> ft {pr->get_future()};
            if(!m_highPriorityQueue.push(new TA_LinkedActivity<LambdaType<void,SharedPromise>,INVALID_INS,void,SharedPromise>(func, std::move(pr))))
                return std::future<TA_Variant> {};
            if(!m_states.back().m_isBusy.load(std::memory_order_acquire))
            {
                m_states.back().resource.release(); 
            }
            return ft;
        }

        std::size_t size() const
        {
            return m_threads.size();
        }

    private:
        void init()
        {
            for(std::size_t idx = 0; idx < m_states.size();++idx)
            {
                if(idx == m_states.size() - 1)
                {
                    m_threads.emplace_back(
                        [&, idx](const std::stop_token &st) {
                            TA_BasicActivity *pActivity {nullptr};
                            while (!st.stop_requested()) {
                                m_states[idx].resource.acquire();
                                m_states[idx].m_isBusy.store(true, std::memory_order_release);
                                while (!m_highPriorityQueue.isEmpty())
                                {
                                    if(m_highPriorityQueue.pop(pActivity) && pActivity)
                                    {
                                        (*pActivity)();
                                        delete pActivity;
                                        pActivity = nullptr;
                                    }
                                }
                                m_states[idx].m_isBusy.store(false, std::memory_order_release);
                            }
                            std::printf("Shut down successuflly!\n");
                        }
                    );
                }
                else
                {
                    m_threads.emplace_back(
                        [&, idx](const std::stop_token &st) {
                            TA_BasicActivity *pActivity {nullptr};
                            while (!st.stop_requested()) {
                                m_states[idx].resource.acquire();
                                m_states[idx].m_isBusy.store(true, std::memory_order_release);
                                while (!m_activityQueue.isEmpty())
                                {
                                    if(m_activityQueue.pop(pActivity) && pActivity)
                                    {
                                        (*pActivity)();
                                        delete pActivity;
                                        pActivity = nullptr;     
                                    }
                                }
                                m_states[idx].m_isBusy.store(false, std::memory_order_release);
                            }
                            std::printf("Shut down successuflly!\n");
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

    struct TA_ThreadHolder
    {
        static TA_ThreadPool & get()
        {
            static TA_ThreadPool pool;
            return pool;
        }
    };
}

#endif // TA_THREADPOOL_H
