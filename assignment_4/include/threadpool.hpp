#ifndef SPM_THREADPOOL_H
#define SPM_THREADPOOL_H

#include <optional>

#include "spmutility.hpp"
#include "unbounded_queue.hpp"

namespace spm {

    class threadpool {

    using uint = unsigned int;
    using thread = std::thread;

    /// The task type is an aliasing of an optional void function.
    /// The empty task (std::nullopt) signals the end of the stream.
    using task = std::optional<std::function<void()>>;
        
    template <typename T> using vector = std::vector<T>;
    template <typename T> using uqueue = spm::unbounded_queue<T>;

    private:

        /// Number of workers that threadpool will use.
        uint nw = thread::hardware_concurrency();

        vector<thread> threads;
        uqueue<task > tasks;

        void initialize() {
            // Initialize the threads
            for (std::size_t i = 0; i < this->nw; i++) {
                threads.emplace_back([&]() -> void { this->loop(); });
            }
        }

        void loop() {
            while (true) {
                // Fetch task from queue
                if (auto next_task = tasks.dequeue()) {
                    (*next_task)(); // Perform the task
                }
                else {
                    // An empty task is the ending of stream message.
                    // Break the loop for the poison pill
                    break;
                }
            }
        }

    public:

        threadpool() { initialize(); }

        explicit threadpool(uint _nw) : nw{_nw} {
            initialize();
        }

        template <typename Out, typename... In>
        void submit(Out&& fun, In&&... args) noexcept {
            auto bind = std::bind(fun, std::forward<In>(args)...);
            tasks.enqueue(task{bind});
        }

        void shutdown() noexcept {
            // Request the shutdown
            // Inject a poison pill in the queue
            for (std::size_t i = 0; i < threads.size(); i++) tasks.enqueue(std::nullopt);
            // Wait for all threads terminating their execution...
            for (auto& t : threads) t.join();
        }

    };
}

#endif