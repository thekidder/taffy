#ifndef BOUNDED_QUEUE_H
#define BOUNDED_QUEUE_H

#include <list>

template<typename T>
class Bounded_queue
{
public:
    typedef typename std::list<T> Container_t;
    typedef typename std::list<T>::iterator iterator;
    typedef typename std::list<T>::const_iterator const_iterator;
    typedef typename std::list<T>::reverse_iterator reverse_iterator;
    typedef typename std::list<T>::const_reverse_iterator const_reverse_iterator;

    Bounded_queue(size_t max_size_) : max_size(max_size_) {}

    iterator begin() { return container.begin(); }
    const_iterator begin() const { return container.begin(); }

    reverse_iterator rbegin() { return container.rbegin(); }
    const_reverse_iterator rbegin() const { return container.rbegin(); }

    iterator end() { return container.end(); }
    const_iterator end() const { return container.end(); }

    reverse_iterator rend() { return container.rend(); }
    const_reverse_iterator rend() const { return container.rend(); }

    T& front() { return container.front(); }
    const T& front() const { return container.front(); }

    T& back() { return container.back(); }
    const T& back() const { return container.back(); }

    size_t size() const { return container.size(); }

    void push(const T& value)
    {
        if(container.size() == max_size)
            container.pop_front();
        container.push_back(value);
    }
private:
    Container_t container;
    size_t max_size;
};

#endif