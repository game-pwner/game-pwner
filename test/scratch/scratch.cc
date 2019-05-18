#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <memory>
#include <regex>
#include <cmath>
#include <iterator>
#include <pwner/common.hh>
// #include <pwner/process/Process.hh>

#include <iostream>

struct foo {
    // static bool bar(int){return 1;}
    bool bar()const{return 2;}
};

int main() {
    // auto a1 = static_cast<bool (*)(int)>(&foo::bar);
    auto a2 = static_cast<bool (foo::*)() const>(&foo::bar);
    std::cout<<" "<<a2;
}

// using namespace std;
//
//
// int main(int argc, char *const argv[], char *envp[]) {
//     using namespace std;
//     oom_score_adj(999);
//
//     return 0;
// }
//