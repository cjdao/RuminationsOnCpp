#include <iostream>
#include <cassert>

using namespace std;

template<typename T>
class Ptr_to_const;
template<typename T>
class Pointer;
template <typename T>
class Array;

// 数组实现类
template <typename T>
class ArrayData{
    friend class Pointer<T>;
    friend class Ptr_to_const<T>;
    friend class Array<T>;

    ArrayData(unsigned n=0):size(n),data(new T[size]),used(1){}
    ~ArrayData(){delete[] data;}

    // 声明而不定义，禁止拷贝和赋值操作
    ArrayData(const ArrayData&);
    ArrayData &operator=(const ArrayData &);

    // 支持下标操作(考虑const的情况)
    const T& operator[](unsigned i) const{
        if (i>=size || 0==data) 
            throw "ArrayData subscript out of range.";
        return data[i];
    }
    // 'effective C++ 条款3:尽可能使用const' 里提到的技巧
    T& operator[](unsigned i){
        return const_cast<T &>(static_cast<const ArrayData &>(*this)[i]);
    }
    
    //有了Pointer 就不需要支持指针操作
    // operator const T*()const{ return data;}
    // operator T*(){return data;}

    unsigned size;
    T *data;

    int used;
};

// 数组封装类
template <typename T>
class Array{
public:
    friend class Pointer<T>;
    friend class Ptr_to_const<T>;

    Array(unsigned n=0):pa(new ArrayData<T>(n)){}
    ~Array(){if(--pa->used==0)delete pa;}
    
    // 支持下标操作(考虑const的情况)
    const T& operator[](unsigned i) const{
        return (*pa)[i];
    }
    // 'effective C++ 条款3:尽可能使用const' 里提到的技巧
    T& operator[](unsigned i){
        return const_cast<T &>(static_cast<const Array &>(*this)[i]);
    }
    
    //有了Pointer就 不需要支持指针操作(考虑const的情况)
    //operator const T*()const{ return *(*pa);}
    //operator T*(){return *(*pa);}
private:
    Array(const Array&);
    Array &operator=(const Array &);
    
    ArrayData<T> *pa;
};

// 指向const Array的指针类
template<typename T>
class Ptr_to_const{
public:
    Ptr_to_const():pa(0),index(0){}
    Ptr_to_const(const Array<T>& a, unsigned i=0):pa(a.pa),index(i){pa->used++;}
    ~Ptr_to_const(){if(pa&&--pa->used==0)delete pa;}
    Ptr_to_const(const Ptr_to_const &p):pa(p.pa?p.pa:0), index(p.index) {
            if(pa)pa->used++;}
    Ptr_to_const &operator=(const Ptr_to_const &p){
        if (p.pa) p.pa->used++;
        if (pa && --pa->used==0) delete pa;
        pa = p.pa;
        index = p.index;
        return *this;
    }
    
    // 返回值均为const 类型
    const T* operator->() const {
        if (0==pa)throw "-> of unbound Pointer";
        else return &((*pa)[index]);
    }
    const T& operator*() const {
        if (0==pa)throw "* of unbound Pointer";
        else return ((*pa)[index]);
    }

//这里必须是protected的了
protected:
    ArrayData<T> *pa;
    unsigned index;
};
// 指向 Array的指针类
template<typename T>
class Pointer:public Ptr_to_const<T>{
public:
    Pointer(Array<T> &a, unsigned i=0):Ptr_to_const<T>(a,i) {}
    Pointer() {}
    // 注意，这里使用了基于作用域的名称掩盖规则，返回值均为非const 类型
    // 在'effective c++的条款36中说：绝不重新定义继承而来的non-virtual函数， 但我们却这么做了'
    // 这里为什么要用using，参考'effective c++的条款43:学习处理模板化基类内的名称'
    using Ptr_to_const<T>::pa;
    using Ptr_to_const<T>::index;
    T* operator->() const {
        if (0==pa)throw "-> of unbound Pointer";
        else return &((*pa)[index]);
    }
    T& operator*() const {
        if (0==pa)throw "* of unbound Pointer";
        else return ((*pa)[index]);
    }
};


int main()
{
    const Array<int> a(10);
    std::cout << a[1] << std::endl;
//    a[1] = 1;// ERROR
//    Pointer<int> p(a, 1);  // ERROR
    Ptr_to_const<int> pc(a, 1);  
//    *pc = 1;  // ERROR 
    assert(*pc==0);
    std::cout << *pc << std::endl; 

    Array<int> a1(10);
    Pointer<int> p(a1,1);
    assert(*p==0);
    cout << *p << endl;
    *p = 1;
    assert(a1[1]==1);
    cout << a1[1] << endl; 
    
    pc=p;
    
    assert(*pc==1);
    cout << *pc << endl;
    
//    *(static_cast<Ptr_to_const>(p)) = 3;// ERROR
    return 0;
}
