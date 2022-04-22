#include <assignmentconfig.h>

#include <argparse/argparse.hpp>
#include <grppi/grppi.h>
#include <spmutility.hpp>

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
        std::exit(EXIT_FAILURE);
    }

    if (auto v = program.present<int>("-nw")) {
        nw = *v;
    }
    if (auto v = program.present<int>("-m")) {
        max_num = *v;
    }

    auto exec_model = grppi::parallel_execution_native();

    ull current = 2;
    grppi::pipeline(
        exec_model,
        [&current, max_num]() -> grppi::optional<ull> {
            if (current < max_num) {
                return current++;
            }
            return {};
        },
        grppi::keep(is_prime),
        [](ull prime) {
            std::cout << "The number: " << prime << " is prime!\n";
        });

    return EXIT_SUCCESS;
}