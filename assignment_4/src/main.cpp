#include <spmutility.hpp>
#include <assignmentconfig.h>
#include <threadpool.hpp>
#include <argparse/argparse.hpp>

int main(int argc, char** argv) {

    using namespace std::chrono_literals;

    argparse::ArgumentParser program(Assignment_PROJECT_NAME);
    int nw = std::thread::hardware_concurrency();

    program.add_argument("-nw", "--parallel-degree")
        .help("Parallel degree of the program")
        .default_value(nw)
        .scan<'i', int>();

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::fprintf(stderr, "Got an exception during parsing: %s\n", err.what());
        return EXIT_FAILURE;
    }

    spm::threadpool pool(program.get<int>("-nw"));
    
    for (int i = 0; i < 16; i++) {
        pool.submit([]() -> void {
            std::cout << "Task 2\n";
        });
    }

    pool.shutdown();

    return EXIT_SUCCESS;
}