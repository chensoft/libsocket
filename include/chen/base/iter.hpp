/**
 * A type erasure to hold any iterator
 * @since  2016.07.18
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <type_traits>
#include <stdexcept>
#include <iterator>
#include <memory>

namespace chen
{
    namespace iter
    {
        /**
         * Base class
         */
        class basic
        {
        protected:
            basic() = default;
            virtual ~basic() = default;
        };

        /**
         * Adaptor class
         */
        template <typename Value, typename Reference, typename Category>
        class adaptor : public basic
        {
        public:
            typedef Value          value_type;
            typedef std::ptrdiff_t difference_type;
            typedef Reference      reference;
            typedef Value*         pointer;
            typedef Category       iterator_category;

        protected:
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
                virtual reference operator*() const
                {
                    throw std::runtime_error("iter: override me in input iterator");
                }

                // todo use template not virtual
//                virtual pointer operator->() const
//                {
//                    throw std::runtime_error("iter: override me in input iterator");
//                }

                virtual void operator++()
                {
                    throw std::runtime_error("iter: override me in input iterator");
                }

                virtual bool operator==(const base &o) const
                {
                    throw std::runtime_error("iter: override me in input iterator");
                }

                /**
                 * Bidirectional iterator
                 */
                virtual void operator--()
                {
                    throw std::runtime_error("iter: override me in bidirectional iterator");
                }

                /**
                 * Random iterator
                 */
                virtual reference operator[](difference_type n) const
                {
                    throw std::runtime_error("iter: override me in random iterator");
                }

                virtual void operator+=(difference_type n)
                {
                    throw std::runtime_error("iter: override me in random iterator");
                }

                virtual void operator-=(difference_type n)
                {
                    throw std::runtime_error("iter: override me in random iterator");
                }

                virtual bool operator<(const base &o) const
                {
                    throw std::runtime_error("iter: override me in random iterator");
                }

                virtual bool operator<=(const base &o) const
                {
                    throw std::runtime_error("iter: override me in random iterator");
                }
            };

            /**
             * Proxy when use operator++(int), operator--(int)
             */
            class proxy
            {
            public:
                proxy(Value keep, const std::unique_ptr<base> &data) : _keep(keep), _data(data->clone())
                {
                }

                Value operator*() const
                {
                    return this->_keep;
                }

                base* clone() const
                {
                    return this->_data->clone();
                }

            private:
                Value _keep;
                std::unique_ptr<base> _data;
            };

        protected:
            adaptor() = default;

            /**
             * Copy constructor
             */
            adaptor(const adaptor &o)
            {
                *this = o;
            }

            adaptor& operator=(const adaptor &o)
            {
                if (this == &o)
                    return *this;

                this->_ptr.reset(o._ptr->clone());

                return *this;
            }

            /**
             * Assign with base object
             */
            void assign(base *data)
            {
                this->_ptr.reset(data);
            }

        protected:
            std::unique_ptr<base> _ptr;
        };

        /**
         * Input iterator
         */
        template <typename Value, typename Reference = Value&, typename Category = std::input_iterator_tag>
        class input : public adaptor<Value, Reference, Category>
        {
        public:
            typedef adaptor<Value, Reference, Category> super_class;

            typedef typename super_class::value_type        value_type;
            typedef typename super_class::difference_type   difference_type;
            typedef typename super_class::reference         reference;
            typedef typename super_class::pointer           pointer;
            typedef typename super_class::iterator_category iterator_category;
            typedef typename super_class::proxy             proxy;
            typedef typename super_class::base              base;

        protected:
            input() = default;

        public:
            input(const proxy &p)
            {
                this->assign(p.clone());
            }

            /**
             * Wrap another iterator
             */
            template <typename Iterator>
            input(Iterator it)
            {
                static_assert(!std::is_base_of<basic, Iterator>::value, "iter: don't use chen::iter::basic derived class as iterator");
                static_assert(!std::is_same<proxy, Iterator>::value, "iter: don't use proxy as iterator");
                this->assign(new data_input<Iterator>(it));
            }

        public:
            /**
             * Input & Forward iterator
             */
            reference operator*() const
            {
                return this->_ptr->operator*();
            }

            // todo use template not virtual
//            pointer operator->() const
//            {
//                return this->_ptr->operator->();
//            }

            input& operator++()
            {
                ++(*this->_ptr);
                return *this;
            }

            proxy operator++(int)
            {
                proxy ret(this->operator*(), this->_ptr);
                ++(*this->_ptr);
                return ret;
            }

            bool operator==(const input &o) const
            {
                return *this->_ptr == *o._ptr;
            }

            bool operator!=(const input &o) const
            {
                return !(*this == o);
            }

        protected:
            template <typename V>
            struct data_input : base
            {
                data_input(V v) : val(v)
                {
                }

                virtual base* clone() const override
                {
                    return new data_input<V>(this->val);
                }

                /**
                 * Input & Forward iterator
                 */
                virtual reference operator*() const override
                {
                    return *this->val;
                }

                // todo use template not virtual
//                virtual pointer operator->() const override
//                {
//                    return std::addressof(*this->val);
//                }

                virtual void operator++() override
                {
                    ++this->val;
                }

                virtual bool operator==(const base &o) const override
                {
                    auto tmp = dynamic_cast<const data_input<V>&>(o);
                    return this->val == tmp.val;
                }

                V val;
            };
        };

        /**
         * Forward iterator
         */
        template <typename Value, typename Reference = Value&, typename Category = std::forward_iterator_tag>
        class forward : public input<Value, Reference, Category>
        {
        public:
            typedef input<Value, Reference, Category> super_class;

            typedef typename super_class::value_type        value_type;
            typedef typename super_class::difference_type   difference_type;
            typedef typename super_class::reference         reference;
            typedef typename super_class::pointer           pointer;
            typedef typename super_class::iterator_category iterator_category;
            typedef typename super_class::proxy             proxy;
            typedef typename super_class::base              base;

        public:
            forward(const proxy &p) : super_class(p)
            {
            }

            /**
             * Wrap another iterator
             */
            template <typename Iterator>
            forward(Iterator it) : super_class(it)
            {
            }

        protected:
            forward() = default;
        };

        /**
         * Bidirectional iterator
         */
        template <typename Value, typename Reference = Value&, typename Category = std::bidirectional_iterator_tag>
        class bidirectional : public forward<Value, Reference, Category>
        {
        public:
            typedef forward<Value, Reference, Category> super_class;

            typedef typename super_class::value_type        value_type;
            typedef typename super_class::difference_type   difference_type;
            typedef typename super_class::reference         reference;
            typedef typename super_class::pointer           pointer;
            typedef typename super_class::iterator_category iterator_category;
            typedef typename super_class::proxy             proxy;
            typedef typename super_class::base              base;

        public:
            bidirectional(const proxy &p) : super_class(p)
            {
            }

            /**
             * Wrap another iterator
             */
            template <typename Iterator>
            bidirectional(Iterator it)
            {
                static_assert(!std::is_base_of<basic, Iterator>::value, "iter: don't use chen::iter::basic derived class as iterator");
                static_assert(!std::is_same<proxy, Iterator>::value, "iter: don't use proxy as iterator");
                this->assign(new data_bidirectional<Iterator>(it));
            }

        protected:
            bidirectional() = default;

        public:
            /**
             * Bidirectional iterator
             */
            bidirectional& operator--()
            {
                --(*this->_ptr);
                return *this;
            }

            bidirectional operator--(int)
            {
                bidirectional tmp = *this;
                --(*this->_ptr);
                return tmp;
            }

        protected:
            template <typename V>
            struct data_bidirectional : super_class::template data_input<V>
            {
                data_bidirectional(V v) : super_class::template data_input<V>(v)
                {
                }

                virtual base* clone() const override
                {
                    return new data_bidirectional<V>(this->val);
                }

                /**
                 * Bidirectional iterator
                 */
                virtual void operator--() override
                {
                    --this->val;
                }
            };
        };

        /**
         * Random access iterator
         */
        template <typename Value, typename Reference = Value&, typename Category = std::random_access_iterator_tag>
        class random : public bidirectional<Value, Reference, Category>
        {
        public:
            typedef bidirectional<Value, Reference, Category> super_class;

            typedef typename super_class::value_type        value_type;
            typedef typename super_class::difference_type   difference_type;
            typedef typename super_class::reference         reference;
            typedef typename super_class::pointer           pointer;
            typedef typename super_class::iterator_category iterator_category;
            typedef typename super_class::proxy             proxy;
            typedef typename super_class::base              base;

        public:
            random(const proxy &p) : super_class(p)
            {
            }

            /**
             * Wrap another iterator
             */
            template <typename Iterator>
            random(Iterator it)
            {
                static_assert(!std::is_base_of<basic, Iterator>::value, "iter: don't use chen::iter::basic derived class as iterator");
                static_assert(!std::is_same<proxy, Iterator>::value, "iter: don't use proxy as iterator");
                this->assign(new data_random<Iterator>(it));
            }

        protected:
            random() = default;

        public:
            /**
             * Random iterator
             */
            reference operator[](difference_type n) const
            {
                return (*this->_ptr)[n];
            }

            random& operator+=(difference_type n)
            {
                (*this->_ptr) += n;
                return *this;
            }

            random operator+(difference_type n) const
            {
                random tmp = *this;
                tmp += n;
                return tmp;
            }

            random& operator-=(difference_type n)
            {
                (*this->_ptr) -= n;
                return *this;
            }

            random operator-(difference_type n) const
            {
                random tmp = *this;
                tmp -= n;
                return tmp;
            }

            bool operator<(const random &o) const
            {
                return *this->_ptr < *o._ptr;
            }

            bool operator>(const random &o) const
            {
                return o < *this;
            }

            bool operator<=(const random &o) const
            {
                return *this->_ptr <= *o._ptr;
            }

            bool operator>=(const random &o) const
            {
                return o <= *this;
            }

        protected:
            template <typename V>
            struct data_random : super_class::template data_bidirectional<V>
            {
                data_random(V v) : super_class::template data_bidirectional<V>(v)
                {
                }

                virtual base* clone() const override
                {
                    return new data_random<V>(this->val);
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
                    auto tmp = dynamic_cast<const data_random<V>&>(o);
                    return this->val < tmp.val;
                }

                virtual bool operator<=(const base &o) const override
                {
                    auto tmp = dynamic_cast<const data_random<V>&>(o);
                    return this->val <= tmp.val;
                }
            };
        };
    }
}