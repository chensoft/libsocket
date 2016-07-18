/**
 * A type erasure to hold any iterator
 * @since  2016.07.18
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <memory>

namespace chen
{
    template <typename T>
    class iter final
    {
    public:
        typedef T         value_type;
        typedef ptrdiff_t difference_type;
        typedef T&        reference;
        typedef T*        pointer;

    public:
        /**
         * Copy constructor
         */
        iter(const iter &o)
        {
            *this = o;
        }

        iter& operator=(const iter &o)
        {
            if (this == &o)
                return *this;

            this->_ptr.reset(o._ptr->clone());

            return *this;
        }

        /**
         * Wrap another iterator
         */
        template <typename Iterator>
        iter(Iterator it) : _ptr(new data<Iterator>(it))
        {
        }

    public:
        /**
         * Input & Forward iterator
         */
        reference operator*() const
        {
            return this->_ptr->operator*();
        }

        pointer operator->() const
        {
            return this->_ptr->operator->();
        }

        iter& operator++()
        {
            ++(*this->_ptr);
            return *this;
        }

        iter operator++(int)
        {
            iter tmp = *this;
            ++(*this->_ptr);
            return tmp;
        }

        bool operator==(const iter &o) const
        {
            return *this->_ptr == *o._ptr;
        }

        bool operator!=(const iter &o) const
        {
            return !(*this == o);
        }

        /**
         * Bidirectional iterator
         */
        iter& operator--()
        {
            --(*this->_ptr);
            return *this;
        }

        iter operator--(int)
        {
            iter tmp = *this;
            --(*this->_ptr);
            return tmp;
        }

        /**
         * Random iterator
         */
        reference operator[](difference_type n) const
        {
            return (*this->_ptr)[n];
        }

        iter& operator+=(difference_type n)
        {
            (*this->_ptr) += n;
            return *this;
        }

        iter operator+(difference_type n) const
        {
            iter tmp = *this;
            tmp += n;
            return tmp;
        }

        iter& operator-=(difference_type n)
        {
            (*this->_ptr) -= n;
            return *this;
        }

        iter operator-(difference_type n) const
        {
            iter tmp = *this;
            tmp -= n;
            return tmp;
        }

        bool operator<(const iter &o) const
        {
            return *this->_ptr < *o._ptr;
        }

        bool operator>(const iter &o) const
        {
            return o < *this;
        }

        bool operator<=(const iter &o) const
        {
            return *this->_ptr <= *o._ptr;
        }

        bool operator>=(const iter &o) const
        {
            return o <= *this;
        }

    private:
        /**
         * Nested class to hold iterator
         */
        struct base
        {
            virtual ~base() = default;
            virtual base* clone() const = 0;

            /**
             * Input & Forward iterator
             */
            virtual reference operator*() const = 0;
            virtual pointer operator->() const = 0;
            virtual void operator++() = 0;
            virtual bool operator==(const base &o) const = 0;

            /**
             * Bidirectional iterator
             */
            virtual void operator--() = 0;

            /**
             * Random iterator
             */
            virtual reference operator[](difference_type n) const = 0;
            virtual void operator+=(difference_type n) = 0;
            virtual void operator-=(difference_type n) = 0;
            virtual bool operator<(const base &o) const = 0;
            virtual bool operator<=(const base &o) const = 0;
        };

        template <typename V>
        struct data : base
        {
            data(V v) : val(v)
            {
            }

            virtual base* clone() const override
            {
                return new data<V>(this->val);
            }

            /**
             * Input & Forward iterator
             */
            virtual reference operator*() const override
            {
                return *this->val;
            }

            virtual pointer operator->() const override
            {
                return std::addressof(operator*());
            }

            virtual void operator++() override
            {
                ++this->val;
            }

            virtual bool operator==(const base &o) const override
            {
                auto tmp = dynamic_cast<const data<V>&>(o);
                return this->val == tmp.val;
            }

            /**
             * Bidirectional iterator
             */
            virtual void operator--() override
            {
                --this->val;
            }

            /**
             * Random iterator
             */
            virtual reference operator[](difference_type n) const override
            {
                return this->val[n];
            }

            virtual void operator+=(difference_type n) override
            {
                this->val += n;
            }

            virtual void operator-=(difference_type n) override
            {
                this->val -= n;
            }

            virtual bool operator<(const base &o) const override
            {
                auto tmp = dynamic_cast<const data<V>&>(o);
                return this->val < tmp.val;
            }

            virtual bool operator<=(const base &o) const override
            {
                auto tmp = dynamic_cast<const data<V>&>(o);
                return this->val <= tmp.val;
            }

            V val;
        };

    private:
        std::unique_ptr<base> _ptr;
    };
}