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

#ifndef TA_ACTIVITYCREATOR_H
#define TA_ACTIVITYCREATOR_H

#include "Components/TA_LinkedActivity.h"

namespace CoreAsync {
    class ITA_ActivityCreator
    {
    public:
        template <typename Ret,typename Fn,typename Ins,typename... FuncPara,typename = SupportMemberFunction<Fn> >
        static constexpr auto create(Fn &&func, Ins &ins, FuncPara &... para)
        {
            return new CoreAsync::TA_LinkedActivity<Fn,Ins,Ret,FuncPara...>(std::move(func),ins,para...);
        }

        template <typename Ret,typename Fn,typename Ins,typename... FuncPara,typename = SupportMemberFunction<Fn> >
        static constexpr auto create(Fn &&func, Ins &ins, FuncPara &&... para)
        {
            return new CoreAsync::TA_LinkedActivity<Fn,Ins,Ret,FuncPara...>(std::move(func),ins,std::forward<FuncPara>(para)...);
        }

        template <typename Ret,typename ...FuncPara>
        static constexpr auto create(NonMemberFunctionPtr<Ret,FuncPara...> &&func, std::decay_t<FuncPara> &...para)
        {
            return new CoreAsync::TA_LinkedActivity<NonMemberFunctionPtr<Ret,FuncPara...>,INVALID_INS,Ret,FuncPara...>(std::move(func),para...);
        }

        template <typename Ret,typename ...FuncPara>
        static constexpr auto create(NonMemberFunctionPtr<Ret,FuncPara...> &&func,  std::decay_t<FuncPara> &&...para)
        {
            return new CoreAsync::TA_LinkedActivity<NonMemberFunctionPtr<Ret,FuncPara ...>,INVALID_INS,Ret,FuncPara...>(std::move(func),std::forward<FuncPara>(para)...);
        }

        template <typename Ret>
        static constexpr auto create(LambdaTypeWithoutPara<Ret> func)
        {
            return new CoreAsync::TA_LinkedActivity<LambdaTypeWithoutPara<Ret>,INVALID_INS,Ret,INVALID_INS>(std::move(func));
        }

        template <typename Ret,typename ...FuncPara>
        static constexpr auto create(LambdaType<Ret,FuncPara...> func, std::decay_t<FuncPara> &...para)
        {
            return new CoreAsync::TA_LinkedActivity<LambdaType<Ret,FuncPara...>,INVALID_INS,Ret,FuncPara...>(std::move(func),para...);
        }

        template <typename Ret,typename ...FuncPara>
        static constexpr auto create(LambdaType<Ret,FuncPara...> func, std::decay_t<FuncPara> &&...para)
        {
            return new CoreAsync::TA_LinkedActivity<LambdaType<Ret,FuncPara...>,INVALID_INS,Ret,FuncPara...>(std::move(func),std::forward<FuncPara>(para)...);
        }

    };
}

#endif // TA_ACTIVITYCREATOR_H
