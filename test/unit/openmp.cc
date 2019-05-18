#include <iostream>
#include <thread>
#include <unistd.h>
#include <omp.h>

int main() {
    const int threads = std::thread::hardware_concurrency();
    int c = 0;

    omp_set_num_threads(threads + 1);
    #pragma omp parallel reduction(+:c)
    {
        c++;
    }
    std::clog<<c<<" "<<threads<<std::endl;

    return (c - 1) != threads;
}
