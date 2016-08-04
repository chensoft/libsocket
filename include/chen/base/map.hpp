/**
 * Created by Jian Chen
 * @since  2016.05.24
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <algorithm>
#include <vector>

namespace chen
{
    class map
    {
    public:
        /**
         * Test if the key is in a map
         */
        template <typename M>
        static bool exist(const M &m, const typename M::key_type &k)
        {
            return m.find(k) != m.end();
        }

        /**
         * Find value in map
         */
        template <typename M>
        static typename M::mapped_type find(const M &m, const typename M::key_type &k)
        {
            auto it = m.find(k);
            return it != m.end() ? it->second : typename M::mapped_type();
        }

        template <typename M, typename R>
        static R find(const M &m, const typename M::key_type &k, R def)
        {
            auto it = m.find(k);
            return it != m.end() ? static_cast<R>(it->second) : def;
        }

        /**
         * Write value if find
         */
        template <typename M, typename O>
        static void write(const M &m, const typename M::key_type &k, O &o)
        {
            auto it = m.find(k);
            if (it != m.end())
                o = it->second;
        }

        /**
         * The keys of a map
         */
        template <typename M>
        static std::vector<typename M::key_type> keys(const M &m)
        {
            std::vector<typename M::key_type> ret;
            for (auto &pair : m)
                ret.emplace_back(pair.first);
            return ret;
        }

        /**
         * The values of a map
         */
        template <typename M>
        static std::vector<typename M::mapped_type> values(const M &m)
        {
            std::vector<typename M::mapped_type> ret;
            for (auto &pair : m)
                ret.emplace_back(pair.second);
            return ret;
        }

        /**
         * Invert the map, user should make sure the values are unique
         * otherwise the same keys will be override
         */
        template <typename M>
        static M flip(const M &m)
        {
            M ret;
            for (auto &pair : m)
                ret[pair.second] = pair.first;
            return ret;
        }
    };
}