// I may improve this later.
// clang++ hello-openmp.cc -fopenmp=libomp -Wall -Wextra

#include <iostream>
#include <omp.h>

int main(int argc, char **argv)
{
    int a[100000];

    #pragma omp parallel for
    for (int i = 0; i < 100000; i++) {
        std::cerr << "nthreads" << omp_get_thread_num() << std::endl;
        a[i] = 2 * i;
    }
    std::cerr << a[100] << std::endl;

    return 0;
}
