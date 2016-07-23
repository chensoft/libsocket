/**
 * A wrapper to hold any type
 * @since  2016.01.12
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <type_traits>
#include <stdexcept>
#include <utility>
#include <memory>

namespace chen
{
    class any final
    {
    public:
        any() = default;
        ~any() = default;

        any(const any &o)
        {
            *this = o;
        }

        any(any &&o)
        {
            *this = std::move(o);
        }

        template <typename T>
        any(T &val) : _ptr(new data<typename std::decay<T>::type>(val))
        {
        }

        template <typename T>
        any(const T &val) : _ptr(new data<typename std::remove_cv<typename std::decay<const T>::type>::type>(val))
        {
        }

        template <typename T>
        any(T &&val) : _ptr(new data<typename std::decay<T>::type>(std::move(val)))
        {
        }

    public:
        /**
         * Copy & Move
         */
        any& operator=(const any &o)
        {
            if (this->_ptr == o._ptr)
                return *this;

            if (o._ptr)
                this->_ptr.reset(o._ptr->clone());
            else
                this->_ptr.reset();

            return *this;
        }

        any& operator=(any &&o)
        {
            if (this->_ptr == o._ptr)
                return *this;

            this->_ptr = std::move(o._ptr);

            return *this;
        }

        /**
         * Assignment
         * e.g: Any a; a = 15;
         */
        template <typename T>
        any& operator=(T &val)
        {
            this->_ptr.reset(new data<typename std::decay<T>::type>(val));
            return *this;
        }

        template <typename T>
        any& operator=(const T &val)
        {
            this->_ptr.reset(new data<typename std::remove_cv<typename std::decay<const T>::type>::type>(val));
            return *this;
        }

        template <typename T>
        any& operator=(T &&val)
        {
            this->_ptr.reset(new data<typename std::decay<T>::type>(std::move(val)));
            return *this;
        }

        /**
         * Conversion
         * e.g: Any a(15); int i = a;
         */
        template <typename T>
        operator T() const
        {
            auto d = dynamic_cast<data<T>*>(this->_ptr.get());

            if (d)
                return d->val;
            else
                throw std::runtime_error("any: bad cast");
        }

        /**
         * Reference, avoid copy
         * if you store string literal, use 'const char*' for T
         */
        template <typename T>
        T& ref()
        {
            auto d = dynamic_cast<data<T>*>(this->_ptr);

            if (d)
                return d->val;
            else
                throw std::runtime_error("any: bad cast");
        }

        template <typename T>
        const T& ref() const
        {
            return this->cref<T>();
        }

        template <typename T>
        const T& cref() const
        {
            auto d = dynamic_cast<data<T>*>(this->_ptr);

            if (d)
                return d->val;
            else
                throw std::runtime_error("any: bad cast");
        }

    public:
        /**
         * Test empty
         */
        bool empty() const
        {
            return this->_ptr == nullptr;
        }

        /**
         * Check type
         */
        template <typename T>
        bool check() const
        {
            auto d = dynamic_cast<data<T>*>(this->_ptr);
            return d != nullptr;
        }

    private:
        struct base
        {
            virtual ~base() = default;
            virtual base* clone() const = 0;
        };

        template <typename T>
        struct data : base
        {
            data(const T &v) : val(v)
            {
            }

            data(T &&v) : val(std::move(v))
            {
            }

            virtual base* clone() const override
            {
                return new data<T>(this->val);
            }

            T val;
        };

    private:
        /**
         * Store object, real type is data
         */
        std::unique_ptr<base> _ptr;
    };
}