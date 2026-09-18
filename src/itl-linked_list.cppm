module;
#include <algorithm>
#include <concepts>
#include <cstddef>
#include <stdexcept>


export module iobz_containers;

namespace {
    inline constexpr std::size_t DEFAULT_LIST_NODE_SIZE = 4;
}

namespace itl
{


    export template<std::copyable T, std::size_t N = DEFAULT_LIST_NODE_SIZE>
    class linked_list
    {
        struct Node
        {
            T data[N];
            std::size_t node_usage{};
            Node* next{};
            Node* prev{};

            bool full() { return node_usage == N; }
            bool empty() { return  !node_usage;}
        };

        Node* head{};
        Node* tail{};
        std::size_t list_usage{};

        void _insert_empty(const T& object)
        {
                head = tail = new Node({object}, 1 );
                list_usage++;
        }

    public:

        std::size_t size() { return list_usage; }

        T& operator[](std::size_t index)
        {
            if (index > list_usage) throw std::out_of_range("Out of Bounds Access!!!");

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

            std::rotate(head->data, head->data + N - 1, head->data + N);
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
            if (!head) throw std::underflow_error("Empty List!!!");

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

            std::rotate(head->data, head->data + 1, head->data + N);
            return object;
        }

        T pop_back()
        {
            if (!tail) throw std::underflow_error("Empty List!!!");

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






    };



















}
