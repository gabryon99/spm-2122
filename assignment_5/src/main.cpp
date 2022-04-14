#include <spmutility.hpp>
#include <assignmentconfig.h>
#include <argparse/argparse.hpp>

#include <omp.h>

/**
 * Implementation of a farm using OpenMP. Tasks to be computed have to be provided through a for loop with iterations 
 * producing one of the input tasks and then awaiting for Ta time (parameter) before executing next iteration. 
 * The parallelism degree of the farm should be a parameter. Each task should spent some parametric amount of 
 * time (possibly using the active_wait function) to produce the final result.
 */

int main(int argc, char** argv, char** envs) {

    // Prints out: OMP_NUM_THREADS
    if (const char* env_omp = std::getenv("OMP_NUM_THREADS"))
        std::cout << "OMP_NUM_THREADS: " << env_omp << '\n';

    auto workers = omp_get_max_threads();
    std::cout << "The current workstation can use up to: " << workers << " threads!\n";

    #pragma omp parallel
    {
        #pragma omp critical
        std::cout << "Hello World from thread " << omp_get_thread_num() << "!\n";
    }
  
    return EXIT_SUCCESS;
}