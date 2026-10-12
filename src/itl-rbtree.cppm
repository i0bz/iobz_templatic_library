module;
#include <concepts>
#include <cstddef>
#include <stdexcept>
#include <utility>


export module iobz_containers:rbtree;
import :constants;
import :iterator;

//TODO: refactor this to remove the duplicate codes for each specializations


namespace itl
{
    
    export template <std::copyable K, std::copyable T, bool KeyIsVal = false>
    class rbtree
    {
    private:

        enum class Color
        {
            BLACK, RED
        };
        static constexpr Color RED = Color::RED;
        static constexpr Color BLACK = Color::BLACK;


        enum class Direction
        {
            LEFT, RIGHT
        };
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
            Color col{RED};

            Node(K ky, T vl, Node* pr) : key(ky), value(vl), parent(pr) {}
        };

        Node* root{};
        std::size_t usage{};



    public:
        //now for iterators
        template<typename U>
        struct inorder_iterator : base_iterator<U, inorder_iterator<U>>
        {
            Node* element{nullptr};
            Node* prev_item{nullptr};
            
            inorder_iterator() {}

            inorder_iterator(Node* node) : element(node) {}
            inorder_iterator(Node* node, Node* prev) : element(node), prev_item(prev) {}

            // list_iterator(U* ptr, Node* node, U* prev_ptr, Node* prev_res) : element(ptr), residence_node(node), prev_item(prev_ptr, prev_res) {}
            
            inorder_iterator(const inorder_iterator& other) : element(other.element), prev_item(other.prev_item) {}


            inorder_iterator& operator++() override
            {

                prev_item = element;
                element = rbtree::_inorder_successor(element);
                return *this;
            }
            
            inorder_iterator operator++(int) override
            {
                inorder_iterator temp = *this;
                ++(*this);
                return temp;
            }

            inorder_iterator& operator--() override
            {
                element = prev_item;
                prev_item = rbtree::_inorder_predecessor(prev_item);
                return *this;
            }
            
            inorder_iterator operator--(int) override
            {
                inorder_iterator temp = *this;
                --(*this);
                return temp;
            }
            
            K operator*() const
            {
                return element->value;
            }
            
            bool operator==(const inorder_iterator& other) const override
            {
                return element == other.element;
            }
            
            bool operator!=(const inorder_iterator& other) const override
            {
                return element != other.element;
            }
        };

        using iterator = inorder_iterator<T>;
        using const_iterator = inorder_iterator<const T>;
        using reverse_iterator = std::reverse_iterator<iterator>;

        iterator begin()
        {
            if (!root)
            {
                return iterator();
            }

            Node* start{root};

            while (start->children[to_id(LEFT)])
            {
                start = start->children[to_id(LEFT)];
            }

            return iterator(start);
        }

        iterator end()
        {
            if (!root)
            {
                return iterator();
            }

            Node* end{root};

            while (end->children[to_id(RIGHT)])
            {
                end = end->children[to_id(RIGHT)];
            }

            return iterator(nullptr, end);
        }

        const_iterator cbegin()
        {
            if (!root)
            {
                return const_iterator();
            }

            Node* start{root};

            while (start->children[to_id(LEFT)])
            {
                start = start->children[to_id(LEFT)];
            }

            return const_iterator(start);
        }

        const_iterator cend()
        {
            if (!root)
            {
                return const_iterator();
            }

            Node* end{root};

            while (end->children[to_id(RIGHT)])
            {
                end = end->children[to_id(RIGHT)];
            }

            return const_iterator(nullptr, end);
        }
        
        reverse_iterator rbegin()
        {
            return reverse_iterator(end());
        }

        reverse_iterator rend()
        {
            return reverse_iterator(begin());
        }





    private:

        Node* _rotate(Node* node, Direction d)
        {
            Node* parent = node->parent;
            Node* replacing_node = node->children[!to_id(d)];
            replacing_node->parent = parent;

            node->children[!to_id(d)] = replacing_node->children[to_id(d)];
            if (node->children[!to_id(d)])
            {
                node->children[!to_id(d)]->parent = node;
            }
            node->parent = replacing_node;
            replacing_node->children[to_id(d)] =  node;
            
            if (!parent)
            {
                root = replacing_node;
                return root;
            }
            if (parent->children[to_id(RIGHT)] == node)
            {
                parent->children[to_id(RIGHT)] = replacing_node;
            }
            else 
            {
                parent->children[to_id(LEFT)] = replacing_node;
            }

            return replacing_node;
        }


