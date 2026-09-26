
module;
#include <concepts>
#include <cstddef>
#include <iterator>
export module iobz_containers:iterator;

namespace itl
{
    export template<typename T, typename Derived>
    struct base_iterator
    {
        //aliases to make std::ranges work because it is standard, fck!!!
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using difference_type = std::ptrdiff_t;
        using iterator_concept = std::bidirectional_iterator_tag;


        base_iterator()
        {
            static_assert(std::default_initializable<Derived>, "Derived calasses must implement default constructor!!!");
        }

        virtual ~base_iterator() = default;
        
        virtual Derived& operator++() = 0;
        virtual Derived operator++(int) = 0;
        virtual Derived& operator--() = 0;
        virtual Derived operator--(int) = 0;
        
        
        virtual bool operator==(const Derived&) const = 0;
        virtual bool operator!=(const Derived&) const = 0;


    };


    template<typename T>
    struct contiguous_iterator : base_iterator<T, contiguous_iterator<T>>
    {

    private:
        T* element;

    public:

        contiguous_iterator() {}
        contiguous_iterator(T* ptr) : element(ptr) {}
        contiguous_iterator(const contiguous_iterator& other) : element(other.element) {}

        contiguous_iterator& operator++() override
        {
            ++element;
            return *this;
        }

        contiguous_iterator operator++(int) override
        {
            contiguous_iterator temp = *this;
            element++;
            return temp;
        }

        contiguous_iterator& operator--() override
        {
            --element;
            return *this;
        }

        contiguous_iterator operator--(int) override
        {
            contiguous_iterator temp = *this;
            element++;
            return temp;
        }

        base_iterator<T, contiguous_iterator>::reference operator*() const
        {
            return *element;
        }

        base_iterator<T, contiguous_iterator>::pointer operator->() const 
        {
            return element;
        }

        bool operator!=(const contiguous_iterator& other) const override
        {
            return element != other.element;
        }

        bool operator==(const contiguous_iterator& other) const override
        {
            return element == other.element;
        }

    };

}