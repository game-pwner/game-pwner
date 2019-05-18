#include <iostream>
#include <thread>
#include <unistd.h>
#include <openacc.h>

extern int __pgi_gangidx();
extern int __pgi_workeridx();
extern int __pgi_vectoridx();
extern int __pgi_blockidx(int);
extern int __pgi_threadidx(int);

int main(void) {
    int gangs = 100;
    int *ids  = new int[gangs];

    //Ensure everything is zeroed
    for(int i=0;i<gangs;i++)
        ids[i] = 0;

#   pragma acc parallel num_gangs(gangs) copyout(ids[0:gangs])
    {
        ids[__pgi_gangidx()] = __pgi_gangidx();
    }

    for(int i=0;i<gangs;i++)
        std::cout<<ids[i]<<" ";
    std::cout<<std::endl;

    return 0;

}