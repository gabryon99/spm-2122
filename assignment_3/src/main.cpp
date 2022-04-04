#include <spmutility.hpp>
#include <assignmentconfig.h>
#include <argparse/argparse.hpp>

void seq_odd_even_sort(std::vector<int>& v) noexcept {
    
    auto n = v.size();
    bool sorted = false;

    while (!sorted) {
        sorted = true;
        // Odd phase
        for (std::size_t i = 1; i < n - 1; i += 2) {
            if (v[i] > v[i + 1]) {
                spm::swap(v[i], v[i + 1]);
                sorted = false;
            }
        }
        // Even phase
        for (std::size_t i = 0; i < n - 1; i += 2) {
            if (v[i] > v[i + 1]) {
                spm::swap(v[i], v[i + 1]);
                sorted = false;
            }
        }
    }
}

template <spm::Ord T>
void par_odd_even_sort(std::vector<T>& v, uint16_t nw) noexcept {
    
    using thread = std::thread;
    using range = std::pair<std::size_t, std::size_t>;

    std::barrier sync_point(nw);

    std::vector<thread> threads;
    std::vector<range> ranges;
    threads.resize(nw); ranges.resize(nw);

    // For now, let's suppose that the array size is even...
    auto n = v.size();
    auto delta = n / nw;

    auto phases = [&](const range& r) {
    
        bool sorted = false;

        while (!sorted) {

            sorted = true;

            // Odd phase
            for (std::size_t i = r.first + 1; i < r.second; i += 2) {
                if (v[i] > v[i + 1]) {
                    spm::swap(v[i], v[i + 1]);
                    sorted = false;
                }
            }
            // Wait for all the threads finishing sorting their partitions (odd)
            sync_point.arrive_and_wait();

            // Even phase
            for (std::size_t i = r.first; i < r.second; i += 2) {
                if (v[i] > v[i + 1]) {
                    spm::swap(v[i], v[i + 1]);
                    sorted = false;
                }
            }
            // Wait for all the threads finishing sorting their partitions (even)
            sync_point.arrive_and_wait();
        }

        // Thread partion is sorted, the thread can drop the barriers
        sync_point.arrive_and_drop();
    };

    for (std::size_t i = 0; i < nw; i++) {
        ranges[i] = range((i * delta), (i != nw - 1) ? (i + 1) * delta : n);
        threads[i] = thread(phases, ranges[i]);
    }

    for (auto& t : threads) t.join();

    return;
}

int main(int argc, char** argv) {

    constexpr auto DEFAULT_VECTOR_SIZE = 8;
    constexpr auto DEFAULT_PARALLEL_DEGREE = 4;

    argparse::ArgumentParser program(Assignment_PROJECT_NAME);

    program.add_argument("-s", "--array-size")
        .help("Number of elements inside the vector")
        .default_value(DEFAULT_VECTOR_SIZE)
        .scan<'i', int>();

    program.add_argument("-nw", "--parallel-degree")
        .help("Parallel degree of the program")
        .default_value(DEFAULT_PARALLEL_DEGREE)
        .scan<'i', int>();

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::fprintf(stderr, "Got an exception during parsing: %s\n", err.what());
        return EXIT_FAILURE;
    }

    auto nw = program.get<int>("-nw");
    auto vector_size = program.get<int>("-s");

    if (nw < 0) {
        std::fprintf(stderr, "The parallel degree cannot be less than zero!\n");
        return EXIT_FAILURE;
    }
    if (vector_size < 0) {
        std::fprintf(stderr, "The vector size cannot be less than zero!\n");
        return EXIT_FAILURE;
    }

    auto seq_time = 0L;
    auto par_time = 0L;
    auto v1 = spm::gen_random_int_vector(vector_size, 0, 1000);
    auto v2 = spm::gen_random_int_vector(vector_size, 0, 1000);

    {
        spm::utimer t{"Sorting a vector using seq_odd_event_sort", &seq_time};
        seq_odd_even_sort(v1);
    }

    {
        spm::utimer t{"Sorting a vector using par_odd_event_sort", &par_time};
        par_odd_even_sort(v2, static_cast<uint16_t>(nw));
    }

    std::fprintf(stdout, "Total speedup: %.2f\n", spm::speedup(static_cast<double>(seq_time), static_cast<double>(par_time)));
    std::cout << "Is v1 sorted? " << (std::is_sorted(v1.begin(), v1.end()) ? "Yes" : "No") << "\n";
    std::cout << "Is v2 sorted? " << (std::is_sorted(v2.begin(), v2.end()) ? "Yes" : "No") << "\n";

    return EXIT_SUCCESS;
}