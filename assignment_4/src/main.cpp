#include <assignmentconfig.h>

#include <argparse/argparse.hpp>
#include <spmutility.hpp>
#include <threadpool.hpp>

int main(int argc, char **argv) {
    using namespace std::chrono_literals;

    argparse::ArgumentParser program(Assignment_PROJECT_NAME);
    auto nw = std::thread::hardware_concurrency();

    program.add_argument("-nw", "--parallel-degree")
        .help("Parallel degree of the program")
        .default_value(nw)
        .scan<'i', int>();

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error &err) {
        std::fprintf(stderr, "Got an exception during parsing: %s\n",
                     err.what());
        return EXIT_FAILURE;
    }

    spm::threadpool pool(program.get<int>("-nw"));
    auto task_3s = pool.submit(
        [](int time) -> int {
            std::this_thread::sleep_for(1s * time);
            return time;
        },
        3);

    std::cout << "Value got: " << task_3s->get() << "\n";

    pool.shutdown();

    return EXIT_SUCCESS;
}