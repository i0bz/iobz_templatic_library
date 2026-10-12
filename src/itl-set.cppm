module;
#include <algorithm>
#include <concepts>
#include <cstddef>
#include <stdexcept>

export module iobz_containers:set;
import :rbtree;
import :constants;

namespace itl
{

    export template<std::totally_ordered T>
    class set
    {
        rbtree<T, T, true> tree;


    public:

        set(){}
        set(std::initializer_list<T> list)
        {
            std::ranges::for_each(list,[this](const auto& x){ tree.insert(x); });
        }

        bool empty() { return tree.empty(); }
        std::size_t size() { return tree.size(); }

        void clear()
        {
            tree.clear();
        }

        bool insert(const T& object)
        {

            return tree.insert(object).second;
        }

        bool erase(const T& object)
        {
            return tree.erase(object);
        }

        bool contains(const T& object)
        {
            try
            {
                tree.at(object);
                return true;
            }
            catch(const std::out_of_range& e)
            {
                return false;
            }
        }


        T operator[](std::size_t index)
        {
            if (index >= size()) throw std::out_of_range(OOB_ERR.data());

            auto usage = size();
            auto iterator = begin();
            for (std::size_t i = 0; i < index; i++)
            {
                ++iterator;
            }

            return *iterator;
        }



        //iterators
        rbtree<T, T, true>::iterator begin()
        {
            return tree.begin();
        }

        rbtree<T, T, true>::iterator end()
        {
            return tree.end();
        }

        rbtree<T, T, true>::const_iterator cbegin()
        {
            return tree.cbegin();
        }

        rbtree<T, T, true>::const_iterator cend()
        {
            return tree.cend();
        }

        rbtree<T, T, true>::reverse_iterator rbegin()
        {
            return tree.rbegin();
        }

        rbtree<T, T, true>::reverse_iterator rend()
        {
            return tree.rend();
        }

    };
}