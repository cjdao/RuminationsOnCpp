## 13章 访问容器中的元素
### Question
-------------
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
-------------
#### 第一个版本
-------------
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
-------------
接下来的这个版本我们来解决Question中提到的指针无效的问题。
我们的解决方案是，Array不再代表数组本身，而是代表**一种使用数组的方式**。而指针类也不再是指向Array而是指向其所表示的那个数组。
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
    Pointer(Array<T>& a, unsigned i=0):pa(a.pa),index(i){pa->used++;}
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
在上面的代码中，我们仅解决了question中提到的第一个问题。如果Array类存在resize方法（即Array支持动态扩容），那么question中提到的第二个问题，按照我们现在的解决放你，p和a都是有效的，但它们指向不同的东西了！这基本上不会是我们想要的。但是针对这个问题，本书并没有给出解决方案，所以我们先搁下，后面再讨论。

[示例代码](https://github.com/cjdao/RuminationsOnCpp/blob/master/part3/ch13/ch13_v2.cpp)

#### 第三个版本
-------------
如果我们的Array是const的，我们的Pointer类将无法很好的工作。
我们先来看一个错误
```cpp
const Array<int> a(10);
Pointer<int> p(a, 1); // ERROR, 我们的Pointer类没有定义接受const Array的构造函数
```
所以我们先试图使Pointer接受const Array的构造
```cpp
template<typename T>
class Pointer{
public:
//...
    // 新加了接受const Array的构造函数，它与接受Array的构造函数是重载关系
    Pointer(const Array<T>& a, unsigned i=0):pa(a.pa),index(i){pa->used++;}
/....
};
```
这样我们就可以接受const Array了，但是还有问题，看下面：
```cpp
const Array<int> a(10);
std::cout << a[1] << std::endl;
// a[1] = 1;// ERROR
Pointer<int> p(a, 1); 
*p = 1;  // 天啊，我们竟然可以修改一个const容器的元素
std::cout << a[1] << std::endl; 
```
问题出在operator->和operator*的返回值上是非const的上面，这样我们就面临了返回值既要非const又要const的问题了！我们或许可以让operator->和operator*的const的版本返回值为const而非const版本返回值为非const，像下面这样的代码段所示：
```cpp
template<typename T>
class Pointer{
public:
//...
    // 给Pointer类调用，返回非const
    T* operator->()  {
    	if (0==pa)throw "-> of unbound Pointer";
        else return &((*pa)[index]);
    }
    T& operator*()  {
    	if (0==pa)throw "* of unbound Pointer";
        else return ((*pa)[index]);
    }
    
    //给const Pointer类调用，返回const
     const T* operator->()  const {
    	if (0==pa)throw "-> of unbound Pointer";
        else return &((*pa)[index]);
    }
    const T& operator*() const {
    	if (0==pa)throw "* of unbound Pointer";
        else return ((*pa)[index]);
    }
/....
};
```
然后我们可以这样，使用Pointer类
```cpp
const Array<int> a(10);
std::cout << a[1] << std::endl;
// a[1] = 1;// ERROR
const Pointer<int> cp(a, 1);  // Pointer 为const
*cp = 1;  // ERROR

Pointer<int> p(a, 1);
*p = 1;  // 天啊，我们仍然可以修改一个const容器的元素！！！
std::cout << a[1] << std::endl; 
```
这里明显有几个问题：
第一，const Pointer的语义应该是Pointer本身是不可变的，而不是Pointer所指向的内容不可变；
第二，这样的设计，很容易造成他人的误用！
所以，基于以上的讨论，我们得出的结论是，我们必须另外实现一个新的类(Ptr_to_const)，它类似于Pointer，但它指向const Array而不是Array。而且为了模拟内建指针的行为，我们支持Pointer类可以转换到这个新类。
我们可以利用继承的概念，来实现新类和Pointer的关系。我们让Ptr_to_const作为父类，而Pointer作为子类。
```cpp
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
```
[示例代码](https://github.com/cjdao/RuminationsOnCpp/blob/master/part3/ch13/ch13_v3.cpp)

#### 第四个版本
-------------



