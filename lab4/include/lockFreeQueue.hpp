#include <atomic>
#include <memory>
#include <iostream>

template <typename T>
class threadsafe_queue {
private:
    // Node structure for queue entries
    struct Node {
        T data;
        std::atomic<Node*> next;

        explicit Node(T value) : data(value), next(nullptr) {}
    };

    // Head and tail pointers
    std::atomic<Node*> head;
    std::atomic<Node*> tail;

public:
    // Constructor: Initialize with a dummy node
    threadsafe_queue() {
        Node* dummy = new Node(T{});
        head.store(dummy);
        tail.store(dummy);
    }

    // Destructor: Clean up all remaining nodes
    ~threadsafe_queue() {
        while (Node* node = head.load()) {
            head.store(node->next);
            delete node;
        }
    }

    // Enqueue operation (lock-free)
    void push(T value) {
        Node* newNode = new Node(value);
        Node* currentTail = nullptr;

        while (true) {
            currentTail = tail.load();
            Node* tailNext = currentTail->next.load();

            // Check if tail is indeed the last node
            if (currentTail == tail.load()) {
                if (tailNext == nullptr) {
                    // Try to link new node at the end of the list
                    if (currentTail->next.compare_exchange_weak(tailNext, newNode)) {
                        // Successfully linked; now update tail
                        tail.compare_exchange_weak(currentTail, newNode);
                        return;
                    }
                } else {
                    // Tail not pointing to the last node, try to swing it forward
                    tail.compare_exchange_weak(currentTail, tailNext);
                }
            }
        }
    }

    // Dequeue operation (lock-free)
    bool try_pop(T& result) {
        while (true) {
            Node* currentHead = head.load();
            Node* currentTail = tail.load();
            Node* nextNode = currentHead->next.load();

            // Check if queue is empty
            if (currentHead == currentTail) {
                if (nextNode == nullptr) {
                    return false; // Queue is empty
                }
                // Tail is falling behind; try to move it forward
                tail.compare_exchange_weak(currentTail, nextNode);
            } else {
                // Read value before advancing head
                result = nextNode->data;

                // Attempt to move head forward
                if (head.compare_exchange_weak(currentHead, nextNode)) {
                    delete currentHead; // Free old dummy node
                    return true;
                }
            }
        }
    }

    // Check if the queue is empty
    bool empty() const {
        Node* currentHead = head.load();
        Node* currentTail = tail.load();
        return (currentHead == currentTail) && (currentHead->next.load() == nullptr);
    }
};
