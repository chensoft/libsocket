/**
 * A type erasure to hold any iterator
 * @since  2016.07.18
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 * -----------------------------------------------------------------------------
 * :-) input_iterator
 * :-) forward_iterator
 * :-) bidirectional_iterator
 * :-) random_iterator
 */
#pragma once

#include <type_traits>
#include <iterator>
#include <memory>

namespace chen
{
    /**
     * Helper classes
     */
    namespace iterator_helper
    {
        // ---------------------------------------------------------------------
        // Miscellaneous
        /**
         * Proxy when use operator++(int) and operator--(int)
         * when working with iterators like istreambuf_iterator, we can't simply copy it and return
         * we must save the current value and return a proxy to the caller
         */
        template <typename Value, typename Base>
        class proxy
        {
        public:
            proxy(Value keep, const Base &data) : _keep(keep), _data(data->clone())
            {
            }

            Value operator*() const
            {
                return this->_keep;
            }

            Base clone() const
            {
                return Base(this->_data->clone());
            }

        private:
            Value _keep;
            Base _data;
        };


        // ---------------------------------------------------------------------
        // Requirements
        template <typename Category, typename Value, typename Reference, typename Pointer, typename Distance>
        class base
        {
        };

        /**
         * Input & Forward iterator
         */
        template <typename Value, typename Reference, typename Pointer, typename Distance>
        class base<std::input_iterator_tag, Value, Reference, Pointer, Distance>
        {
        public:
            virtual ~base() = default;
            virtual base* clone() const = 0;

        public:
            virtual Reference operator*() const = 0;
            virtual void operator++() = 0;
            virtual bool operator==(const base &o) const = 0;
        };

        /**
         * Bidirectional iterator
         */
        template <typename Value, typename Reference, typename Pointer, typename Distance>
        class base<std::bidirectional_iterator_tag, Value, Reference, Pointer, Distance>
        {
        public:
            virtual ~base() = default;
            virtual base* clone() const = 0;

        public:
            virtual Reference operator*() const = 0;
            virtual void operator++() = 0;
            virtual bool operator==(const base &o) const = 0;

        public:
            virtual void operator--() = 0;
        };

        /**
         * Random iterator
         */
        template <typename Value, typename Reference, typename Pointer, typename Distance>
        class base<std::random_access_iterator_tag, Value, Reference, Pointer, Distance>
        {
        public:
            virtual ~base() = default;
            virtual base* clone() const = 0;

        public:
            virtual Reference operator*() const = 0;
            virtual void operator++() = 0;
            virtual bool operator==(const base &o) const = 0;

        public:
            virtual void operator--() = 0;

        public:
            virtual Reference operator[](Distance n) const = 0;
            virtual void operator+=(Distance n) = 0;
            virtual void operator-=(Distance n) = 0;
            virtual bool operator<(const base &o) const = 0;
            virtual bool operator<=(const base &o) const = 0;
        };


        // ---------------------------------------------------------------------
        // Implementations
        template <typename Category, typename Iterator, typename Value, typename Reference, typename Pointer, typename Distance>
        class impl
        {
        };

        /**
         * Input & Forward iterator
         */
        template <typename Iterator, typename Value, typename Reference, typename Pointer, typename Distance>
        class impl<std::input_iterator_tag, Iterator, Value, Reference, Pointer, Distance> : public base<std::input_iterator_tag, Value, Reference, Pointer, Distance>
        {
        public:
            typedef base<std::input_iterator_tag, Value, Reference, Pointer, Distance> super_class;

        public:
            impl(Iterator it) : _it(it)
            {
            }

            virtual super_class* clone() const override
            {
                return new impl(this->_it);
            }

        public:
            virtual Reference operator*() const override
            {
                return *this->_it;
            }

            virtual void operator++() override
            {
                ++this->_it;
            }

            virtual bool operator==(const super_class &o) const override
            {
                auto tmp = dynamic_cast<const impl&>(o);
                return this->_it == tmp._it;
            }

        protected:
            Iterator _it;
        };

        /**
         * Bidirectional iterator
         */
        template <typename Iterator, typename Value, typename Reference, typename Pointer, typename Distance>
        class impl<std::bidirectional_iterator_tag, Iterator, Value, Reference, Pointer, Distance> : public base<std::bidirectional_iterator_tag, Value, Reference, Pointer, Distance>
        {
        public:
            typedef base<std::bidirectional_iterator_tag, Value, Reference, Pointer, Distance> super_class;

        public:
            impl(Iterator it) : _it(it)
            {
            }

            virtual super_class* clone() const override
            {
                return new impl(this->_it);
            }

        public:
            virtual Reference operator*() const override
            {
                return *this->_it;
            }

            virtual void operator++() override
            {
                ++this->_it;
            }

            virtual bool operator==(const super_class &o) const override
            {
                auto tmp = dynamic_cast<const impl&>(o);
                return this->_it == tmp._it;
            }

        public:
            virtual void operator--() override
            {
                --this->_it;
            }

        protected:
            Iterator _it;
        };

        /**
         * Random iterator
         */
        template <typename Iterator, typename Value, typename Reference, typename Pointer, typename Distance>
        class impl<std::random_access_iterator_tag, Iterator, Value, Reference, Pointer, Distance> : public base<std::random_access_iterator_tag, Value, Reference, Pointer, Distance>
        {
        public:
            typedef base<std::random_access_iterator_tag, Value, Reference, Pointer, Distance> super_class;

