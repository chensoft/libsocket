/**
 * Created by Jian Chen
 * @since  2016.05.24
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <algorithm>
#include <vector>
#include <map>

namespace chen
{
    class map
    {
    public:
        /**
         * Test if the key is in a map
         */
        template <typename K, typename V>
        static bool exist(const std::map<K, V> &m, const K &k)
        {
            return m.find(k) != m.end();
        }

        /**
         * The keys of a map
         */
        template <typename K, typename V>
        static std::vector<K> keys(const std::map<K, V> &m)
        {
            std::vector<K> ret;
            for (auto &pair : m)
                ret.push_back(pair.first);
            return ret;
        }

        /**
         * The values of a map
         */
        template <typename K, typename V>
        static std::vector<V> values(const std::map<K, V> &m)
        {
            std::vector<V> ret;
            for (auto &pair : m)
                ret.push_back(pair.second);
            return ret;
        }

        /**
         * Invert the map, user should make sure the values are unique
         * otherwise the same keys will be override
         */
        template <typename K, typename V>
        static std::map<V, K> flip(const std::map<K, V> &m)
        {
            std::map<V, K> ret;
            for (auto &pair : m)
                ret[pair.second] = pair.first;
            return ret;
        }
    };
}