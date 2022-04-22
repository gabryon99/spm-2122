#include <spmutility.hpp>
#include <assignmentconfig.h>
#include <argparse/argparse.hpp>

#include <omp.h>

void execute_farm(std::size_t par_degree, std::size_t tasks, const std::chrono::nanoseconds& ta,  const std::chrono::nanoseconds& ts) {

    #pragma omp parallel num_threads(par_degree)
    {
        #pragma omp single
        {
            // Emit new tasks
            for (std::size_t i = 0; i < tasks; i++) {

                #pragma omp task
                {
                    #pragma omp critical
                    {
                        std::cout << "Thread " << omp_get_thread_num() << ": executing task " << i << "...\n";
                    }
                    spm::active_delay(ts); // Fake the computation of a task
                    #pragma omp critical
                    {
                        std::cout << "Thread " << omp_get_thread_num() << ": task " << i << " done!\n";
                    }
                }

                // Wait to emit a new task
                std::this_thread::sleep_for(ta); // Emit a new task half a second
            }
            #pragma omp taskwait
            std::cout << "All tasks were completed!\n";
        }
    }
}

int main(int argc, char** argv) {

    using namespace std::chrono_literals;

    // Prints out: OMP_NUM_THREADS
    if (const char* env_omp = std::getenv("OMP_NUM_THREADS"))
        std::cout << "OMP_NUM_THREADS: " << env_omp << '\n';

    auto workers = omp_get_max_threads();
    std::cout << "The current node can use up to " << workers << " threads!\n";

    execute_farm(workers, 16, 250ms, 10s);

    return EXIT_SUCCESS;
}