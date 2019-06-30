#pragma once

#include <pwner/common.hh>
#include <pwner/scanner/Value.hh>

NAMESPACE_BEGIN(pwner)

class match {
public:
    uintptr_t address;
    a64 value;
    u16 type;

    explicit match()
            : address{0}, value{0}, type{0} {}

    explicit match(uintptr_t address, a64 value, u16 type)
            : address{address}, value{value}, type{type} {}
};

NAMESPACE_END(pwner)
