#include <boost/test/auto_unit_test.hpp>
#include <pwner/process/Process.hh>
#include <pwner/wrapper/mm_vector.hh>
#include <pwner/scanner/Matches.hh>
#include <pwner/wrapper/temporary_file.hh>
// #include <pwner/scanner/ValueScanner.hh>
#include <ostream>
#include <cmath>
#include <boost/interprocess/allocators/allocator.hpp>


struct constrable_check {
    static size_t counter;

    constrable_check() {
        counter++;
    }
    constrable_check(const constrable_check&) {
        counter++;
    }
    ~constrable_check() {
        counter--;
    }
};
size_t constrable_check::counter = 0;


BOOST_AUTO_TEST_SUITE(scanner)

BOOST_AUTO_TEST_CASE(temporary_file_functional)
{
    std::filesystem::path p1;
    {
        temporary_file tf;
        BOOST_CHECK_EQUAL(tf.path.empty(), true);
        BOOST_CHECK_EQUAL(bool(tf), false);

        tf.create();
        p1 = tf.path;
        BOOST_CHECK_EQUAL(tf.path.empty(), false);
        BOOST_CHECK_EQUAL(bool(tf), true);
        BOOST_CHECK_EQUAL(std::filesystem::file_size(tf.path), 0);

        tf.destroy();
        BOOST_CHECK_EQUAL(tf.path.empty(), true);
        BOOST_CHECK_EQUAL(bool(tf), false);
        BOOST_CHECK_EQUAL(std::filesystem::exists(tf.path), false);

        tf.create();
    }
    BOOST_CHECK_EQUAL(std::filesystem::exists(p1), false);
}

BOOST_AUTO_TEST_CASE(mmap_allocator_functional)
{
    {
        std::vector<char, mmap_allocator<char>> v;
        BOOST_CHECK_EQUAL(v.capacity(), 0);
        BOOST_CHECK_EQUAL(v.size(), 0);
        for(char c : v) {
            BOOST_ERROR("v should be empty, but have "+HEX(c));
            break;
        }

        v.push_back('0');
        BOOST_CHECK_EQUAL(v.capacity(), 1);
        BOOST_CHECK_EQUAL(v.size(), 1);
        BOOST_CHECK_EQUAL(v[0], '0');

        v.push_back('1');
        BOOST_CHECK_EQUAL(v.capacity(), 2);
        BOOST_CHECK_EQUAL(v.size(), 2);
        BOOST_CHECK_EQUAL(v[0], '0');
        BOOST_CHECK_EQUAL(v[1], '1');

        v.insert(v.end(), {'a', 'b'});
        v.emplace_back(' ') = 'c';
        BOOST_CHECK_EQUAL(v.capacity(), 8);
        BOOST_CHECK_EQUAL(v.size(), 5);
        BOOST_CHECK_EQUAL(memcmp(v.data(), "01abc", v.size()), 0);
        BOOST_CHECK_EQUAL(memcmp(v.data(), "01abc\0\0\0", v.capacity()), 0);

        v.resize(v.size()-1);
        BOOST_CHECK_EQUAL(v.capacity(), 8);
        BOOST_CHECK_EQUAL(v.size(), 4);
        BOOST_CHECK_EQUAL(memcmp(v.data(), "01ab", v.size()), 0);

        v.shrink_to_fit();
        BOOST_CHECK_EQUAL(v.capacity(), 4);
        BOOST_CHECK_EQUAL(v.size(), 4);
        BOOST_CHECK_EQUAL(memcmp(v.data(), "01ab", v.size()), 0);

        v.erase(std::remove_if(v.begin(),
                               v.end(),
                               [](char x) -> bool { return std::isdigit(x); }),
                v.end());
        BOOST_CHECK_EQUAL(memcmp(v.data(), "ab", v.size()), 0);
    }

    /// check constrable_check
    {
        using T = constrable_check;
        {
            std::vector<T> v;
            v.emplace_back();
            BOOST_REQUIRE_EQUAL(constrable_check::counter, 1);
            v.emplace_back();
            BOOST_REQUIRE_EQUAL(constrable_check::counter, 2);
        }
        BOOST_REQUIRE_EQUAL(constrable_check::counter, 0);

        {
            std::vector<T, mmap_allocator<T>> v;
            v.emplace_back();
            BOOST_CHECK_EQUAL(constrable_check::counter, 1);
            v.emplace_back();
            BOOST_CHECK_EQUAL(constrable_check::counter, 2);
        }
        BOOST_CHECK_EQUAL(constrable_check::counter, 0);
    }
}


BOOST_AUTO_TEST_CASE(mm_vector_functional)
{
    {
        using T = char;
        std::filesystem::path p = std::filesystem::current_path()/"scanner__mm_vector_functional";
        {
            std::ofstream os(p);
            if (os)
                os<<"0123";
        }
        mm_vector<T> v(p);
        BOOST_CHECK_EQUAL(v.capacity(), 4);
        BOOST_CHECK_EQUAL(v.size(), 4);

        v.emplace_back('4');
        BOOST_CHECK_EQUAL(v.capacity(), 8);
        BOOST_CHECK_EQUAL(v.size(), 5);
        BOOST_CHECK_EQUAL(memcmp(v.data(), "01234", v.size()), 0);

        {
            std::ifstream is(p);
            std::string s((std::istreambuf_iterator<char>(is)),
                            std::istreambuf_iterator<char>());
            BOOST_CHECK_EQUAL(memcmp(s.c_str(), "01234\0\0\0", s.size()), 0);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END() //scanner
