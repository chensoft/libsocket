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