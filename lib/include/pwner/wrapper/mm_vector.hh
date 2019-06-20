#pragma once

#include <vector>
#include <filesystem>
#include <stdio.h>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/filesystem.hpp>
#include <pwner/wrapper/temporary_file.hh>


/**
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
        _M_mfp.path = filename;
        _M_mfp.flags = boost::iostreams::mapped_file::readwrite;
    }

    [[nodiscard]] T *allocate(size_t __n) noexcept {
        if UNLIKELY(!_M_mf.is_open()) {
            if UNLIKELY(!std::filesystem::exists(_M_tf.path)) {
                if (_M_mfp.path.empty()) {
                    _M_tf.create(__n);
                    _M_mfp.path = _M_tf.path;
                } else {
                    std::ofstream of(_M_tf.path);
                }
            }
            _M_mf.open(_M_mfp);
        }
        _M_mf.resize(__n);
        return reinterpret_cast<T *>(_M_mf.data());
    }

    void deallocate([[maybe_unused]] T *p, [[maybe_unused]] size_t n) noexcept {}

private:
    temporary_file _M_tf;
    boost::iostreams::mapped_file_params _M_mfp;
    boost::iostreams::mapped_file _M_mf;
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
template<typename _Tp>
class mm_vector : public std::vector<_Tp, mmap_allocator<_Tp>> {
    typedef std::vector<_Tp, mmap_allocator<_Tp>>   _Vector;
public:
    typedef _Tp				            value_type;
    typedef mmap_allocator<_Tp>			    allocator_type;

    explicit mm_vector(std::filesystem::path filename)
    : _Vector(allocator_type(filename)), _M_filename(filename) {
        const size_t __n = std::filesystem::file_size(filename);
        this->_M_impl._M_start = this->_M_allocate(__n);
        this->_M_impl._M_finish = this->_M_impl._M_start + __n;
        this->_M_impl._M_end_of_storage = this->_M_impl._M_start + __n;
    }

    ~mm_vector() {
        std::filesystem::resize_file(_M_filename, this->size());
    }

private:
    std::filesystem::path _M_filename;
};
