#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <memory>
#include <regex>
#include <cmath>
#include <iterator>
#include <iostream>
#include <criu/criu.h>
#include <fcntl.h>
#include <wait.h>
#include <unistd.h>
#include <memory>
#include <libscratch.hh>



int main(int argc, char *argv[]) {
    using namespace std;

    func_t pred = select_predicate(1);
    cout<<"pred(3): "<<pred(3)<<endl;
    cout<<"pred: "<<pred<<endl;
    cout<<"pred.name: "<<typeid(pred).name()<<endl;
    pred = select_predicate(2);
    cout<<"pred(3): "<<pred(3)<<endl;
    cout<<"pred: "<<pred<<endl;
    cout<<"pred.name: "<<typeid(pred).name()<<endl;


}
