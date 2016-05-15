/**
 * Created by Jian Chen
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
        callable() = default;
        ~callable() = default;

        template <typename F>
        callable(F &&f)
        : _ptr(new data<F>(std::move(f)))
        {
        }

        callable(callable &&o);
        callable& operator=(callable &&o);

    public:
        void operator()();

    private:
        struct base
        {
            virtual ~base() = default;
            virtual void call() = 0;
        };

        template <typename F>
        struct data : base
        {
            data(F &&o) : f(std::move(o))
            {
            }

            virtual void call() override
            {
                f();
            }

            F f;
        };

    private:
        std::unique_ptr<base> _ptr;

    private:
        callable(const callable&) = delete;
        callable& operator=(const callable&) = delete;
    };
}