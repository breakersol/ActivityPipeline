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

#ifndef TA_METAREFLEX_H
#define TA_METAREFLEX_H

#include <string_view>
#include <variant>
#include <concepts>

#include "Components/TA_MetaStringView.h"
#include "TA_TypeFilter.h"
#include "TA_TypeList.h"

namespace CoreAsync::Reflex{

template <typename T , typename = std::void_t<> >
struct HasValidString
{
    static constexpr bool value = false;
};

template <typename T>
struct HasValidString<T, std::void_t<typename std::basic_string_view<T>::value_type > >
{
    static constexpr bool value = true;
};

template <typename TypeStr>
struct TA_BaseTypeName
{
    using TName = TypeStr;
    static constexpr std::string_view name = TName::data();
};

template <typename T, typename N>
class TA_MetaTypeName : public TA_BaseTypeName<N>
{
public:
    constexpr TA_MetaTypeName(T value) : TA_BaseTypeName<N>() ,m_value(value)
    {

    }

    static constexpr bool isValid()
    {
        return m_isValid;
    }

    constexpr T value() const
    {
        return m_value;
    }

    template <typename OT>
    constexpr bool operator == (OT value) const
    {
        if constexpr(std::is_same_v<T, OT>)
        {
            return m_value == value;
        }
        else
        {
            return false;
        }
    }

private:
    const T m_value;
    static constexpr bool m_isValid = true;

};

template <typename N>
class TA_MetaTypeName<void, N> : public TA_BaseTypeName<N>
{
public:
    static constexpr bool isValid()
    {
        return m_isValid;
    }

    template <typename OT>
    constexpr bool operator == (OT) const
    {
        return false;
    }

private:
    static constexpr bool m_isValid = false;

};

template <typename T>
struct TA_MemberTypeTrait
{
    static constexpr bool enumFlag = std::is_enum<T>::value;
    static constexpr bool funcFlag = std::is_function_v<T>;
    static constexpr bool noneStaticMemberVariableFlag = IsNonStaticMemberVariable<T>::value;
    static constexpr bool noneStaticMemberFuncFlag = IsNonStaticMemberFunc<T>::value;
};

template <typename T, typename NAME>
struct TA_MetaField : TA_MemberTypeTrait<T>, TA_MetaTypeName<T,NAME>
{
    constexpr TA_MetaField(T t, NAME) : TA_MetaTypeName<T,NAME>{t}
    {

    }

    constexpr TA_MetaField() : TA_MetaTypeName<T,NAME> {nullptr}
    {

    }
};

template <typename ...FIELDS>
class TA_MetaFieldList
{
public:
    using ValueTypes = TA_MetaTypelist<decltype(FIELDS {}.value())...>;

    constexpr TA_MetaFieldList(FIELDS ...fs) : m_fields(fs...)
    {

    }

    constexpr auto get() const
    {
        return m_fields;
    }

    constexpr std::size_t size() const
    {
        return sizeof...(FIELDS);
    }

    template <std::size_t INDEX>
    constexpr auto getField() const
    {
        return std::get<INDEX>(m_fields);
    }

    template <typename NAME>
    constexpr auto findType(NAME = {}) const
    {
        constexpr std::size_t index = [](){
            constexpr decltype(NAME::data()) names [] {FIELDS::name...};
            for(std::size_t i = 0;i <sizeof...(FIELDS);++i)
            {
                if(NAME::data() == names[i])
                {
                    return i;
                }
            }
            return std::size_t(-1);
        } ();
        return std::get<index>(m_fields).value();
    }

    template <typename VALUE>
    constexpr auto findName(VALUE &&v) const
    {
        return findMatchedName(std::forward<VALUE>(v),std::make_index_sequence<sizeof...(FIELDS)>());
    }

    template <typename VALUE>
    constexpr bool containsType(VALUE &&v) const
    {
        return containsMatchedType(std::forward<VALUE>(v),std::make_index_sequence<sizeof...(FIELDS)>{});
    }

    template <typename NAME>
    constexpr bool containsName(NAME = {}) const
    {
        return [](){
            constexpr decltype(NAME::data()) names [] {FIELDS::name...};
            for(std::size_t i = 0;i <sizeof...(FIELDS);++i)
            {
                if(NAME::data() == names[i])
                {
                    return true;
                }
            }
            return false;
        } ();
    }

private:

    template <typename VALUE>
    constexpr bool containsMatchedType(VALUE &&, std::index_sequence<>) const
    {
        return false;
    }

