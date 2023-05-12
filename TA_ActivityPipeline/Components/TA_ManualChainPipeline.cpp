﻿/*
 * Copyright [2023] [Shuang Zhu / Sol]
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Components/TA_ManualChainPipeline.h"
#include "Components/TA_CommonTools.h"

namespace CoreAsync {
    TA_ManualChainPipeline::TA_ManualChainPipeline() : TA_BasicPipeline()
    {

    }

    void TA_ManualChainPipeline::run()
    {
        if(!m_pActivityList.empty() && m_currentIndex.load(std::memory_order_acquire) <= m_pActivityList.size() - 1)
        {
            auto pActivity = TA_CommonTools::at<TA_BasicActivity *>(m_pActivityList, m_currentIndex.load(std::memory_order_acquire));
            if(pActivity)
            {
                TA_Variant var = pActivity->execute();
                TA_CommonTools::replace(m_resultList, m_currentIndex.load(std::memory_order_acquire), var);
            }
            m_currentIndex.fetch_add(1);
        }
        if(m_currentIndex.load(std::memory_order_acquire) < m_pActivityList.size())
        {
            setState(State::Waiting);
        }
        else
        {
            setState(State::Ready);
        }
        TA_Connection::active(this, &TA_ManualChainPipeline::activityCompleted, m_currentIndex.load(std::memory_order_acquire));
    }

    void TA_ManualChainPipeline::setStartIndex(unsigned int index)
    {
        TA_BasicPipeline::setStartIndex(index);
        m_currentIndex.store(startIndex(),std::memory_order_release);
    }

    bool TA_ManualChainPipeline::remove(ActivityIndex index)
    {
        std::ignore = index;
        return false;
    }

    void TA_ManualChainPipeline::reset()
    {
        if(state() == State::Busy)
        {
            assert(state() != State::Busy);
            std::printf("Reset pipeline failed!");
            return;
        }
        m_currentIndex.store(0,std::memory_order_release);
        m_mutex.lock();
        m_resultList.clear();
        m_resultList.resize(m_pActivityList.size());
        m_mutex.unlock();
        setState(State::Waiting);
        setStartIndex(0);
    }

    void TA_ManualChainPipeline::clear()
    {
        if(state() == State::Busy)
        {
            assert(state() != State::Busy);
            std::printf("Clear pipeline failed!");
            return;
        }
        m_mutex.lock();
        for(auto &pActivity : m_pActivityList)
        {
            if(pActivity)
            {
                delete pActivity;
                pActivity = nullptr;
            }
        }
        m_pActivityList.clear();
        m_resultList.clear();
        m_mutex.unlock();
        setState(State::Waiting);
        setStartIndex(0);
        m_currentIndex.store(0,std::memory_order_release);
    }

    bool TA_ManualChainPipeline::waitingComplete()
    {
        std::chrono::steady_clock::time_point asyncStartTime;
        constexpr int interval = 100;
        while (state() == State::Busy)
        {
            asyncStartTime = std::chrono::steady_clock::now();
            while (std::chrono::duration_cast<Milliseconds>(std::chrono::steady_clock::now() - asyncStartTime).count() < interval){}
        }

        return true;
    }
}

