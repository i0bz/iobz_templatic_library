module;
#include <algorithm>
#include <cstddef>
#include <functional>
#include <stdexcept>

export module iobz_containers:set;
import :rbtree;
import :constants;

namespace itl
{

    export template<CopyAndHashable T>
    class set
    {
        rbtree<std::size_t, T> tree;
        std::hash<T> hasher;


    public:

        set(){}
        set(std::initializer_list<T> list)
        {
            std::ranges::for_each(list,[this](const auto& x){ tree.insert(hasher(x), x); });
        }

        bool empty() { return tree.empty(); }
        std::size_t size() { return tree.size(); }

        void clear()
        {
            tree.clear();
        }

        bool insert(const T& object)
        {

            return tree.insert(hasher(object), object).second;
        }

        bool erase(const T& object)
        {
            return tree.erase(hasher(object));
        }

        bool contains(const T& object)
        {
            try
            {
                tree.at(hasher(object));
                return true;
            }
            catch(const std::out_of_range& e)
            {
                return false;
            }
        }



        //iterators
        rbtree<std::size_t, T>::iterator begin()
        {
            return tree.begin();
        }

        rbtree<std::size_t, T>::iterator end()
        {
            return tree.end();
        }

        rbtree<std::size_t, T>::const_iterator cbegin()
        {
            return tree.cbegin();
        }

        rbtree<std::size_t, T>::const_iterator cend()
        {
            return tree.cend();
        }

        rbtree<std::size_t, T>::reverse_iterator rbegin()
        {
            return tree.rbegin();
        }

        rbtree<std::size_t, T>::reverse_iterator rend()
        {
            return tree.rend();
        }






    };





}