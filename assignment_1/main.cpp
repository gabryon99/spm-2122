#include <cstdio>
#include <thread>
#include <future>
#include <vector>
#include <memory> 
#include <chrono>
#include <numeric>
#include <functional>

/* Define a new type for unary functions which get an In type template and an Out type template */
template <typename Out, typename In>
using fun_unary = std::function<Out(In)>;

/***
 * Evaluate overhead needed to start/join a thread and to start an async (launch::async policy) and get the result of the computed future. 
 * Measures should be taken as averages of a large number of events. 
 * Possibly, measure times on your machine AND on the remote virtual machine
 */

void inline print_statistics(std::vector<long long>& stats, const std::size_t iters, const char* message) {

    double avg = std::accumulate(stats.begin(), stats.end(), 0LL, std::plus<long long>()) / static_cast<double>(stats.size());
    std::fprintf(stdout, "%s: %ld iterations, average %.2f museconds\n", message, iters, avg);

    return;
}

template <typename Out, typename In>
void run_thread_suite(const std::size_t iters, const fun_unary<Out, In> &fun, In arg) {

    std::vector<long long> results; 

    for (std::size_t i = 0; i < iters; i++) {

        /* Start the timer. */
        auto start = std::chrono::system_clock::now();

        /* Start a new thread */
        auto tid = std::thread(fun, arg);
        /* Join for the new thread */
        tid.join();

        /* End the timer. */
        auto elapsed = std::chrono::system_clock::now() - start;
        auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        /* Put the elapsed time into the measure array. */
        results.push_back(musec);
    }

    print_statistics(results, iters, "Overhead to start/join thread");

    return;
}

template <typename Out, typename In>
void run_async_suite(const std::size_t iters, const fun_unary<Out, In> &fun, In arg) {

    std::vector<long long> results;

    for (std::size_t i = 0; i < iters; i++) {

        /* Start the timer. */
        auto start = std::chrono::system_clock::now();

        /* Start an async */
        auto future = std::async(std::launch::async, fun, arg);
        /* Wait for its completition and discard the returned value */
        future.get();

        /* End the timer. */
        auto elapsed = std::chrono::system_clock::now() - start;
        auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        /* Put the elapsed time into the measure array. */
        results.push_back(musec);
    }

    print_statistics(results, iters, "Overhead to create/get async");

    return;
}

int main(int argc, char** argv) {

    constexpr uint64_t iters = 128;

    auto lambda_test = [](int a) -> int {
        return a;
    };

    run_thread_suite<int, int>(iters, lambda_test, 0xff);
    run_async_suite<int, int>(iters, lambda_test, 0xff);

    return 0;
}