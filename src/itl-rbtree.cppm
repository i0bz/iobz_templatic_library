module;
#include <concepts>
#include <stdexcept>
#include <utility>


export module iobz_containers:rbtree;
import :constants;

//TODO add red black properties and balancing later you lazy fuck 


namespace itl
{
    
    export template <std::copyable K, std::copyable T>
    class rbtree
    {
        enum class Direction;
        static constexpr Direction LEFT = Direction::LEFT;
        static constexpr Direction RIGHT = Direction::RIGHT;

        static constexpr std::size_t to_id(Direction d)
        {
            return static_cast<std::size_t>(d);
        }

        struct Node
        {
            K key;
            T value;
            Node* children[2];
            Node* parent{};

            Node(K ky, T vl, Node* pr) : key(ky), value(vl), parent(pr) {}
        };

        Node* root{};

        inline Node* _child(Node* node, Direction d)
        {
            if (!node) return nullptr;
            return node->children[to_id(d)];
        }

        std::pair<Node*, Node*> _search(const K& key)
        {
            Node* first_ptr{root};
            Node* second_ptr{};
 
            while (first_ptr && first_ptr->key != key)
            {
                second_ptr = first_ptr;
                if (key > first_ptr->key)
                {
                    first_ptr = _child(first_ptr, RIGHT);
                }
                else
                {
                    first_ptr = _child(first_ptr, LEFT);
                }
            }
            return std::pair(first_ptr, second_ptr);
        }

        Node* _inorder_successor(Node* node)
        {
            if (!node) return nullptr;

            if (_child(node, RIGHT))
            {
                node = _child(node, RIGHT);
                
                while (_child(node, LEFT))
                {
                    node = _child(node, LEFT);
                }

                return node;
            }

            auto* temp{node->parent};
            while (temp && _child(temp, LEFT) != node)
            {
                node = temp;
                temp = temp->parent;
            }

            return temp;
        }


    public:

        ~rbtree()
        {
            clear();
        }

        void clear()
        {
            Node* temp_1{root};
            Node* temp_2{};
            
            while (temp_1)
            {
                temp_2 = temp_1;

                auto successor = _inorder_successor(temp_1);
                if (successor)
                {
                    successor->children[to_id(LEFT)] = _child(temp_1, LEFT);
                }
                
                temp_1 = _child(temp_1, RIGHT);
                delete temp_2;
            }
            
            root = nullptr;
        }

        T& at(const K& key)
        {
            auto ptrs = _search(key);
            if (!ptrs.first) throw std::out_of_range(OOB_ERR.data());
            return ptrs.first->value;
        }
        

        T& operator[](const K& key)
        {
            auto res = insert(key, T());
            return res.first->value;
        }

        std::pair<Node*, bool> insert(const K& key, const T& obj)
        {
            if (!root)
            {
                root = new Node(key, obj, nullptr);
                return std::pair(root, true);
            }

            auto search_ptrs = _search(key);

            if (!search_ptrs.first)
            {
                auto location = key > search_ptrs.second->key;
                search_ptrs.second->children[location] = new Node(key, obj, search_ptrs.second);
                return std::pair(search_ptrs.second->children[location], true);
            }

            return std::pair(search_ptrs.first, false);
        }

        bool erase(const K& key)
        {
            if (!root)
            {
                return false;
            }

            auto search_ptrs = _search(key);

            if (!search_ptrs.first)
            {
                return false;
            }

            if (_child(search_ptrs.first, LEFT) && _child(search_ptrs.first, RIGHT))
            {
                Node* successor = _inorder_successor(search_ptrs.first);
                search_ptrs.first->value = successor->value;
                search_ptrs.first->key = successor->key;
                search_ptrs.first = successor;
                search_ptrs.second = successor->parent;
            }

            auto replacement_child = (_child(search_ptrs.first, LEFT)) ? _child(search_ptrs.first, LEFT) : _child(search_ptrs.first, RIGHT);
            

            if (replacement_child)
            {
                replacement_child->parent = search_ptrs.second;
            }

            if (!search_ptrs.second)
            {
                root = replacement_child;
                delete search_ptrs.first;
                return true;
            }

            if (_child(search_ptrs.second, LEFT) == search_ptrs.first)
            {
                search_ptrs.second->children[to_id(LEFT)] = replacement_child;
            }
            else 
            {
                search_ptrs.second->children[to_id(RIGHT)] = replacement_child;
            }
            delete search_ptrs.first;
            
            return true;
        }
    };



    template<std::copyable K, std::copyable T>
    enum class rbtree<K, T>::Direction
    {
        LEFT,
        RIGHT
    };

}