## 代理类
### Question:
设计一种解决方案，使C++容器能包含类型不同但彼此相关(如继承关系)的对象.

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
像上面声明一个vector肯定是不行的，因为插入vector的子类(如Sub1)都会被**切割**成父类Super，下面的代码将无法正常运行：
```cpp
v.push_back(Sub1());
v.back().f(); //ERROR, output "f() in super ."
```

* 第二种方案：
```cpp
vector<Super &> v; //ERROR, 木有引用类型的容器啊
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

* 第四种方案

我们在方案三的基础上，添加一个**中间层**，用这个中间类(就叫Surrogate吧)来隐藏指针,并负责动态内存的管理。

**使用类来表示概念**(在这里就是使用类来表示Super * 这个概念);

#### 我们来看看该怎么设计这个中间类!

从我们会怎样使用这个中间类开始：
首先它必须能被作为vector的元素类型，即:
```cpp
vector<Surrogate> v;
```
所以，Surrogate 必须有**默认构造函数**。

然后，我们如何让一个Surrogate能够指向Sub继承树上的任何一类对象呢?
让Surrogate有一个类型为‘Super * ’的数据成员，并通过Sub继承树上的任一类对象的拷贝构造Surrogate。
```cpp
Surrogate s0(Super()); // 不能是这样Surrogate s0(new Super());
Surrogate s1(Sub1());
Surrogate s2(Sub2());
```

所以，Surrogate的初步轮廓应该是这样的：
```cpp
class Surrogate{
public:
	Surrogate():p(0) {}   // Surrogate必须有默认构造函数!
	Surrogate(const Super&s):p(new /*我们在这里要new什么啊?*/) {}
private:
	Super *p;             // 不能是Super p 或者 Super &p
};
```
在上面的Surrogate定义中，我们在其拷贝构造函数里遇到了问题，更具定义我们无法知道运行时的参数s到底是Sub继承树上的哪一个类,
所以我们就无法new出我们期望的实体！

解决这个问题的技巧在于，让Super定义一个叫clone的virtual函数，由各子类继承实现。因此我们的继承树就变成了这样：
```cpp
class Super{
public:
    virtual void f() {cout << "f() in super ."<< endl;}
    virtual Super* clone()const {return new Super();}
};
class Sub1:public Super{
public:
    virtual void f() {cout << "f() in sub1 ."<< endl;}
    virtual Super* clone()const {return new Sub1();}
};
class Sub2:public Super{
public:
    virtual void f() {cout << "f() in sub2 ."<< endl;}
    virtual Super* clone()const {return new Sub2();}
};
```
于是，我们Surrogate类就这样这么实现：
```cpp
class Surrogate{
public:
        Surrogate():p(0) {}   // Surrogate必须有默认构造函数!
        Surrogate(const Super&s):p(s.clone()) {}
private:
        Super *p;             // 不能是Super p 或者 Super &p
};
```

#### 我们怎么通过Surrogate访问Super中的方法？
* 方案一：直接在Surrogate中定义Super相应的方法
```cpp
class Surrogate{
public:
        /*....*/	
	void f() {return p->f();} // 
        /*....*/	
};

```

* 方案二：重载Surrogate中的操作符
```cpp
class Surrogate{
public:
        /*....*/	
	Super * operator->(){return p;} // C++ 重载 ‘->’ 操作符！
	Super & operator*(){return *p;} // C++ 重载 ‘*’ 操作符！
	// C++ 无法重载 ‘.’ 操作符！
        /*....*/	
};
// 于是我们可以这样调用f方法：
Surrogate s(Super());
s->f();
(*s).f();
```
* 方案三：在Surrogate中定义get方法
```cpp
class Surrogate{
public:
	//......
	Super *get(){return p;}
	//Super &get(){return *p;}
	//......
};
// 于是我们可以这样调用f方法：
Surrogate s(Super());
s.get()->f();
// s.get().f();
```

* 方案四：不知道无没有

#### 最后不要忘了
Surrogate的拷贝构造函数，赋值操作符，及虚构函数！！！
完整的Surrogate：
```cpp
class Surrogate{
public:
        Surrogate():p(0) {}   // Surrogate必须有默认构造函数!
        Surrogate(const Super&s):p(s.clone()) {}
        Surrogate(const Surrogate&s):p(s.p?s.p->clone():0) {}
        Surrogate &operator=(const Surrogate &s){
                if (this!=&s) {
                        delete p;
                        p = s.p->clone();
                }
                return *this;
        }
        ~Surrogate(){delete p;}


        void f() {return p->f();} //

        Super * operator->(){return p;} // C++ 重载 ‘->’ 操作符！
        Super & operator*(){return *p;} // C++ 重载 ‘*’ 操作符！

        Super *get(){return p;}
        //Super &get(){return *p;}

private:
        Super *p;             // 不能是Super p 或者 Super &p
};

```


### Code:

