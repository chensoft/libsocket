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

namespace chen
{
    /**
     * Helper classes
     */
    namespace iterator
    {
        // ---------------------------------------------------------------------
        // Miscellaneous
        /**
         * Proxy when use operator++(int) and operator--(int)
         * since some iterators are single-pass(e.g: istreambuf_iterator)
         * we can't simply copy erasure and return when call operator++(int)
         * so we save the current value and return a proxy to the caller instead
         */
        template <typename Value, typename Data>
        class proxy
        {
        public:
            proxy(Value keep, Data *data) : _keep(keep), _data(data->clone())
            {
            }

            ~proxy()
            {
                delete this->_data;
            }

            Value operator*() const
            {
                return this->_keep;
            }

            Data* clone() const
            {
                return this->_data->clone();
            }

        private:
            Value _keep;
            Data *_data;
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
            virtual Distance distance() const = 0;
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
            virtual Distance distance() const
            {
                // only valid in input iterator
                // use std::distance in other iterators
                return 0;
            }

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
            virtual Distance distance() const
            {
                // only valid in input iterator
                // use std::distance in other iterators
                return 0;
            }

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
                ++this->_distance;
            }

            virtual bool operator==(const super_class &o) const override
            {
                auto tmp = dynamic_cast<const impl&>(o);
                return this->_it == tmp._it;
            }

            virtual Distance distance() const override
            {
                return this->_distance;
            }

        protected:
            Iterator _it;
            Distance _distance = 0;
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


        // ---------------------------------------------------------------------
        // The erasure
        template <typename Category, typename Value, typename Reference = Value&, typename Pointer = Value*, typename Distance = std::ptrdiff_t>
        class erasure
        {
        public:
            typedef Value     value_type;
            typedef Distance  difference_type;
            typedef Pointer   pointer;
            typedef Reference reference;
            typedef Category  iterator_category;

            typedef base<Category, Value, Reference, Pointer, Distance> data_type;

        public:
            /**
             * Copy constructor
             */
            erasure(const erasure &o)
            {
                *this = o;
            }

            erasure& operator=(const erasure &o)
            {
                auto tmp = this->_data;
                this->_data = o._data->clone();

                delete tmp;
                return *this;
            }

            /**
             * Construct by proxy
             */
            erasure(const proxy<Value, data_type> &p) : _data(p.clone())
            {
            }

            /**
             * Wrap another iterator
             */
            template <typename Iterator>
            erasure(Iterator it) : _data(new impl<Category, Iterator, Value, Reference, Pointer, Distance>(it))
            {
            }

            ~erasure()
            {
                delete this->_data;
            }

        public:
            /**
             * Input & Forward iterator
             */
            reference operator*() const
            {
                return **this->_data;
            }

            erasure& operator++()
            {
                ++*this->_data;
                return *this;
            }

            proxy<Value, data_type> operator++(int)
            {
                proxy<Value, data_type> ret(**this, this->_data);
                ++*this->_data;
                return ret;
            }

            bool operator==(const erasure &o) const
            {
                return *this->_data == *o._data;
            }

            bool operator!=(const erasure &o) const
            {
                return !(*this == o);
            }

            difference_type distance() const
            {
                // since input iterator is single-pass, we can't use std::distance on it
                // however, in some cases we want to know input iterator's position, so I add this method
                // @caution don't use this method if your category is not input iterator
                return this->_data->distance();
            }

            /**
             * Bidirectional iterator
             */
            erasure& operator--()
            {
                --*this->_data;
                return *this;
            }

            proxy<Value, data_type> operator--(int)
            {
                proxy<Value, data_type> ret(**this, this->_data);
                --*this->_data;
                return ret;
            }

            /**
             * Random iterator
             */
            reference operator[](difference_type n) const
            {
                return (*this->_data)[n];
            }

            erasure& operator+=(difference_type n)
            {
                *this->_data += n;
                return *this;
            }

            erasure operator+(difference_type n) const
            {
                erasure tmp = *this;
                tmp += n;
                return tmp;
            }

            erasure& operator-=(difference_type n)
            {
                *this->_data -= n;
                return *this;
            }

            erasure operator-(difference_type n) const
            {
                erasure tmp = *this;
                tmp -= n;
                return tmp;
            }

            bool operator<(const erasure &o) const
            {
                return *this->_data < *o._data;
            }

            bool operator>(const erasure &o) const
            {
                return o < *this;
            }

            bool operator<=(const erasure &o) const
            {
                return *this->_data <= *o._data;
            }

            bool operator>=(const erasure &o) const
            {
                return o <= *this;
            }

        private:
            data_type *_data = nullptr;
        };
    }


    /**
     * Type alias(C++11)
     */
    template<typename Value, typename Reference = Value&, typename Pointer = Value*, typename Distance = std::ptrdiff_t>
    using input_iterator = iterator::erasure<std::input_iterator_tag, Value, Reference, Pointer, Distance>;

    template<typename Value, typename Reference = Value&, typename Pointer = Value*, typename Distance = std::ptrdiff_t>
    using forward_iterator = input_iterator<Value, Reference, Pointer, Distance>;

    template<typename Value, typename Reference = Value&, typename Pointer = Value*, typename Distance = std::ptrdiff_t>
    using bidirectional_iterator = iterator::erasure<std::bidirectional_iterator_tag, Value, Reference, Pointer, Distance>;

    template<typename Value, typename Reference = Value&, typename Pointer = Value*, typename Distance = std::ptrdiff_t>
    using random_iterator = iterator::erasure<std::random_access_iterator_tag, Value, Reference, Pointer, Distance>;
}