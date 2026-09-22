module;
#include <algorithm>
#include <functional>
#include <concepts>
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
    };





}