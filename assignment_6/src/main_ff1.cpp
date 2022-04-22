#include <assignmentconfig.h>

#include <argparse/argparse.hpp>
#include <spmutility.hpp>

#include <ff/ff.hpp>

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

// FastFlow structure: A pipeline with a farm in the middle.

struct Emitter : ff::ff_monode_t<ull> {

    ull max = 2;
    ull workers = 4;

    Emitter(ull max, ull workers) : max{max}, workers{workers} {}

    ull *svc(ull *_) {

        int worker = 0;

        for (ull i = 2; i < max; i++) {
            ff_send_out_to(new ull(i), worker);
            worker = (worker + 1) % this->workers;
        }

        return EOS;
    }
};

struct IsPrimeStage : ff::ff_node_t<ull> {
    ull *svc(ull *task) {
        if (is_prime(*task)) {
            return task;
        } else {
            // If the number is not prime, deallocate the task
            delete task;
            // and drop it
            return GO_ON;
        }
    }
};

struct Collector : ff::ff_minode_t<ull> {
    ull *svc(ull *task) {
        std::cout << "The number: " << *task << " is prime.\n";
        // Deallocate the task
        delete task;
        // End the pipeline
        return GO_ON;
    }
};

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

    Emitter s1(max_num, nw);
    Collector s3;

    std::vector<std::unique_ptr<ff::ff_node>> workers;
    for (auto i = 0; i < nw; i++) {
        workers.push_back(std::make_unique<IsPrimeStage>());
    }

    ff::ff_Farm<IsPrimeStage> farm(std::move(workers));
    farm.add_emitter(s1);
    farm.add_collector(s3);

    if (farm.run_and_wait_end() < 0) {
        ff::error("Error on processing farm.");
    }

    return EXIT_SUCCESS;
}