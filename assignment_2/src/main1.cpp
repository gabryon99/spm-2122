#include <cstdio>
#include <thread>
#include <vector>
#include <functional>

using float_vector = std::vector<float>;
using thread_vector = std::vector<std::thread>;
using float_float_fun = std::function<float(float)>;

enum class spm_mode {
    chunk,
    cyclic
};

float_vector map(const float_vector& v, float_float_fun f, spm_mode mode, int nw) {

    float_vector result;
    thread_vector threads;

    auto m = v.size();

    result.resize(m); threads.resize(nw);

    switch (mode) {
        case spm_mode::chunk: {

            auto lambda = [&](int start, int end) {
                for (std::size_t i = start; i <= end; i++) {
                    result[i] = f(v[i]);
                }
            };

            int delta = (m / nw);
            int temp = 0;

            for (std::size_t i = 0 ; i < nw - 1; i++) {
                threads[i] = std::thread(lambda, temp, temp + delta - 1);
                temp += delta;
            }
            threads[nw - 1] = std::thread(lambda, temp, m - 1);

            break;
        }
        case spm_mode::cyclic: {
            
            auto lambda = [&](int index) {
                for (std::size_t i = index; i < m; i += nw) {
                    result[i] = f(v[i]);
                }
            };

            for (std::size_t i = 0; i < nw; i++) {
                threads[i] = std::thread(lambda, i);
            }

            for (std::size_t i = 0; i < nw; i++) {
                threads[i].join();
            }

            break;
        }
        default: {
            break;
        }
    }


    return result;
}

int main(void) {
    
    float_vector v1 = {1.0, 2.0, 3.0, 4.0};
    auto v2 = map(v1, [](float x) {return 2 * x; }, spm_mode::cyclic, 2);

    for (auto& v : v2) {
        std::fprintf(stdout, "%.2f ", v);
    }

    return 0;
}