    template <typename VALUE, std::size_t IDX0, std::size_t ...IDXS>
    constexpr bool containsMatchedType(VALUE &&v, std::index_sequence<IDX0, IDXS...>) const
    {
        if constexpr(std::is_same_v<VALUE, decltype(std::get<IDX0>(m_fields).value())>)
        {
            return v == std::get<IDX0>(m_fields).value() ? true : containsMatchedType(std::forward<VALUE>(v),std::index_sequence<IDXS...>());
        }
        else
        {
            return containsMatchedType(std::forward<VALUE>(v),std::index_sequence<IDXS...>());
        }
    }

    template <typename VALUE>
    constexpr auto findMatchedName(VALUE &&, std::index_sequence<>) const
    {
        return "";
    }

    template <typename VALUE, std::size_t IDX0, std::size_t ...IDXS>
    constexpr auto findMatchedName(VALUE &&v, std::index_sequence<IDX0,IDXS...>) const
    {
        if constexpr(std::is_same_v<VALUE,  decltype(std::get<IDX0>(m_fields).value())>)
        {
            return v == std::get<IDX0>(m_fields).value() ? std::get<IDX0>(m_fields).name : findMatchedName(std::forward<VALUE>(v),std::index_sequence<IDXS...>());
        }
        else
        {
            return findMatchedName(std::forward<VALUE>(v),std::index_sequence<IDXS...>());
        }
    }


private:
    std::tuple<FIELDS...> m_fields;

};

template <typename T>
struct TA_MetaTypeAttribute
{
    static constexpr bool isPolymorphic = std::is_polymorphic_v<T>;
    static constexpr bool isAbstract = std::is_abstract_v<T>;
    static constexpr bool isClass = std::is_class_v<T>;
};

template <typename T>
struct TA_TypeInfo;

template <typename T, typename ...BASES>
struct TA_MetaTypeInfo :  TA_MetaTypeAttribute<T>
{
    using Raw = T;

    static constexpr std::size_t size()
    {
        return std::tuple_size_v<decltype(aggregate())>;
    }

    template <typename NAME, typename ...PARAS>
    static constexpr auto invoke(NAME = {}, PARAS &&...paras)
    {
        constexpr auto target = findType(NAME {});
        using CF = decltype(target);
        if constexpr(std::is_enum_v<decltype(target)>)
        {
            return target;
        }
        else if constexpr(!std::is_function_v<std::remove_pointer_t<CF> > && !std::is_member_object_pointer_v<CF> && !std::is_member_function_pointer_v<CF>)
        {
            return *target;
        }
        else
        {
            return std::invoke(target,std::forward<PARAS>(paras)...);
        }
    }

    template <typename VALUE>
    static constexpr bool containsValue(VALUE && v)
    {
        return containsField(std::forward<VALUE>(v), std::make_index_sequence<std::tuple_size_v<decltype(aggregate())> > {});
    }

    template <typename VALUE>
    static constexpr auto findName(VALUE &&v)
    {
        return findName(std::forward<VALUE>(v), std::make_index_sequence<std::tuple_size_v<decltype(aggregate())> > {});
    }

    static constexpr auto findTypeValue(std::string_view &str)      //run time finding
    {
        return findValue(std::forward<std::string_view>(str), std::make_index_sequence<std::tuple_size_v<decltype(aggregate())>> {});
    }

    template <typename NAME>
    static constexpr bool containsName(NAME = {})
    {
        if constexpr(!TA_MetaTypeInfo<T>::isClass)
        {
            return TA_TypeInfo<T>::fields.containsName(NAME{});
        }
        if constexpr(!TA_MetaTypeInfo<T>::isPolymorphic && sizeof...(BASES) == 0)
        {
            return TA_TypeInfo<T>::fields.containsName(NAME{});
        }
        else if constexpr(TA_TypeInfo<T>::fields.containsName(NAME {}))
        {
            return true;
        }
        else
        {
            return containsNameFromBase(std::make_index_sequence<sizeof...(BASES)> {}, NAME {});
        }
    }

    template <typename NAME>
    static constexpr auto findType(NAME = {})           //compile time finding
    {
        if constexpr(!TA_MetaTypeInfo<T>::isClass)
        {
            if constexpr(TA_TypeInfo<T>::fields.containsName(NAME{}))
            {
                return TA_TypeInfo<T>::fields.findType(NAME{});
            }
            else
            {
                return nullptr;
            }
        }
        if constexpr(!TA_MetaTypeInfo<T>::isPolymorphic && sizeof...(BASES) == 0)
        {
            if constexpr(TA_TypeInfo<T>::fields.containsName(NAME{}))
            {
                return TA_TypeInfo<T>::fields.findType(NAME{});
            }
            else
            {
                return nullptr;
            }
        }
        else
        {
            if constexpr(TA_TypeInfo<T>::fields.containsName(NAME{}))
            {
                return TA_TypeInfo<T>::fields.findType(NAME{});
            }
            else
            {
                return findTypeFromBase(std::make_index_sequence<sizeof...(BASES)>{}, NAME {});
            }
        }
    }

