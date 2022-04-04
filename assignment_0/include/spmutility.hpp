#ifndef SPM_UTILITY_H_
#define SPM_UTILITY_H_

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <cstdio>
#include <thread>
#include <random>
#include <barrier>
#include <utility>
#include <concepts>
#include <algorithm>
#include <functional>

namespace spm {

    struct stats {
        // how many tests have been performed
        int iterations = 0; 
        double min = 0;
        double max = 0;
        double average = 0;
    };

    template <typename T>
    concept Eq = requires(T a, T b) {
        {a == b} -> std::convertible_to<bool>;
    };

    template <typename T>
    concept Ord = Eq<T> && requires(T a, T b) {
        {a >= b} -> std::convertible_to<bool>;
        {a <= b} -> std::convertible_to<bool>;
        {a  > b} -> std::convertible_to<bool>;
        {a  < b} -> std::convertible_to<bool>;
    };

    /***
     * Timer class to measure running time of a function.
     * The class uses the RAII pattern to compute the running time.
     */
    class utimer {

        std::string message; 
        std::chrono::system_clock::time_point start;
        std::chrono::system_clock::time_point stop;

        using usecs = std::chrono::microseconds;
        using msecs = std::chrono::milliseconds;

        private:
            long* us_elapsed;
        
        public:

            utimer(const std::string&& m) : message(std::move(m)), us_elapsed(nullptr) {
                start = std::chrono::system_clock::now();
            }
                
            utimer(const std::string&& m, long* us) : message(std::move(m)), us_elapsed(us) {
                start = std::chrono::system_clock::now();
            }

            ~utimer() {
                stop = std::chrono::system_clock::now();
                std::chrono::duration<double> elapsed = stop - start;
                auto musec = std::chrono::duration_cast<usecs>(elapsed).count();
                
                std::cout << message << " computed in " << musec << " μsec " << std::endl;
                if (us_elapsed != nullptr) (*us_elapsed) = musec;
            }
    }; 

    template <typename T>
    void swap(T& a, T& b) noexcept {
        T temp = a;
        a = b;
        b = temp;
    }

    std::vector<int> gen_random_int_vector(std::size_t size, int min = 0, int max = 1000) noexcept {
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(min, max);

        std::vector<int> v{};
        v.resize(size);

        for (auto& vi: v) vi = distrib(gen);

        return v;
    }

    double speedup(double seq_time, double par_time) {
        return seq_time / par_time;
    }

    template <typename Fun, typename ...Args>
    stats test_suite(int iterations, Fun&& f, Args&& ...args) {
        // min, max, average
        return {};
    }

}

#endif