        //makapabagtit etoy nga part ti parparbangon yt
        //putangina 30 mins debugging hell eto
        //nakalimutan lang na recolor grandparent sa red uncle case 
        void _insertion_fix(Node* node)
        {
            while(node)
            {
                Node* parent{node->parent};

                //Root Case
                if(!parent)
                {
                    node->col = BLACK;
                    return;
                }

                //No Red Red conlict
                if (parent->col == BLACK)
                {
                    return;
                }

                Node* grandparent{parent->parent};

                //Parent is root
                if (!grandparent)
                {
                    parent->col = BLACK;
                    return;
                }

                int uncle_location{};
                Node* uncle = [&]()
                {
                    if (grandparent->children[to_id(RIGHT)] == parent)
                    {
                        uncle_location = to_id(LEFT);
                        return grandparent->children[to_id(LEFT)];
                    }
                    else 
                    {
                        uncle_location = to_id(RIGHT);
                        return grandparent->children[to_id(RIGHT)];
                    }
                }();

                
                //Red Uncle
                if (uncle && uncle->col == RED)
                {
                    uncle->col = BLACK;
                    parent->col = BLACK;
                    grandparent->col = RED;
                    node = grandparent;
                    continue;
                }


                int node_location = (parent->children[to_id(RIGHT)] == node) ? to_id(RIGHT) : to_id(LEFT);
                

                //ZigZag
                if (node_location == uncle_location)
                {
                    parent = _rotate(parent, static_cast<Direction>(!node_location));
                }
                
                //Straight ako, kasi crush ko si Dennise
                parent->col = BLACK;
                grandparent->col = RED;
                _rotate(grandparent,  static_cast<Direction>(uncle_location));
            }
        }

        void _deletion_fix(Node* node, Node* parent)
        {
            auto is_black = [](Node* n)
            {
                return !n || n->col == BLACK;
            };

            while (true)
            {
                if (!parent || !is_black(node))
                {
                    if (node)
                    {
                        node->col = BLACK;
                    }
                    return;
                }


                int node_location = (parent->children[to_id(RIGHT)] == node) ? to_id(RIGHT) : to_id(LEFT);
                
                Node* sibling = parent->children[!node_location];

                if (!is_black(sibling))
                {
                    parent->col = RED;
                    sibling->col = BLACK;
                    _rotate(parent, static_cast<Direction>(node_location));
                    sibling = parent->children[!node_location];
                }

                Node* sibling_far_child = sibling ? sibling->children[!node_location] : nullptr;
                Node* sibling_near_child = sibling ? sibling->children[node_location] : nullptr;

                if (is_black(sibling_far_child))
                {
                    if (is_black(sibling_near_child))
                    {
                        sibling->col = RED;
                        node = parent;
                        parent = node->parent;
                        continue;
                    }

                    sibling->col = RED;
                    if (sibling_near_child) 
                    {
                        sibling_near_child->col = BLACK;
                    }    
                    _rotate(sibling, static_cast<Direction>(!node_location));
                    
                    sibling = parent->children[!node_location];
                    sibling_far_child = sibling->children[!node_location];
                }

                sibling->col = parent->col;
                parent->col = BLACK;
                if (sibling_far_child)
                {
                    sibling_far_child->col = BLACK;
                }
                _rotate(parent, static_cast<Direction>(node_location));

                return;
            }
        }

        static inline Node* _child(Node* node, Direction d)
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

        static Node* _inorder_successor(Node* node)
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

        static Node* _inorder_predecessor(Node* node)
        {
            if (!node) return nullptr;

            if (_child(node, LEFT))
            {
                node = _child(node, LEFT);
                
                while (_child(node, RIGHT))
                {
                    node = _child(node, RIGHT);
                }

                return node;
            }

            auto* temp{node->parent};
            while (temp && _child(temp, RIGHT) != node)
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

        bool empty() { return !usage; }
        std::size_t size() { return usage; }

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
            usage = 0;
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
                ++usage;
                return std::pair(root, true);
            }

            auto search_ptrs = _search(key);

