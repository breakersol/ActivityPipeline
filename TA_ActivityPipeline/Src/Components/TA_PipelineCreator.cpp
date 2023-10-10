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

#include "TA_PipelineCreator.h"

namespace CoreAsync {
    TA_PipelineCreator & TA_PipelineCreator::GetInstance()
    {
        static TA_PipelineCreator creator;
        return creator;
    }

    TA_PipelineCreator::TA_PipelineCreator()
    {

    }

    TA_PipelineCreator::~TA_PipelineCreator()
    {

    }

    std::unique_ptr<TA_MainPipelineHolder<TA_PipelineHolder<TA_AutoChainPipeline> > > TA_PipelineCreator::createAutoChainPipeline()
    {
        return std::make_unique<TA_PipelineHolder<TA_AutoChainPipeline>>();
    }

    std::unique_ptr<TA_MainPipelineHolder<TA_PipelineHolder<TA_ParallelPipeline> > > TA_PipelineCreator::createParallelPipeline()
    {
        return std::make_unique<TA_PipelineHolder<TA_ParallelPipeline>>();
    }

    std::unique_ptr<TA_MainPipelineHolder<TA_PipelineHolder<TA_ManualChainPipeline> > > TA_PipelineCreator::createManaualChainPipeline()
    {
        return std::make_unique<TA_PipelineHolder<TA_ManualChainPipeline>>();
    }

    std::unique_ptr<TA_MainPipelineHolder<TA_PipelineHolder<TA_ManualStepsChainPipeline> > > TA_PipelineCreator::createManualStepsChainPipeline()
    {
        return std::make_unique<TA_PipelineHolder<TA_ManualStepsChainPipeline>>();
    }

    std::unique_ptr<TA_MainPipelineHolder<TA_PipelineHolder<TA_ManualKeyActivityChainPipeline> > > TA_PipelineCreator::createManualKeyActivityChainPipeline()
    {
        return std::make_unique<TA_PipelineHolder<TA_ManualKeyActivityChainPipeline>>();
    }
}
