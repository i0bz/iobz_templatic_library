module;
#include <concepts>


export module iobz_containers:map;
import :rbtree;


namespace itl
{

    export template<std::copyable K, std::copyable V>
    class map
    {
        rbtree<K, V> tree;

        //until i implement iterators for this the rbtree will still return 
        //a pointer, and bool and this will return bool only

    public:

        void clear()
        {
            tree.clear();
        }

        bool empty() { return tree.empty(); }
        std::size_t size() { return tree.size(); }

        bool insert(const K& key, const V& value)
        {
            return tree.insert(key,value).second;
        }

        bool erase(const K& key)
        {
            return tree.erase(key);
        }

        V& at(const K& key)
        {
            return tree.at(key);
        }

        V& operator[](const K& key)
        {
            return tree[key];
        }
    };



}