            if (!search_ptrs.first)
            {
                auto location = key > search_ptrs.second->key;
                search_ptrs.second->children[location] = new Node(key, obj, search_ptrs.second);
                _insertion_fix(search_ptrs.second->children[location]);
                ++usage;
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
                --usage;
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
            auto replaced_node_col = search_ptrs.first->col;
            delete search_ptrs.first;

            if (replaced_node_col == BLACK)
            {
                _deletion_fix(replacement_child, search_ptrs.second);
            }
            
            --usage;
            return true;
        }
    };

    export template <std::copyable K, std::copyable T>
    class rbtree<K, T, true>
    {
    private:


        enum class Color
        {
            BLACK, RED
        };
        static constexpr Color RED = Color::RED;
        static constexpr Color BLACK = Color::BLACK;


        enum class Direction
        {
            LEFT, RIGHT
        };
        static constexpr Direction LEFT = Direction::LEFT;
        static constexpr Direction RIGHT = Direction::RIGHT;

        static constexpr std::size_t to_id(Direction d)
        {
            return static_cast<std::size_t>(d);
        }

        struct Node
        {
            K key;
            Node* children[2];
            Node* parent{};
            Color col{RED};

            Node(K ky, Node* pr) : key(ky), parent(pr) {}
        };

        Node* root{};
        std::size_t usage{};


    public:
        //now for iterators
        template<typename U>
        struct inorder_iterator : base_iterator<U, inorder_iterator<U>>
        {
            Node* element{nullptr};
            Node* prev_item{nullptr};
            
            inorder_iterator() {}

            inorder_iterator(Node* node) : element(node) {}
            inorder_iterator(Node* node, Node* prev) : element(node), prev_item(prev) {}

            // list_iterator(U* ptr, Node* node, U* prev_ptr, Node* prev_res) : element(ptr), residence_node(node), prev_item(prev_ptr, prev_res) {}
            
            inorder_iterator(const inorder_iterator& other) : element(other.element), prev_item(other.prev_item) {}


            inorder_iterator& operator++() override
            {

                prev_item = element;
                element = rbtree::_inorder_successor(element);
                return *this;
            }
            
            inorder_iterator operator++(int) override
            {
                inorder_iterator temp = *this;
                ++(*this);
                return temp;
            }

            inorder_iterator& operator--() override
            {
                element = prev_item;
                prev_item = rbtree::_inorder_predecessor(prev_item);
                return *this;
            }
            
            inorder_iterator operator--(int) override
            {
                inorder_iterator temp = *this;
                --(*this);
                return temp;
            }
            
            K operator*() const
            {
                return element->key;
            }
            
            bool operator==(const inorder_iterator& other) const override
            {
                return element == other.element;
            }
            
            bool operator!=(const inorder_iterator& other) const override
            {
                return element != other.element;
            }
        };
        using iterator = inorder_iterator<K>;
        using const_iterator = inorder_iterator<const K>;
        using reverse_iterator = std::reverse_iterator<iterator>;

        iterator begin()
        {
            if (!root)
            {
                return iterator();
            }

            Node* start{root};

            while (start->children[to_id(LEFT)])
            {
                start = start->children[to_id(LEFT)];
            }

            return iterator(start);
        }

        iterator end()
        {
            if (!root)
            {
                return iterator();
            }

            Node* end{root};

            while (end->children[to_id(RIGHT)])
            {
                end = end->children[to_id(RIGHT)];
            }

            return iterator(nullptr, end);
        }

        const_iterator cbegin()
        {
            if (!root)
            {
                return const_iterator();
            }

            Node* start{root};

            while (start->children[to_id(LEFT)])
            {
                start = start->children[to_id(LEFT)];
            }

            return const_iterator(start);
        }

        const_iterator cend()
        {
            if (!root)
            {
                return const_iterator();
            }

            Node* end{root};

            while (end->children[to_id(RIGHT)])
            {
                end = end->children[to_id(RIGHT)];
            }

            return const_iterator(nullptr, end);
        }
        
        reverse_iterator rbegin()
        {
            return reverse_iterator(end());
        }

        reverse_iterator rend()
        {
            return reverse_iterator(begin());
        }





    private:


        Node* _rotate(Node* node, Direction d)
        {
            Node* parent = node->parent;
            Node* replacing_node = node->children[!to_id(d)];
            replacing_node->parent = parent;

            node->children[!to_id(d)] = replacing_node->children[to_id(d)];
            if (node->children[!to_id(d)])
            {
                node->children[!to_id(d)]->parent = node;
            }
            node->parent = replacing_node;
            replacing_node->children[to_id(d)] =  node;
            
            if (!parent)
            {
                root = replacing_node;
                return root;
            }
            if (parent->children[to_id(RIGHT)] == node)
            {
                parent->children[to_id(RIGHT)] = replacing_node;
            }
            else 
            {
                parent->children[to_id(LEFT)] = replacing_node;
            }

            return replacing_node;
        }


        //makapabagtit etoy nga part ti parparbangon yt
        //putangina 30 mins debugging hell eto
        //nakalimutan lang na recolor grandparent sa red uncle case 
        void _insertion_fix(Node* node)
        {
            while(node)
            {
                Node* parent{node->parent};

                //Root Case
                if(!parent)
                {
                    node->col = BLACK;
                    return;
                }

                //No Red Red conlict
                if (parent->col == BLACK)
                {
                    return;
                }

                Node* grandparent{parent->parent};

                //Parent is root
                if (!grandparent)
                {
                    parent->col = BLACK;
                    return;
                }

                int uncle_location{};
                Node* uncle = [&]()
                {
                    if (grandparent->children[to_id(RIGHT)] == parent)
                    {
                        uncle_location = to_id(LEFT);
                        return grandparent->children[to_id(LEFT)];
                    }
                    else 
                    {
                        uncle_location = to_id(RIGHT);
                        return grandparent->children[to_id(RIGHT)];
                    }
                }();

                
                //Red Uncle
                if (uncle && uncle->col == RED)
                {
                    uncle->col = BLACK;
                    parent->col = BLACK;
                    grandparent->col = RED;
                    node = grandparent;
                    continue;
                }


                int node_location = (parent->children[to_id(RIGHT)] == node) ? to_id(RIGHT) : to_id(LEFT);
                

                //ZigZag
                if (node_location == uncle_location)
                {
                    parent = _rotate(parent, static_cast<Direction>(!node_location));
                }
                
                //Straight ako, kasi crush ko si Dennise
                parent->col = BLACK;
                grandparent->col = RED;
                _rotate(grandparent,  static_cast<Direction>(uncle_location));
            }
        }

        void _deletion_fix(Node* node, Node* parent)
        {
            auto is_black = [](Node* n)
            {
                return !n || n->col == BLACK;
            };

            while (true)
            {
                if (!parent || !is_black(node))
                {
                    if (node)
                    {
                        node->col = BLACK;
                    }
                    return;
                }


                int node_location = (parent->children[to_id(RIGHT)] == node) ? to_id(RIGHT) : to_id(LEFT);
                
                Node* sibling = parent->children[!node_location];

                if (!is_black(sibling))
                {
                    parent->col = RED;
                    sibling->col = BLACK;
                    _rotate(parent, static_cast<Direction>(node_location));
                    sibling = parent->children[!node_location];
                }

                Node* sibling_far_child = sibling ? sibling->children[!node_location] : nullptr;
                Node* sibling_near_child = sibling ? sibling->children[node_location] : nullptr;

                if (is_black(sibling_far_child))
                {
                    if (is_black(sibling_near_child))
                    {
                        sibling->col = RED;
                        node = parent;
                        parent = node->parent;
                        continue;
                    }

                    sibling->col = RED;
                    if (sibling_near_child) 
                    {
                        sibling_near_child->col = BLACK;
                    }    
                    _rotate(sibling, static_cast<Direction>(!node_location));
                    
                    sibling = parent->children[!node_location];
                    sibling_far_child = sibling->children[!node_location];
                }

                sibling->col = parent->col;
                parent->col = BLACK;
                if (sibling_far_child)
                {
                    sibling_far_child->col = BLACK;
                }
                _rotate(parent, static_cast<Direction>(node_location));

                return;
            }
        }

        static inline Node* _child(Node* node, Direction d)
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

        static Node* _inorder_successor(Node* node)
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

        static Node* _inorder_predecessor(Node* node)
        {
            if (!node) return nullptr;

            if (_child(node, LEFT))
            {
                node = _child(node, LEFT);
                
                while (_child(node, RIGHT))
                {
                    node = _child(node, RIGHT);
                }

                return node;
            }

            auto* temp{node->parent};
            while (temp && _child(temp, RIGHT) != node)
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

        bool empty() { return !usage; }
        std::size_t size() { return usage; }

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
            usage = 0;
        }

        T& at(const K& key)
        {
            auto ptrs = _search(key);
            if (!ptrs.first) throw std::out_of_range(OOB_ERR.data());
            return ptrs.first->key;
        }
        

        T& operator[](const K& key)
        {
            auto res = insert(key);
            return res.first->key;
        }

        std::pair<Node*, bool> insert(const K& key)
        {
            if (!root)
            {
                root = new Node(key, nullptr);
                ++usage;
                return std::pair(root, true);
            }

            auto search_ptrs = _search(key);

            if (!search_ptrs.first)
            {
                auto location = key > search_ptrs.second->key;
                search_ptrs.second->children[location] = new Node(key, search_ptrs.second);
                _insertion_fix(search_ptrs.second->children[location]);
                ++usage;
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
                --usage;
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
            auto replaced_node_col = search_ptrs.first->col;
            delete search_ptrs.first;

            if (replaced_node_col == BLACK)
            {
                _deletion_fix(replacement_child, search_ptrs.second);
            }
            
            --usage;
            return true;
        }
    };

}