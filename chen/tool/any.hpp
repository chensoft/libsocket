/**
 * A wrapper to hold any type
 * @since  2016.01.12
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

namespace chen
{
    class any
    {
    public:
        any() = default;
        any(const any &o);
        any(any &&o);

        template <typename T>
        any(T val)
        : _ptr(new data<T>(val))
        {
        }

        ~any();

    public:
        /**
         * Copy & Move
         */
        any& operator=(const any &o);
        any& operator=(any &&o);

        /**
         * Assignment
         * e.g: Any a; a = 15;
         */
        template <typename T>
        any& operator=(T val)
        {
            this->clear();
            this->_ptr = new data<T>(val);
            return *this;
        }

        /**
         * Conversion
         * e.g: Any a(15); int i = a;
         */
        template <typename T>
        operator T() const
        {
            auto d = dynamic_cast<data<T>*>(this->_ptr);
            return d ? d->val : T();
        }

    public:
        /**
         * Test empty
         */
        bool empty() const;

        /**
         * Check type
         */
        template <typename T>
        bool check() const
        {
            auto d = dynamic_cast<data<T>*>(this->_ptr);
            return d != nullptr;
        }

        /**
         * Clear data
         */
        void clear();

    private:
        struct base
        {
            virtual ~base() = default;
            virtual base* clone() const = 0;
        };

        template <typename T>
        struct data : base
        {
            data(T v) : val(v)
            {
            }

            virtual base* clone() const override
            {
                return new data<T>(val);
            }

            T val;
        };

    private:
        /**
         * Store object, real type is data
         */
        base *_ptr = nullptr;
    };
}