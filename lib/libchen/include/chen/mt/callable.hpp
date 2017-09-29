/**
 * A wrapper to hold any callable object, used in threadpool
 * @since  2016.05.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 * @caution internal use only
 */
#pragma once

#include <memory>

namespace chen
{
    class callable final
    {
    public:
        callable(callable &&o) : _ptr(std::move(o._ptr))
        {
        }

        callable& operator=(callable &&o)
        {
            if (this == &o)
                return *this;

            this->_ptr = std::move(o._ptr);

            return *this;
        }

        template <typename F>
        callable(F &&f) : _ptr(new data<F>(std::move(f)))
        {
        }

    public:
        void operator()()
        {
            if (this->_ptr)
                this->_ptr->call();
        }

    private:
        struct base
        {
            virtual ~base() = default;
            virtual void call() = 0;
        };

        template <typename F>
        struct data : base
        {
            data(F &&o) : f(std::move(o)) {}

            virtual void call() override
            {
                this->f();
            }

            F f;
        };

    private:
        callable(const callable &o) = delete;
        callable& operator=(const callable &o) = delete;

    private:
        std::unique_ptr<base> _ptr;
    };
}