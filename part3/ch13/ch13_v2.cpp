#include <iostream>

using namespace std;

template<typename T>
class Pointer;
template <typename T>
class Array;

// 数组实现类
template <typename T>
class ArrayData{
    friend class Pointer<T>;
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

// 指针类
template<typename T>
class Pointer{
public:
    Pointer():pa(0),index(0){}
    Pointer(Array<T>& a, unsigned i=0):pa(a.pa),index(i){pa->used++;}
    ~Pointer(){if(pa&&--pa->used==0)delete pa;}
    Pointer(const Pointer &p):pa(p.pa?p.pa:0), index(p.index){if(pa)pa->used++;}
    Pointer &operator=(const Pointer &p){
        if (p.pa) p.pa->used++;
        if (pa && --pa->used==0) delete pa;
        pa = p.pa;
        index = p.index;
        return *this;
    }
    
    T* operator->() const{if (0==pa)throw "-> of unbound Pointer";else return &((*pa)[index]);}
    T& operator*() const{if (0==pa)throw "* of unbound Pointer";else return ((*pa)[index]);}
private:
    ArrayData<T> *pa;
    unsigned index;
};

// 为测试而定义的类
class UserdefineClass {
public:
    void greeting() {cout << "hello world."<<endl;}
};

int main()
{
    Array<int> *ai = new Array<int>(100);
    int i;
    for (i=0; i!=100; i++)  
        (*ai)[i] = i;

    Pointer<int> *pp = NULL; 
    
    Pointer<int> p;
    {
        // 数组构造
        Pointer<int> p1(*ai, 10);
        // 拷贝构造
        pp = new Pointer<int>(p1);
        // 赋值操作符
        p = p1;
        // 解指针
        cout << *p1 << endl;
        delete ai;
        cout << *p1 << endl;
        cout << *p << endl;
    }
    cout << *p << endl; 
    *p = 199;
    cout << *p << endl; 
    cout << *(*pp) << endl;
    
    cout << endl;
    Array<UserdefineClass> au(10) ;
    Pointer<UserdefineClass> pua(au, 0);
    // 间接引用
    pua->greeting();

    return 0;
}
