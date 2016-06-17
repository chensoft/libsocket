/**
 * A wrapper to hold any callable object, used in threadpool
 * @since  2016.05.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <memory>

namespace chen
{
    class callable final
    {
    public:
        callable() {};
        ~callable() = default;

        callable(callable &o)
        {
            *this = o;
        }

        callable(const callable &o)
        {
            *this = o;
        }

        callable& operator=(const callable &o)
        {
            if (this == &o)
                return *this;

            this->_ptr.reset(o._ptr->clone());

            return *this;
        }

        callable(callable &&o) : _ptr(std::move(o._ptr)) {}

        callable& operator=(callable &&o)
        {
            if (this == &o)
                return *this;

            this->_ptr = std::move(o._ptr);

            return *this;
        }

        template <typename F>
        callable(F &&f) : _ptr(new data<F>(std::move(f))) {}

    public:
        void operator()()
        {
            this->_ptr->call();
        }

    private:
        struct base
        {
            virtual ~base() = default;
            virtual base* clone() const = 0;
            virtual void call() = 0;
        };

        template <typename F>
        struct data : base
        {
            data(const F &o) : f(o) {}
            data(F &&o) : f(std::move(o)) {}

            virtual base* clone() const override
            {
                return new data<F>(f);
            }

            virtual void call() override
            {
                f();
            }

            F f;
        };

    private:
        std::unique_ptr<base> _ptr;
    };
}