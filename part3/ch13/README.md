## 13章 
### Question
在第12章中，我们用Array类模拟了数组。而在Array类的实现中存在一个问题(数组也存在该问题)。先看例子：
```cpp
//访问无效指针
int *p;
{
	Array<int> a(10);
    p = &a[9];
}
cout << *p; // ERROR! p指针已经是无效的指针

Array<int> a(10);
p = &a[9];
a.resize(20);
cout << *p; // ERROR! p指针已经是无效的指针
```
所以我们本章的目标是设计一种解决方案，来解决上述问题。
### Sulotion

#### 第一个版本
我们的第一个版本先设计一个指针类(Pointer)，让它的行为与原始指针类似。
* 支持空指针
* 支持指针间的拷贝与赋值
* 支持指针绑定到数组的某个元素
* 支持*和->操作

```cpp
template<typename T>
class Pointer{
public:
    // 支持空指针
    Pointer():data(0),index(0){}
    // 允许指针指向数组的某个元素
    Pointer(Array<T> &a, unsigned i=0):data(&a),index(i){}
    
    //指针间的拷贝与赋值, 让编译器代为实现^^
    
    //允许针对指针进行解引用，及间接操作
    T& operator*() const{
        if (!data)
            throw "* of unbound Pointer.";
        else 
            return (*data)[index];
    }
    T* operator->(){
        if (!data)
            throw "-> of unbound Pointer.";
        else 
            return &((*data)[index]);
    }
private:
    // 指向数组的某个元素
    Array<T> *data;
    unsigned index;
};

```
所以，我们可以这么使用Pointer:

```cpp
// 测试类
class Test{
public:
    void greeting(void){std::cout<< "hello world."<<std::endl;}
    int value;
};

int main()
{
    std::cout << "Testing first version of Pointer."<<std::endl;
    // 测试数据准备
    Array<Test> ai(10);
    int i;
    for(i=0; i!=10; i++)
        ai[i].value = i+1;
        
    // 空指针
    Pointer<Test> p;
    
    // 绑定数组元素，及指针赋值
    p = Pointer<Test>(ai, 5);

    // 解指针及间接引用
    assert((*p).value==ai[5].value);
    (*p).value=7;
    assert(ai[5].value==7);
    p->greeting();
    
    std::cout<<" --- OK."<<std::endl;
    return 0;
} 
```
[实例代码](https://github.com/cjdao/RuminationsOnCpp/blob/master/part3/ch13/ch13_v1.cpp)

#### 第二个版本
接下来的这个版本我们来解决Question中提到的指针无效的问题。
我们的解决方案是，Array不在代表数组本身，而是代表**一种使用数组的方式**。而指针类也不再是指向Array而是指向其所表示的那个数组。
也是就说，我们可以用ArrayData类来表示原来Array表示的概念，而Array类和Pointer类变成了使用数组的两种范式--数组方式和指针方式。配合引用计数的技巧，就可以使在Array被删除是Pointer还是有效的。
因此我们将Array的实现更名为ArrayData，并加上引用计数
```cpp
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
```
然后实现Array类
```cpp
// 数组接口类
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
```

然后是Pointer类
```cpp
// 指针类
template<typename T>
class Pointer{
public:
    Pointer():pa(0),index(0){}
    Pointer(/*const 不能用const修饰, 后面有取地址的操作*/Array<T>& a, unsigned i=0):pa(a.pa),index(i){pa->used++;}
    ~Pointer(){if(pa&&--pa->used==0)delete pa;}
    Pointer(const Pointer &p):pa(p.pa?p.pa:0), index(p.index) {
            if(pa)pa->used++;}
    Pointer &operator=(const Pointer &p){
        if (p.pa) p.pa->used++;
        if (pa && --pa->used==0) delete pa;
        pa = p.pa;
        index = p.index;
        return *this;
    }
    
    T* operator->() const {
    	if (0==pa)throw "-> of unbound Pointer";
        else return &((*pa)[index]);
    }
    T& operator*() const {
    	if (0==pa)throw "* of unbound Pointer";
        else return ((*pa)[index]);
    }
private:
    ArrayData<T> *pa;
    unsigned index;
};
```
[示例代码]()