        public:
            impl(Iterator it) : _it(it)
            {
            }

            virtual super_class* clone() const override
            {
                return new impl(this->_it);
            }

        public:
            virtual Reference operator*() const override
            {
                return *this->_it;
            }

            virtual void operator++() override
            {
                ++this->_it;
            }

            virtual bool operator==(const super_class &o) const override
            {
                auto tmp = dynamic_cast<const impl&>(o);
                return this->_it == tmp._it;
            }

        public:
            virtual void operator--() override
            {
                --this->_it;
            }

        public:
            virtual Reference operator[](Distance n) const override
            {
                return this->_it[n];
            }

            virtual void operator+=(Distance n) override
            {
                this->_it += n;
            }

            virtual void operator-=(Distance n) override
            {
                this->_it -= n;
            }

            virtual bool operator<(const super_class &o) const override
            {
                auto tmp = dynamic_cast<const impl&>(o);
                return this->_it < tmp._it;
            }

            virtual bool operator<=(const super_class &o) const override
            {
                auto tmp = dynamic_cast<const impl&>(o);
                return this->_it <= tmp._it;
            }

        protected:
            Iterator _it;
        };
    }

    /**
     * The iterator
     */
    template <typename Category, typename Value, typename Reference = Value&, typename Pointer = Value*, typename Distance = std::ptrdiff_t>
    class iterator
    {
    public:
        typedef Value     value_type;
        typedef Distance  difference_type;
        typedef Pointer   pointer;
        typedef Reference reference;
        typedef Category  iterator_category;

        typedef std::unique_ptr<iterator_helper::base<Category, Value, Reference, Pointer, Distance>> data_type;

    public:
        /**
         * Copy constructor
         */
        iterator(const iterator &o)
        {
            *this = o;
        }

        iterator& operator=(const iterator &o)
        {
            if (this == &o)
                return *this;

            this->_ptr.reset(o._ptr->clone());

            return *this;
        }

        /**
         * Construct by proxy
         */
        iterator(const iterator_helper::proxy<Value, data_type> &p) : _ptr(p.clone())
        {
        }

        /**
         * Wrap another iterator
         */
        template <typename Iterator>
        iterator(Iterator it) : _ptr(new iterator_helper::impl<Category, Iterator, Value, Reference, Pointer, Distance>(it))
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

        iterator& operator++()
        {
            ++(*this->_ptr);
            return *this;
        }

        iterator_helper::proxy<Value, data_type> operator++(int)
        {
            iterator_helper::proxy<Value, data_type> ret(**this, this->_ptr);
            ++(*this->_ptr);
            return ret;
        }

        bool operator==(const iterator &o) const
        {
            return *this->_ptr == *o._ptr;
        }

        bool operator!=(const iterator &o) const
        {
            return !(*this == o);
        }

        /**
         * Bidirectional iterator
         */
        iterator& operator--()
        {
            --(*this->_ptr);
            return *this;
        }

        iterator_helper::proxy<Value, data_type> operator--(int)
        {
            iterator_helper::proxy<Value, data_type> ret(**this, this->_ptr);
            --(*this->_ptr);
            return ret;
        }

        /**
         * Random iterator
         */
        reference operator[](difference_type n) const
        {
            return (*this->_ptr)[n];
        }

        iterator& operator+=(difference_type n)
        {
            (*this->_ptr) += n;
            return *this;
        }

        iterator operator+(difference_type n) const
        {
            iterator tmp = *this;
            tmp += n;
            return tmp;
        }

        iterator& operator-=(difference_type n)
        {
            (*this->_ptr) -= n;
            return *this;
        }

        iterator operator-(difference_type n) const
        {
            iterator tmp = *this;
            tmp -= n;
            return tmp;
        }

        bool operator<(const iterator &o) const
        {
            return *this->_ptr < *o._ptr;
        }

        bool operator>(const iterator &o) const
        {
            return o < *this;
        }

        bool operator<=(const iterator &o) const
        {
            return *this->_ptr <= *o._ptr;
        }

        bool operator>=(const iterator &o) const
        {
            return o <= *this;
        }

    private:
        data_type _ptr;
    };

    /**
     * Handy type alias(C++11)
     */
    template<typename Value, typename Reference = Value&, typename Pointer = Value*, typename Distance = std::ptrdiff_t>
    using input_iterator = iterator<std::input_iterator_tag, Value, Reference, Pointer, Distance>;

    template<typename Value, typename Reference = Value&, typename Pointer = Value*, typename Distance = std::ptrdiff_t>
    using forward_iterator = input_iterator<Value, Reference, Pointer, Distance>;

    template<typename Value, typename Reference = Value&, typename Pointer = Value*, typename Distance = std::ptrdiff_t>
    using bidirectional_iterator = iterator<std::bidirectional_iterator_tag, Value, Reference, Pointer, Distance>;

    template<typename Value, typename Reference = Value&, typename Pointer = Value*, typename Distance = std::ptrdiff_t>
    using random_iterator = iterator<std::random_access_iterator_tag, Value, Reference, Pointer, Distance>;
}