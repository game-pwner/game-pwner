#pragma once

#include <vector>
#include <filesystem>
#include <stdio.h>
#include <boost/iostreams/device/mapped_file.hpp>
#include <pwner/wrapper/temporary_file.hh>


/*
 * FIXME: какого хуя оно не работает после 512 даблов?
 * mmap_allocator: works as expected to regular allocator.
 * allocated memory will be stored on disk, not in RAM.
 */
template<typename T>
class mmap_allocator {
public:
    typedef T value_type;

    explicit mmap_allocator()
    : mmap_allocator("") {}

    explicit mmap_allocator(const std::filesystem::path& filename) {
        m_mfp.path = filename;
        m_mfp.flags = boost::iostreams::mapped_file::readwrite;
    }

    [[nodiscard]] T *allocate(size_t __n) noexcept {
        if UNLIKELY(!m_mf.is_open()) {
            if UNLIKELY(!std::filesystem::exists(_M_tf.path)) {
                if (m_mfp.path.empty()) {
                    _M_tf.create(__n);
                    m_mfp.path = _M_tf.path;
                } else {
                    std::ofstream of(_M_tf.path);
                }
            }
            m_mf.open(m_mfp);
        }
        m_mf.resize(__n);
        return reinterpret_cast<T *>(m_mf.data());
    }

    void deallocate([[maybe_unused]] T *p, [[maybe_unused]] size_t n) noexcept {}

private:
    temporary_file _M_tf;
    boost::iostreams::mapped_file_params m_mfp;
    boost::iostreams::mapped_file m_mf;
};

template <class T, class U>
bool operator==(const mmap_allocator<T>&, const mmap_allocator<U>&) { return true; }

template <class T, class U>
bool operator!=(const mmap_allocator<T>&, const mmap_allocator<U>&) { return false; }


/** DONT INHERIT ON ME! -- STL Library.
 * I know that it is bad to inherit the std::vector, but if you were in my place,
 * would you have written thousands of lines to create mapped vector from scratch
 * and thousands more to make bindings for it?
 */
template<typename T>
class mm_vector : public std::vector<T, mmap_allocator<T>> {
    typedef std::vector<T, mmap_allocator<T>>   vector;
public:
    typedef T				        value_type;
    typedef mmap_allocator<T>			allocator_type;

    explicit mm_vector(std::filesystem::path filename)
    : vector(allocator_type(filename)), m_filename(filename) {
        const size_t __n = std::filesystem::file_size(filename);
        this->_M_impl._M_start = this->_M_allocate(__n);
        this->_M_impl._M_finish = this->_M_impl._M_start + __n;
        this->_M_impl._M_end_of_storage = this->_M_impl._M_start + __n;
    }

    ~mm_vector() {
        std::filesystem::resize_file(m_filename, this->size());
    }

    std::filesystem::path path() {
        return m_filename;
    }

private:
    std::filesystem::path m_filename;
};
