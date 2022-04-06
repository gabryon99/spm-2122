#ifndef SPM_UNBOUNDED_QUEUE_H
#define SPM_UNBOUNDED_QUEUE_H

#include <queue>

namespace spm {
template <typename T>
class unbounded_queue {
   private:
    std::queue<T> queue;
    std::mutex queue_lock;
    std::condition_variable cv;

   public:
    void enqueue(const T &&e) noexcept {
        std::lock_guard<std::mutex> lock(queue_lock);
        // Insert the element in the queue
        queue.push(std::move(e));
        cv.notify_all();
    }

    T dequeue() noexcept {
        std::unique_lock<std::mutex> lock(queue_lock);

        // Wait until the queue is not empty
        cv.wait(lock, [&]() { return !queue.empty(); });

        // Someone inserted an element, we can pop it from the queue
        auto front = queue.front();
        // Remove the element from the queue
        queue.pop();

        return front;
    }
};
}  // namespace spm

#endif