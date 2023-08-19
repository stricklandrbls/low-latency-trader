#pragma once
#include <iostream>
#include <vector>
#include <atomic>
#include <thread>
#include <chrono>

/**
 * This class holds a std::vector that is the actual queue of data.
 * 
 * An atomic _i_next_[write/read] tracks the index in of the next element to be [written/read] to.
 * These need to be atomic since the reading and writing operations are performed from different threads.
*/
template <typename T>
class LFQueue final {
public:
    explicit LFQueue(size_t size): 
        _store{size, T()}
    {}

    auto get_next_writable() noexcept { return &_store[_i_next_write]; }
    auto update_write_index() noexcept {
        _i_next_write = (_i_next_write + 1) % _store.size();
        _num_elements++;
    }
    auto get_next_readable() const noexcept {
        return (_i_next_read == _i_next_write) ? nullptr : &_store[_i_next_read];
    }
    auto update_read_index() noexcept {
        _i_next_read = (_i_next_read + 1) % _store.size();
        _num_elements--;
    }
    auto size() const noexcept {
        return _num_elements.load();
    }
private:
    std::vector<T> _store;
    std::atomic<size_t> _i_next_write   {0};
    std::atomic<size_t> _i_next_read    {0};
    std::atomic<size_t> _num_elements   {0};
};

/** 
 * Great example of Scott Meyers global helper fn that increases encapsulation of an object. 
 * 
 * If this function was declared a member of the LFQueue, then it would have access to all
 * the private data members of an LFQueue.
 * 
 */
template <typename T>
void set_writable(LFQueue<T>* lfq, const T& data) {
    *(lfq->get_next_writable()) = data;
    lfq->update_write_index();
}

template <typename T>
auto get_readable(LFQueue<T>* lfq) {
    const auto next = lfq->get_next_readable();
    lfq->update_read_index();
    return next;
}

/* Test Data Struct */
struct LFQ_D_t {
    int x {0}, y {0};
};

auto consumer_fn(LFQueue<LFQ_D_t>* lfq) {
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(5s);
    
    while(lfq->size()) {
        const auto next_read = get_readable(lfq);
        std::cout << 
            "consumer read { " << 
            next_read->x << ", " <<
            next_read->y << " }\tLFQ size: " <<
            lfq->size() << "\n";
        std::this_thread::sleep_for(1s);
    }
    std::cout << "\nconsumer exiting\n";
}
