#include <omp.h>
#include <pwner/common.hh>


NAMESPACE_BEGIN(OMP)

class Lock {
public:
    Lock() {
        omp_init_lock(&m_lock);
    }

    virtual ~Lock() {
        omp_destroy_lock(&m_lock);
    }

    FORCE_INLINE
    void set() {
        omp_set_lock(&m_lock);
    }

    FORCE_INLINE
    void unset() {
        omp_unset_lock(&m_lock);
    }

public:
    omp_lock_t m_lock{};
};

NAMESPACE_END(OMP)
