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

#ifndef TA_PARALLELPIPELINE_H
#define TA_PARALLELPIPELINE_H

#include "TA_BasicPipeline.h"

namespace CoreAsync {
    class ASYNC_PIPELINE_EXPORT TA_ParallelPipeline : public TA_BasicPipeline
    {
    public:
        explicit TA_ParallelPipeline();
        virtual ~TA_ParallelPipeline();

        TA_ParallelPipeline(const TA_ParallelPipeline &activity) = delete;
        TA_ParallelPipeline(TA_ParallelPipeline &&activity) = delete;
        TA_ParallelPipeline & operator = (const TA_ParallelPipeline &) = delete;

        void asyncTaskCompleted(std::size_t id);

    protected:
        void run() override final;

    private:
        std::vector<std::pair<std::shared_future<TA_Variant>, std::size_t> >m_ftArray;

    };

    namespace Reflex
    {
        template <>
        struct ASYNC_PIPELINE_EXPORT TA_TypeInfo<TA_ParallelPipeline> : TA_MetaTypeInfo<TA_ParallelPipeline,TA_BasicPipeline>
        {
            static constexpr TA_MetaFieldList fields = {
                TA_MetaField {&Raw::asyncTaskCompleted, META_STRING("asyncTaskCompleted")},
            };
        };
    }
}

#endif // TA_PARALLELPIPELINE_H
