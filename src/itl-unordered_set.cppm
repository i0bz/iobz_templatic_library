module;
#include <cstddef>
#include <functional>
#include <stdexcept>

export module iobz_containers:unordered_set;
import :rbtree;
import :constants;



namespace itl 
{





    export template<HashKeyable T, std::size_t N = 256>
    class unordered_set
    {
        rbtree<T, T, true>* buckets;
        std::hash<T> hasher;
        std::size_t usage{};



    public:

        unordered_set() : buckets(new rbtree<T,T, true>[N]) {}
        ~unordered_set()
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
        
        std::size_t size() { return usage; }

        bool insert(const T& key)
        {
            usage++;
            return buckets[hasher(key)].insert(key).second;
        }

        bool erase(const T& key)
        {
            usage--;
            return buckets[hasher(key)].erase(key);
        }

        T& contains(const T& key)
        {
            try
            {
                buckets[hasher(key)].at(key);
                return true;
            }
            catch(const std::out_of_range& e)
            {
                return false;
            }
        }

        T operator[](std::ptrdiff_t index)
        {
            if (index >= usage || index < 0) throw std::out_of_range(OOB_ERR.data());

            std::size_t bucket_i{};
            auto chain_iterator = buckets[0].begin();

            while(index >= 0)
            {
                if (buckets[bucket_i].empty())
                {
                    bucket_i++;
                    continue;
                }

                std::ptrdiff_t last_chain_index = buckets[bucket_i].size();
                if ((index - last_chain_index) > -1)
                {
                    index -= last_chain_index;
                    bucket_i++;
                    continue;
                }


                chain_iterator = buckets[bucket_i].begin();
                while (index > 0)
                {
                    ++chain_iterator;
                    index--;
                }

                return *chain_iterator;
            }

            return *chain_iterator;
        }

    };
}