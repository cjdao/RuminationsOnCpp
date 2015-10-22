## 代理类
### Question:
设计一个C++容器，使它能包含类型不同但彼此相关(如继承关系)的对象.

### Solution:
假设我们有这么些有继承关系的类：
```cpp
class Super{
public:
    virtual void f() {cout << "f() in super ."<< endl;}
};
class Sub1:public Super{
public:
    virtual void f() {cout << "f() in sub1 ."<< endl;}
};
class Sub2:public Super{
public:
    virtual void f() {cout << "f() in sub2 ."<< endl;}
};
```
那么我们要在一个vector容器里保存所有这些有继承关系的对象，我们该如何做呢?

* 第一种方案:
```cpp
vector<Super> v;
```
像上面声明一个vector肯定是不行的，因为插入vector的子类如RoudVehicle都会被**切割**成父类Vehicle，下面的代码将无法正常运行：
```cpp
v.push_back(Sub1());
v.back().f(); //ERROR, output "f() in super ."
```

* 第二种方案：
```cpp
vector<Super &> v; //ERROR, 木有引用类型的容器
```

* 第三种方案：
```cpp
vector<Super *> v;
```
```cpp
v.push_back(new Sub1());
v.back().f(); //OK, output "f() in sub1 ."
// 你必须总是记得当你要删除一个vector中的元素时要像下面这样释放内存!
auto tmp = v.back();
v.pop_back();
delete tmp;
```
这种方法，虽然能够工作，但却为使用者带来了动态内容管理的负担。

**使用类来表示概念**(在这里就是使用类来表示Super *这个概念);

* 第四种解决方案
我们在方案三的基础上，添加一个中间层，用类来隐藏指针。
我们来看看该怎么设计这个中间类(假设这个类的名字Surrogate)！
首先，先想想我们会怎样使用这个中间类：
第一它必须能被作为vector的元素类型，
```cpp
class Surrogate{
public:
	Surrogate(const Super&s):p(new /*我们在这里要new什么啊?*/) {}
private:
	Super *p;
};
```

### Code:
