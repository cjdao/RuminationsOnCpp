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

    ArrayData(unsigned n=0):sz(n),data(new T[sz]),used(1){}
    ~ArrayData(){delete[] data;}

    ArrayData(const ArrayData& a):sz(a.sz),data(new T[sz], used(1)) {
       copy(a.data,sz);
    }

    void clone(const ArrayData &a , unsigned s) {

        if (s!=sz) {
            T *ndata=new T[s];
            sz = s;
            delete []data;
            data=ndata;
        }
        copy(a.data, sz);
    }

    ArrayData &operator=(const ArrayData &);

    // 支持下标操作(考虑const的情况)
    const T& operator[](unsigned i) const{
        if (i>=sz || 0==data) 
            throw "ArrayData subscript out of range.";
        return data[i];
    }
    // 'effective C++ 条款3:尽可能使用const' 里提到的技巧
    T& operator[](unsigned i){
        return const_cast<T &>(static_cast<const ArrayData &>(*this)[i]);
    }
    
    void copy(T *d, unsigned s) {
        for (int i=0; i!=s; i++) {
            data[i] = d[i];
        }
    }   

    unsigned min(unsigned a, unsigned b) {
        return a<b?a:b; 
    }

    void resize(unsigned news) {
        if (news==sz) return;

        // 考虑异常安全性
        T *nd = new T[news]; 
        T *tmp = data;
        data = nd;
        copy(tmp, min(sz,news));
        sz = news;
        delete [] tmp;
    }

    void reserve(unsigned s) {
        if (s<sz) return ;

        unsigned news = sz;
        if (news==0) news = 1;

        while (news<=s) { // 这里不是news<s
            news *= 2;
        }
        resize(news);
    }
    
    unsigned size() const {
        return sz;
    }

    unsigned sz;
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

    Array(const Array& a):pa(new ArrayData<T>(*(a.pa))) {}

    Array &operator=(const Array &a) {
        if (this != &a)
            pa->clone(*(a.pa), a.size());
        return *this;
    }
    
    // 支持下标操作(考虑const的情况)
    const T& operator[](unsigned i) const{
        return (*pa)[i];
    }
    // 'effective C++ 条款3:尽可能使用const' 里提到的技巧
    T& operator[](unsigned i){
        return const_cast<T &>(static_cast<const Array &>(*this)[i]);
    }
    
    void resize(unsigned s)
    {
       pa->resize(s);
    }

    void reserve(unsigned s) 
    {
        pa->reserve(s);
    }

    unsigned size()const
    {
        return pa->size();
    }
private:
    
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


    // 完善Pointer类的++ -- != 及==操作
    Pointer& operator++() {
        index++;
        return *this;
    }

    Pointer operator++(int){
        Pointer tmp(*this);  
        ++(*this); 
        return tmp;
    }

    Pointer& operator--() {
        index--;
        return *this;
    }

    Pointer operator--(int){
        Pointer tmp(*this);  
        ++(*this); 
        return tmp;
    } 

    bool operator==(const Pointer &p) {
        return (pa==p.pa && index==p.index);
    }

    bool operator!=(const Pointer &p) {
        return !(*this == p);
    }
};


int main()
{
    Array<int> a(10);
    int i;
    for (i=0; i!=10; i++) {
        a[i] = i;
    }
    Pointer<int> pstart(a, 0);
    Pointer<int> pend(a, 10);  
    i = 0;
    for (; pstart!=pend; pstart++, i++) {
        assert(*pstart == i);
    }

    
    i=9;
    do{
        assert(*(--pend)==i--);
    } while (pend==pstart);

    Array<int> b(10);
    Pointer<int> p1(a, 0);
    Pointer<int> p2(a, 0);
    Pointer<int> p3(a, 2);
    Pointer<int> p4(b, 0);
    assert(p1==p2 && p1!=p3 && p1!=p4);

    std::cout << " --- OK." << std::endl;
    return 0;
}
