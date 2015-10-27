## 12章　设计容器类
容器是一种保存值的集合的数据结构。C有两种内建的容器：数组和结构体。C\++没有提供更多的容器(C++语言本身没有提供任何容器，但STL提供了不少容器类)。设计一个成功的容器不可能是自然而然的事，它需要设计者的很多权衡与选择。

### 包含什么
对于放入容器中的对象，容器是应该包含该对象的副本还是指向该对象的指针?总的来说，我们建议容器应该包含对象的副本。

### 容器间的复制意味着什么
一般而言，复制容器应该是，复制容器中的元素的副本，而不是让两个容器指向同一个底层实现。

### 怎样获取容器的元素
我们从容器Container<T> 中获取对象时，是应该得到类型T还是类型T&的对象?  
如果十分看重效率，或者需要能够修改容器中的对象，那么可以使用引用类型。  
但是，返回引用类型，可能会导致用户在使用容器的过程中，保存获得的引用地址，这就可能导致后续使用出现错误。

### 怎样区分读和写
有这样一种设计方式：提供一个update(i,x)的方法，用于改变容器中下标为i的元素的内容，而使operator[]操作符仅返回对象的副本。
但是这种方式，无法处理容器的元素类型是容器的这种情况。
**所以我们的设计选择一般是,让operator[]返回T&  **

### 怎样处理容器的增长
* 区分向容器添加一个新的元素的操作和改变一个已有元素的操作
* 设计有逐步扩展能力的容器时，如何为容器中的元素分配内存

### 容器支持哪些操作
* 弄清楚想利用容器做什么很重要

### 怎样设想容器元素的类型
容器要设计的越通用，对其所要存储的元素类型的限制就要越少。

### 容器和继承
不同元素类型的容器间不应该存在继承关系！(其讨论过程不是看得很懂)

---------------------------------------------------------------

### 设计一个类似与数组的容器类
* 它是一个template,因为我们必须能够创建不同元素类型的容器
* 它与数组一样贝构造函数和赋值操作符必须为private的
* 它与数组一样，不支持动态扩容，即长度在创建时就决定了
* 它与数组一样，支持下标和指针操作

```cpp	
template <typename T>
class Array{
public:
	//unsigned n=0 使得Array有默认构造函数，因此可以创建类型为Array的Array,  
    //然而这并没有什么卵用，因为Array不支持动态扩容，所以我们无法指定作为元素的Array的大小
    Array(unsigned n=0):size(n),data(new T[size]){}
    ~Array(){delete[] data;}
    
    // 支持下标操作(考虑const的情况)
    const T& operator[](unsigned i) const{
    	if (i>=size) 
        	throw "";
        return data[i];
    }
    // effective C++里条款３：尽可能使用const 里提到的技巧
    T& operator[](unsigned i){
		return const_cast<T &>(static_cast<const Array &>(*this)[i]);
    }
    
    // 支持指针操作(考虑const的情况)
    operator T*(){return data;}
    operator const T*()const{ return data;}
private:
	Array(const Array&);
    Array &operator=(const Array &);

	unsigned size;
    T *data;
};
```
[示例代码](https://github.com/cjdao/RuminationsOnCpp/blob/master/part3/ch12/ch12.cpp)

