//
// Created by root on 05.05.19.
//

#pragma once

#include <omp.h>
#include <pwner/wrapper/OMP.hh>
#include <ostream>

template <typename _Type>
class Generator {
public:
    Generator() = default;
    Generator(const Generator<_Type>& rhs) {
        this = &rhs;
    };

    virtual _Type next() = 0;
    virtual void fill(std::vector<_Type>& memory) = 0;
    virtual std::string str() const = 0;

    friend std::ostream &operator<<(std::ostream &os, const Generator &generator) {
        return os << generator.str();
    }
};


template <typename _Type>
class GeneratorFlat : public Generator<_Type> {
public:
    GeneratorFlat()
    : Generator<_Type>() {
        this->c = std::numeric_limits<_Type>::min();
    }

    explicit GeneratorFlat(_Type c) : c(c) { }

    _Type next() override {
        return c;
    }

    void fill(std::vector<_Type>& memory) override {
        std::fill(std::begin(memory), std::end(memory), c);
    }

    std::string str() const {
        return "flat";
    }

private:
    _Type c;
};


template <typename _Type>
class GeneratorSequential : public Generator<_Type> {
public:
    GeneratorSequential()
    : Generator<_Type>() {
        // this->c = std::numeric_limits<_Type>::min();
        this->c = std::numeric_limits<_Type>::max() - 2;
    }


    _Type next() override {
        return static_cast<_Type>(c++);
    }

    void fill(std::vector<_Type>& memory) override {
        // for (auto& v : memory)
        //     v = c++;
        size_t sz = memory.size();
#       pragma omp parallel for firstprivate(c)
        for (size_t i = 0; i < sz; i++)
            memory[i] = static_cast<_Type>(i+c);
        c += static_cast<_Type>(sz);
    }

    std::string str() const {
        return "sequential";
    }

private:
    _Type c;
};


template <typename _Type>
class GeneratorRandom : public Generator<_Type> {
public:
    GeneratorRandom()
    : Generator<_Type>(), engine(r()), uniform_dist(std::numeric_limits<_Type>::min(), std::numeric_limits<_Type>::max()) { }

    _Type next() override {
        return static_cast<_Type>(engine());
    }

    void fill(std::vector<_Type>& memory) override {
        OMP::Lock lock;
#       pragma omp parallel shared(memory, lock, r)
        {
            lock.set();
            std::default_random_engine engine_(r());
            lock.unset();

#           pragma omp for
            for(size_t i = 0; i < memory.size(); i++) {
                // std::uniform_int_distribution<int> uniform_dist(1, 100);
                // memory[i] = uniform_dist(engine);
                memory[i] = static_cast<_Type>(engine_());
            }
        }
    }

    std::string str() const {
        return "random";
    }

private:
    std::random_device r;
    std::default_random_engine engine;
    std::uniform_int_distribution<_Type> uniform_dist;
};


