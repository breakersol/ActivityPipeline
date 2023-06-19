/*
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

#include "Components/TA_ParallelPipeline.h"
#include "Components/TA_CommonTools.h"
#include "Components/TA_ThreadPool.h"

namespace CoreAsync {
    TA_ParallelPipeline::TA_ParallelPipeline() : TA_BasicPipeline(), m_ftArray(0)
    {
        TA_Connection::connect(&TA_ThreadHolder::get(), &TA_ThreadPool::taskCompleted, this, &TA_ParallelPipeline::asyncTaskCompleted);
    }

    TA_ParallelPipeline::~TA_ParallelPipeline()
    {

    }

    void TA_ParallelPipeline::run()
    {
        unsigned int sIndex(std::move(startIndex()));
        std::size_t activitySize = m_pActivityList.size();
        if(activitySize > 0)
        {
            m_ftArray.resize(activitySize);
            for(std::size_t i = sIndex;i < activitySize;++i)
            {
                m_ftArray[i] = TA_ThreadHolder::get().postActivity(TA_CommonTools::at<TA_BasicActivity *>(m_pActivityList, i));
            }
            for(int index = sIndex;index < activitySize;++index)
            {
                m_resultList[index] = m_ftArray[index].first.get();

            }
        }
    }

    void TA_ParallelPipeline::asyncTaskCompleted(std::size_t id)
    {
        if(m_ftArray.empty())
            return;
        for(int i = 0;i < m_ftArray.size();++i)
        {
            if(m_ftArray[i].second == id)
            {
                m_resultList[i] = m_ftArray[i].first.get();
                TA_Connection::active(this, &TA_ParallelPipeline::activityCompleted, i, m_resultList[i]);
            }
            if(m_ftArray[i].first.valid())
            {
                return;
            }
        }
        setState(State::Ready);
    }
}
