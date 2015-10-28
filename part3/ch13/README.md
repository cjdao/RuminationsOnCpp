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
所以我们本章的目标是设计一种指针类(Pointer)，来解决上述问题。

### 第一个版本
我们的第一个版本先设计一个指针类，让它的行为与原始指针类似。
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

### 第二个版本

