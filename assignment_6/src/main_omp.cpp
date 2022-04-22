#include <assignmentconfig.h>

#include <argparse/argparse.hpp>
#include <spmutility.hpp>

#include <omp.h>

using ull = unsigned long long;

static bool is_prime(ull n) {

    if (n <= 3)
        return n > 1; // 1 is not prime !

    if (n % 2 == 0 || n % 3 == 0)
        return false;

    for (ull i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    }

    return true;
}

int main(int argc, char **argv) {

    argparse::ArgumentParser program(Assignment_PROJECT_NAME);

    int nw = std::thread::hardware_concurrency();
    int max_num = 1'000'000;

    program.add_argument("-nw", "--parallel-degree")
        .help("Parallel degree of the program")
        .scan<'i', int>();

    program.add_argument("-m", "--max-num")
        .help("Maximum number in the range (i.e. [2, m]")
        .scan<'i', int>();

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error &err) {
        std::fprintf(stderr, "Got an exception during parsing: %s\n",
                     err.what());
        return EXIT_FAILURE;
    }

    if (auto v = program.present<int>("-nw")) {
        nw = *v;
    }
    if (auto v = program.present<int>("-m")) {
        max_num = *v;
    }

    int primes = 0;

    double start = omp_get_wtime();
    #pragma omp parallel for reduction(+:primes) num_threads(nw)
    for (ull i = 2; i < max_num; i++) {
        if (is_prime(i)) {
            primes++;
        }
    }
    double elapsed = omp_get_wtime() - start;

    std::cout << "Found " << primes << " prime numbers, in " << elapsed << " seconds\n";

    return EXIT_SUCCESS;
};