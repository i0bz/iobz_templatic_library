module;
#include <algorithm>
#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>


export module iobz_containers:list;
import :constants;
import :iterator;
namespace 
{
    inline constexpr std::size_t DEFAULT_LIST_NODE_SIZE = 4;
}

namespace itl
{

    export template<std::copyable T, std::size_t N = DEFAULT_LIST_NODE_SIZE>
    class list
    {
    public:
        template<typename U>
        struct list_iterator;
        using iterator = list_iterator<T>;
        using const_iterator = list_iterator<const T>;
    
    private:

        struct Node
        {
            T data[N];
            std::size_t node_usage{};
            Node* next{};
            Node* prev{};

            bool full() const { return node_usage == N; }
            bool empty() const { return  !node_usage;}
        };

        Node* head{};
        Node* tail{};
        std::size_t list_usage{};

        void _insert_empty(const T& object)
        {
            head = tail = new Node({object}, 1 );
            list_usage++;
        }
        void _stitch(Node* node_1, Node* node_2)
        {
            node_1->next = node_2;
            node_2->prev = node_1;
        }
        void _balance(Node* node)
        {
            if (node->node_usage >= (N / 4)) return;

            if (node->empty())
            {
                _stitch(node->prev, node->next);
                delete node;
                return;
            }

            if (node->prev->node_usage + node->node_usage <= N)
            {
                std::copy_n(node->data, node->node_usage, &node->prev->data[node->prev->node_usage]);
                node->prev->node_usage += node->node_usage;
                _stitch(node->prev, node->next);
                delete node;
            }
            else if (node->next->node_usage + node->node_usage <= N)
            {
                std::rotate(node->next->data, &node->next->data[N - node->node_usage], &node->next->data[N]);
                std::copy_n(node->data, node->node_usage, node->next->data);
                node->next->node_usage += node->node_usage;
                _stitch(node->prev, node->next);
                delete node;
            }

        }



    public:

        list() {};
        
        list(const std::initializer_list<T>& init)
        {
            std::ranges::for_each(init, [this](const T& x) { push_back(x); });
        }
        
        list(const list& other)
        {
            std::ranges::for_each(other, [this](const T& x) { push_back(x); });
        }

        ~list()
        {
            while (head)
            {
                auto temp = head;
                head = head->next;
                delete temp;
            }
        }

        std::size_t size() const { return list_usage; }
        
        bool empty() const { return !list_usage; }

        iterator begin() 
        { 
            if (!head) return iterator(nullptr, nullptr);

            return  iterator(head->data, head);
        }

        iterator end()
        {
            return iterator(nullptr, nullptr);
        }

        const_iterator cbegin() const
        { 
            if (!head) return iterator(nullptr, nullptr);

            return  iterator(head->data, head);
        }

        const_iterator cend() const
        {
            return iterator(nullptr, nullptr);
        }


        T& operator[](std::size_t index)
        {
            if (index > list_usage) throw std::out_of_range(OOB_ERR.data());

            auto temp = head;

            while (index >= temp->node_usage)
            {
                index -= temp->node_usage;
                temp = temp->next;
            }

            return temp->data[index];
        }


        void push_front(const T& object)
        {
            if (!head)
            {
                _insert_empty(object);
                return;
            }

            if (head->full())
            {
                head = new Node({object}, 1, head);
                head->next->prev = head;
                list_usage++;
                return;
            }

            std::rotate(head->data, &head->data[N - 1], &head->data[N]);
            head->data[0] = object;
            list_usage++;
            head->node_usage++;
        }

        void push_back(const T& object)
        {
            if (!head)
            {
                _insert_empty(object);
                return;
            }

            if (tail->full())
            {
                tail->next = new Node({object}, 1, nullptr, tail);
                tail = tail->next;

                list_usage++;
                return;
            }

            tail->data[tail->node_usage++] = object;
            list_usage++;
        }

