module;
#include <concepts>
#include <cstddef>
#include <stdexcept>

export module iobz_containers:stack;
import :constants;
import :linked_list;


namespace 
{
    inline constexpr std::size_t DYNAMIC_MEMORY_THRESHOLD = 32;
    inline constexpr std::size_t CACHELINE_SIZE = 64;
    inline constexpr std::size_t cacheline_aligned_size(std::size_t type_size)
    {
        return (CACHELINE_SIZE / type_size > 4) ? CACHELINE_SIZE / type_size : 4;
    }
}



namespace itl
{
    export template<std::copyable T, std::size_t N = 0>
    class stack 
    {
        T data[N];
        std::size_t usage{};


    public:

        std::size_t size() { return usage; }
        bool full() { return usage == N; }
        bool empty() { return !usage; }

        T& peek() 
        {
            if (empty()) throw std::out_of_range(OOB_ERR.data());
            return data[usage - 1];
        }

        void push(const T& object)
        {
            if (full()) throw std::overflow_error(OVERF_ERR.data());

            data[usage++] = object;
        }

        T pop()
        {
            if (empty()) throw std::underflow_error(UNDERF_ERR.data());
            return data[--usage];
        }

    };

    export template<std::copyable T, std::size_t N>
    requires (N > DYNAMIC_MEMORY_THRESHOLD)
    class stack<T, N> 
    {

        T* data = new T[N];
        std::size_t usage{};


    public:
        ~stack()
        {
            delete [] data;
        }

        std::size_t size() { return usage; }
        bool full() { return usage == N; }
        bool empty() { return !usage; }

        T& peek() 
        {
            if (empty()) throw std::out_of_range(OOB_ERR.data());
            return data[usage - 1];
        }

        void push(const T& object)
        {
            if (full()) throw std::overflow_error(OVERF_ERR.data());

            data[usage++] = object;
        }

        T pop()
        {
            if (empty()) throw std::underflow_error(UNDERF_ERR.data());
            return data[--usage];
        }

    };


    export template<std::copyable T>
    class stack<T, 0>
    {
        linked_list<T, cacheline_aligned_size(sizeof(T))> stack_list;



    public:

        std::size_t size() { return stack_list.size(); }
        bool empty() { return stack_list.empty(); }

        T& peek() 
        {
            if (empty()) throw std::underflow_error(UNDERF_ERR.data());
            return stack_list[stack_list.size() - 1];
        }

        void push(const T& object)
        {
            stack_list.push_back(object);
        }

        T pop()
        {
            if (empty()) throw std::underflow_error(UNDERF_ERR.data());
            return stack_list.pop_back();
        }

    };



}