    struct TA_Values
    {
        using VariantTypes = typename MetaMerge<typename decltype(TA_TypeInfo<T>::fields)::ValueTypes, typename TA_TypeInfo<BASES>::TA_Values::VariantTypes...>::type;
    };

    template <typename BASE>
    struct TA_MetaInfoPack
    {
        using type = TA_TypeInfo<BASE>;
    };

    struct TA_BaseTypes
    {
        using types = TA_MetaTypelist<BASES...>;
        static constexpr std::size_t size {sizeof...(BASES)};

        static constexpr auto typeNames()
        {
            std::array<std::string_view, Reflex::TA_TypeInfo<std::decay_t<T>>::TA_BaseTypes::size> arr {typeid(BASES).name()...};
            return arr;
        }
    };

    struct TA_MetaBaseTypesInfo
    {
        using types = typename MetaMap<TA_MetaTypelist<BASES...>, TA_MetaInfoPack>::type;
        static constexpr std::size_t size = types::size;
    };

    static constexpr auto aggregate()
    {
        return std::tuple_cat(TA_MetaInfoPack<T>::type::fields.get(), TA_TypeInfo<BASES>::aggregate()...);
    }

private:
    template <typename NAME>
    static constexpr bool containsNameFromBase(std::index_sequence<> , NAME = {})
    {
        return false;
    }

    template <std::size_t IDX0, std::size_t ...IDXS, typename NAME>
    static constexpr bool containsNameFromBase(std::index_sequence<IDX0, IDXS...> , NAME = {})
    {
        if constexpr(MetaTypeAt<typename TA_MetaBaseTypesInfo::types, IDX0>::type::containsName(NAME {}))
        {
            return true;
        }
        else
        {
            return containsNameFromBase(std::index_sequence<IDXS...> {}, NAME {});
        }
    }

    template <typename NAME>
    static constexpr auto findTypeFromBase(std::index_sequence<> , NAME = {})
    {
        return nullptr;
    }

    template <std::size_t IDX0, std::size_t ...IDXS, typename NAME>
    static constexpr auto findTypeFromBase(std::index_sequence<IDX0,IDXS...> , NAME = {})
    {
        return [](NAME = {}) {
            if constexpr(MetaTypeAt<typename TA_MetaBaseTypesInfo::types, IDX0>::type::containsName(NAME {}))
            {
                return MetaTypeAt<typename TA_MetaBaseTypesInfo::types, IDX0>::type::findType(NAME {});
            }
            else
            {
                return findTypeFromBase(std::index_sequence<IDXS...> {}, NAME {});
            }
        } ();
    }

    template <typename VALUE>
    static constexpr bool containsField(VALUE &&, std::index_sequence<>)
    {
        return false;
    }

    template <typename VALUE ,std::size_t IDX0, std::size_t ...IDXS>
    static constexpr bool containsField(VALUE &&v, std::index_sequence<IDX0, IDXS...> = {})
    {
        if constexpr(std::is_same_v<decltype(std::get<IDX0>(aggregate()).value()),VALUE>)
        {
            if(v == std::get<IDX0>(aggregate()).value())
                return true;
        }
        return containsField(std::forward<VALUE>(v), std::index_sequence<IDXS...> {});
    }

    template <typename VALUE>
    static constexpr auto findName(VALUE &&, std::index_sequence<>)
    {
        return std::string_view {}.data();
    }

    template <typename VALUE, std::size_t IDX0, std::size_t ...IDXS>
    static constexpr auto findName(VALUE &&v, std::index_sequence<IDX0, IDXS...> = {})
    {
        if constexpr(std::is_same_v<decltype(std::get<IDX0>(aggregate()).value()),std::decay_t<VALUE> >)
        {
            if(v == std::get<IDX0>(aggregate()).value())
                return std::get<IDX0>(aggregate()).name.data();
        }
        return findName(std::forward<VALUE>(v), std::index_sequence<IDXS...> {});
    }


    static constexpr decltype(auto) findValue(std::string_view &&str, std::index_sequence<> = {})
    {
        return typename MetaVariant<typename TA_Values::VariantTypes>::Var {};
    }

    template <std::size_t IDX0, std::size_t ...IDXS>
    static constexpr decltype(auto) findValue(std::string_view &&str, std::index_sequence<IDX0, IDXS...> = {})
    {
        if(str == std::get<IDX0>(aggregate()).name)
        {
            typename MetaVariant<typename TA_Values::VariantTypes>::Var var {std::get<IDX0>(aggregate()).value()};
            return var;
        }
        return findValue(std::forward<std::string_view>(str),std::index_sequence<IDXS...> {});
    }
};

}
#endif // TA_METAREFLEX_H
