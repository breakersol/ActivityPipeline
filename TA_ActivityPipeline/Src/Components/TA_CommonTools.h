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

#ifndef TA_COMMONTOOLS_H
#define TA_COMMONTOOLS_H

#include <list>
#include <map>
#include <string_view>
#include <vector>

#include "TA_MarcoDefine.h"

namespace CoreAsync
{
	class TA_CommonTools
	{
	public:
        template <typename T, typename Container = std::list<std::decay_t<T> > >
		static auto at(const Container &container, std::size_t index)
		{
			if (index >= container.size())
			{
				return T{};
			}
            typename Container::const_iterator pIter = container.begin();
            for (std::size_t i = 0; i < index; i++)
			{
				pIter++;
			}
			return *pIter;
		}

		template <typename T, typename Container = std::list<std::decay_t<T> > >
		static bool replace(Container &container, std::size_t index, T &&elem)
		{
			if (index >= container.size())
			{
				return false;
			}
            typename Container::iterator pIter = container.begin();
            for (std::size_t i = 0; i < index; i++)
			{
				pIter++;
			}
			*pIter = elem;
			return true;
		}

		template <typename T, typename Container = std::list<std::decay_t<T> > >
		static bool remove(Container &container, std::size_t index)
		{
			if (index >= container.size())
			{
				return false;
			}
            typename Container::iterator pIter{ container.begin() };
            for (std::size_t i = 0; i < index; ++i)
			{
				pIter++;
			}
            if constexpr (std::is_pointer_v<decltype(*pIter)>)
			{
				if (*pIter)
				{
					delete* pIter;
					*pIter = nullptr;
				}
			}
			container.erase(pIter);
			return true;
		}

        template <typename Text, typename ...Paras>
        static void debugInfo(Text text, Paras &&...paras)
        {
#ifdef  DEBUG_INFO_ON
            std::printf(std::string_view{Text::data()}.data(), paras...);
#endif
        }

        template <typename Num = std::int_fast64_t>
        static std::string decimalToBinary(Num n) {
            static std::string zero {"0"}, one {"1"};
            if (n == 0) {
                return zero;
            }

            std::string binary = "";
            while (n > 0) {
                binary = (n % 2 == 0 ? zero : one) + binary;
                n /= 2;
            }

            return binary;
        }
	};

    class MapUtils
    {
    public:
        template <typename Key, typename T, typename Cmp = std::less<Key>, typename Allocator = std::allocator<std::pair<const Key, T>> ,template <typename K, typename V, typename C, typename A> class MapType = std::map>
        static std::vector<T> values(MapType<Key, T, Cmp, Allocator> &map, Key k)
        {
            std::vector<T> vec;
            auto &&[start, end] = map.equal_range(k);
            for(auto iter = start; iter != end;++iter)
            {
                vec.emplace_back(iter->second);
            }
            return vec;
        }

        template <typename Key, typename T, typename Cmp = std::less<Key>, typename Allocator = std::allocator<std::pair<const Key, T>>, template <typename K, typename V, typename C, typename A> class MapType = std::map>
        static void remove(MapType<Key, T, Cmp, Allocator> &map, const Key &k)
        {
            for(std::decay_t<decltype(map)> pIter = map.begin(); pIter != map.end();)
            {
                if(pIter->first == k)
                {
                    pIter = map.erase(pIter);
                }
                else
                    ++pIter;
            }
        }

        template <typename Key, typename T, typename Cmp = std::less<Key>, typename Allocator = std::allocator<std::pair<const Key, T>>, template <typename K, typename V, typename C, typename A> class MapType = std::map>
        static void remove(MapType<Key, T, Cmp, Allocator> &map, Key &&k)
        {
            for(typename std::decay_t<decltype(map)>::iterator pIter = map.begin(); pIter != map.end();)
            {
                if(pIter->first == k)
                {
                    pIter = map.erase(pIter);
                }
                else
                    ++pIter;
            }
        }
    };
}

#endif
