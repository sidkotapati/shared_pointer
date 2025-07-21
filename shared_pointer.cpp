#include "shared_pointer.h"


int main(){
    SharedPointer<int> ptr = SharedPointer<int>::make_shared(32);

    {
    SharedPointer<int> a(ptr);
    SharedPointer<int> b(ptr);
    SharedPointer<int> c(ptr);
    *c = 1;
    }
    std::cout << *ptr << "\n";
    
}

/**
 * TODO:
 *  - add docstrings
 *  - create unit tests
 *  - create threaded tests
 */