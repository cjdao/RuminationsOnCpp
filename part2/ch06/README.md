## 第六章 句柄：第一部分

### Questions:
由代理类引出来的问题，在代理类的实现中，代理类间的拷贝或者赋值，都会造成其所代理对象的拷贝或赋值。而在实际情况中，有些被代理的对象，或者出于性能上的考虑，或者出于逻辑上的考虑，而不支持拷贝和赋值操作。
因此我们需要设计一种方案，实现类似代理类的功能，但代理类共享被代理对象，即多个代理类可以代理一个被代理对象。这种类就叫句柄类吧(handle)。

### Sulotion:
我们开始吧！

首先，既然功能类似于代理类，那就先把代理类的定义弄过来，然后改个名字
```cpp
class Handle{
public:
        Handle():p(0) {}   
        Handle(const Super&s):p(s.clone()) {}
        Handle(const Handle&h):p(h.p?h.p->clone():0) {}
        Handle &operator=(const Handle &h){
                if (this!=&h) {
                        delete p;
                        p = h.p->clone();
                }
                return *this;
        }
        ~Handle(){delete p;}
private:
        Super *p;             
};

```


### Code:
