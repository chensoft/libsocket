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
    class array
    {
    public:
        /**
         * Make array be unique
         */
        template <typename V>
        static std::vector<V>& unique(std::vector<V> &v)
        {
            std::sort(v.begin(), v.end());
            v.erase(std::unique(v.begin(), v.end()), v.end());
            return v;
        }

        /**
         * Union the two arrays
         * notice the two arrays will be sorted
         */
        template <typename V>
        static std::vector<V> combine(std::vector<V> &v1, std::vector<V> &v2)
        {
            std::sort(v1.begin(), v1.end());
            std::sort(v2.begin(), v2.end());

            std::vector<V> v(v1.size() + v2.size());

            auto it = std::set_union(v1.begin(), v1.end(), v2.begin(), v2.end(), v.begin());

            v.resize(it - v.begin());

            return v;
        }

        /**
         * Get the intersection of two arrays
         */
        template <typename V>
        static std::vector<V> intersect(std::vector<V> &v1, std::vector<V> &v2)
        {
            std::sort(v1.begin(), v1.end());
            std::sort(v2.begin(), v2.end());

            std::vector<V> v(v1.size() + v2.size());

            auto it = std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), v.begin());

            v.resize(it - v.begin());

            return v;
        }

        /**
         * Get the values exist in v1, but not in v2
         */
        template <typename V>
        static std::vector<V> difference(std::vector<V> &v1, std::vector<V> &v2)
        {
            std::sort(v1.begin(), v1.end());
            std::sort(v2.begin(), v2.end());

            std::vector<V> v(v1.size() + v2.size());

            auto it = std::set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), v.begin());

            v.resize(it - v.begin());

            return v;
        }
    };
}