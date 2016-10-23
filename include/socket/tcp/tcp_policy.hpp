/**
 * Created by Jian Chen
 * @since  2016.10.23
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <cstddef>
#include <string>

namespace chen
{
    namespace tcp
    {
        // ---------------------------------------------------------------------
        // policy
        class policy
        {
        public:
            enum class Type {Some, All, Line, Amount, Delimiter};

        protected:
            policy(Type type, bool once = true) : type(type), once(once) {}

        public:
            Type type;
            bool once;
        };


        // ---------------------------------------------------------------------
        // read_some_policy
        class read_some_policy : public policy
        {
        public:
            read_some_policy(std::size_t size) : policy(Type::Some), size(size) {}

            std::size_t size;
        };


        // ---------------------------------------------------------------------
        // read_all_policy
        class read_all_policy : public policy
        {
        public:
            read_all_policy() : policy(Type::All, false) {}
        };


        // ---------------------------------------------------------------------
        // read_line_policy
        class read_line_policy : public policy
        {
        public:
            read_line_policy() : policy(Type::Line) {}

            std::size_t pos = 0;
        };


        // ---------------------------------------------------------------------
        // read_amount_policy
        class read_amount_policy : public policy
        {
        public:
            read_amount_policy(std::size_t size) : policy(Type::Amount), size(size) {}

            std::size_t size;
        };


        // ---------------------------------------------------------------------
        // read_delimiter_policy
        class read_delimiter_policy : public policy
        {
        public:
            read_delimiter_policy(const std::string &text) : policy(Type::Delimiter), text(text) {}

            std::string text;
            std::size_t pos = 0;
        };
    }
}