module;
#include <concepts>
#include <cstddef>
#include <functional>

export module iobz_containers:unordered_map;
import :constants;
import :rbtree;


//TODO implement iterators and a rehashing and resizing of the buckets

namespace itl
{

    export template<HashKeyable K, std::copyable V, std::size_t N = 256>
    class unordered_map
    {
        rbtree<K, V>* buckets;
        std::hash<K> hasher;
        std::size_t usage{};


    public:

        unordered_map() : buckets(new rbtree<K,V>[N]) {}
        ~unordered_map()
        {
            clear();
            delete [] buckets;
        }

        void clear()
        {
            for (int i = 0; i < N; i++)
            {
                buckets[i].clear();
            }
        }

        bool empty() { return !usage; }

        bool insert(const K& key, const V& value)
        {
            return buckets[hasher(key)].insert(key,value).second;
        }

        bool erase(const K& key)
        {
            return buckets[hasher(key)].erase(key);
        }

        V& at(const K& key)
        {
            return buckets[hasher(key)].at(key);
        }

        V& operator[](const K& key)
        {
            return buckets[hasher(key)][key];
        }


    };




}