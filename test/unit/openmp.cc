#include <boost/test/auto_unit_test.hpp>
#include <omp.h>


BOOST_AUTO_TEST_SUITE(calendar_calc_suite)
    BOOST_AUTO_TEST_CASE(test_convert_abs)
    {
        int threads = -1;
        #pragma omp parallel
        {
            #pragma omp single
            threads = omp_get_num_threads();
        }
        BOOST_CHECK_GE(threads, 1);

        int c = 0;
        omp_set_num_threads(threads+2);
        #pragma omp parallel reduction(+:c)
        {
            c++;
        }
        BOOST_CHECK_EQUAL(threads+2, c);
        auto b = 0;
        BOOST_CHECK_EQUAL(b, 0);
    }
BOOST_AUTO_TEST_SUITE_END()