        T pop_front()
        {
            if (!head) throw std::underflow_error(UNDERF_ERR.data());

            T object = head->data[0];
            head->node_usage--;
            list_usage--;

            if (head->empty() && head == tail)
            {
                delete head;
                head = tail = nullptr;
                return object;
            }

            if (head->empty())
            {
                auto temp = head;
                head = head->next;
                delete temp;
                return object;
            }

            std::rotate(head->data, &head->data[1], &head->data[N]);
            return object;
        }

        T pop_back()
        {
            if (!tail) throw std::underflow_error(UNDERF_ERR.data());

            T object = tail->data[--tail->node_usage];
            list_usage--;

            if (tail->empty() && head == tail)
            {
                delete head;
                head = tail = nullptr;
                return object;
            }

            if (tail->empty())
            {
                tail = tail->prev;
                delete tail->next;
                tail->next = nullptr;

                return object;
            }

            return object;
        }

        T remove(std::size_t index)
        {
            if (!head) throw std::underflow_error(UNDERF_ERR.data());

            if (index > list_usage) throw std::out_of_range(OOB_ERR.data());

            if (index == 0)
            {
                return pop_front();
            }

            if (index == list_usage)
            {
                return pop_back();
            }

            auto temp = head;

            while (index >= temp->node_usage)
            {
                index -= temp->node_usage;
                temp = temp->next;
            }
            auto object = temp->data[index];
            std::rotate(&temp->data[index], &temp->data[index + 1], &temp->data[N]);
            temp->node_usage--;
            list_usage--;

            _balance(temp);

            return object;
        }

        void insert(const T& object, std::size_t index)
        {
            if (index > list_usage) throw std::out_of_range(OOB_ERR.data());

            if (index == 0)
            {
                push_front(object);
                return;
            }

            if (index == list_usage)
            {
                push_back(object);
                return;
            }

            auto temp = head;

            while (index >= temp->node_usage)
            {
                index -= temp->node_usage;
                temp = temp->next;
            }

            if (temp->full())
            {
                temp->next = new Node({}, N - index, temp->next, temp);
                std::copy_n(&temp->data[index], N - index, temp->next->data);
                temp->data[index] = object;
                temp->node_usage = ++index;
                temp->next->next->prev = temp->next;
                list_usage++;

                _balance(temp);
                return;
            }

            std::rotate(&temp->data[index], &temp->data[N - 1], &temp->data[N]);
            temp->data[index] = object;
            temp->node_usage++;
            list_usage++;

        }
        
    };


















    template <std::copyable T, std::size_t N>
    template<typename U>
    struct list<T,N>::list_iterator : base_iterator<U, list_iterator<U>>
    {
        U* element;
        Node* residence_node;
        
        list_iterator() {}
        list_iterator(U* ptr, Node* node) : element(ptr), residence_node(node) {}
        list_iterator(const list_iterator& other) : element(other.element), residence_node(other.residence_node) {}


        list_iterator& operator++() override
        {
            if (++element >= residence_node->data + residence_node->node_usage)
            {
                residence_node = residence_node->next;
                if (!residence_node)
                {
                    element = nullptr;
                } 
                else
                {
                    element = residence_node->data;
                }
            }
            return *this;
        }
        
        list_iterator operator++(int) override
        {
            list_iterator temp = *this;
            ++(*this);
            return temp;
        }

        list_iterator& operator--() override
        {
            if (--element < residence_node->data)
            {
                residence_node = residence_node->prev;
                if (!residence_node)
                {
                    element = nullptr;
                } 
                else
                {
                    element = residence_node->data + residence_node->node_usage - 1;
                }
            }
            return *this;
        }
        
        list_iterator operator--(int) override
        {
            list_iterator temp = *this;
            --(*this);
            return temp;
        }
        
        base_iterator<U, list_iterator>::reference operator*() const override
        {
            return *element;
        }

        base_iterator<U, list_iterator>::pointer operator->() const override
        {
            return element;
        }
        
        bool operator==(const list_iterator& other) const override
        {
            return element == other.element;
        }
        
        bool operator!=(const list_iterator& other) const override
        {
            return element != other.element;
        }
    };
}
