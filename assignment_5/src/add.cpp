#include <spmutility.hpp>
#include <assignmentconfig.h>
#include <argparse/argparse.hpp>

#include <omp.h>

std::vector<int> sum(std::vector<int>& a, std::vector<int> b) {

    std::vector<int> c(a.size());

    #pragma omp parallel for
    for (std::size_t i = 0; i < a.size(); i++) {
        c[i] = a[i] + b[i];
    }

    return c;
}

int main(int argc, char** argv, char** envs) {

    // Prints out: OMP_NUM_THREADS
    if (const char* env_omp = std::getenv("OMP_NUM_THREADS"))
        std::cout << "OMP_NUM_THREADS: " << env_omp << '\n';

    long elapsed = 0;
    auto amount = spm::get_cache_line_size() * 10'000'000;

    auto a = spm::gen_random_int_vector(amount, 0, 10000);
    auto b = spm::gen_random_int_vector(amount, 0, 10000);

    {
        spm::utimer timer("Parallel sum of elements", &elapsed);
        auto _ = sum(a, b);
    }

    std::cout << "Total amount of seconds: " << spm::utimer::to_seconds(elapsed) << "s\n";

    return EXIT_SUCCESS;
}