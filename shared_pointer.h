#include <cstdlib>
#include <iostream>
#include <mutex>

namespace {
template<typename T>
class control_block {
private:
    uint32_t counter_;
    T* obj_;
    std::mutex mtx_; // only responsible for protecting the counter not the obj itself, needs to be done by the user

public:
    uint32_t count(){ 
        std::lock_guard<std::mutex> lock(mtx_);
        return counter_; 
    }

    void increment(){ 
        std::lock_guard<std::mutex> lock(mtx_);
        counter_++; 
    }

    bool decrement(){
        std::lock_guard<std::mutex> lock(mtx_);
        counter_--; 
        if (counter_ == 0){ 
            delete obj_; 
            return true;
        }
        return false;
    }

    T* get_obj_ptr(){ return obj_; }

    template<typename... Args>
    control_block(Args&&... args) : counter_(1) {
        obj_ = new T(std::forward<Args>(args)...);
    }

    ~control_block() {}
};
}

template<typename T>
class SharedPointer {
private:
    control_block<T>* ptr;    
    // control block constructor should only be used for make_shared (creating a control block)
    SharedPointer(control_block<T>* block_ptr) : ptr(block_ptr) {}

public:
    // copy constructor
    SharedPointer(const SharedPointer &original) : ptr(original.ptr) {
        if (ptr){ ptr->increment(); } 
    }

    // assignment operator
    SharedPointer& operator=(const SharedPointer &original) {
        if (this == &original) return *this;
        if (ptr && ptr->decrement()){
            delete ptr;
        }
        ptr = original.ptr;
        if (ptr) { ptr->increment(); }
        return *this;
    }

    // destructor 
    ~SharedPointer(){
        if (ptr && ptr->decrement()){
            delete ptr;
        }
    }

    // overload arrow operator
    T* operator->(){
        if (!ptr) { std::cerr << "Pointer doesn't exist!\n"; std::exit(1); }
        return ptr->get_obj_ptr();
    }

    // overload dereference operator
    T& operator*(){
        if (!ptr) { std::cerr << "Pointer doesn't exist!\n"; std::exit(1); }
        return *(ptr->get_obj_ptr());
    }

    // for creating the control block
    template<typename... types>
    static SharedPointer make_shared(types... args){
        control_block<T>* block = new control_block<T>(std::forward<types>(args)...);
        return SharedPointer{block};
    }
};