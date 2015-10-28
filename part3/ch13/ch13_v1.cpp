#include <iostream>
#include <cassert> // for assert

// 数组类
template <typename T>
class Array{
public:
	//unsigned n=0 使得Array有默认构造函数，因此可以创建类型为Array的Array,  
    //然而这并没有什么卵用，因为Array不支持动态扩容，所以我们无法指定作为元素的Array的大小
    Array(unsigned n=0):size(n),data(new T[size]){}
    ~Array(){delete[] data;}
    
    // 支持下标操作(考虑const的情况)
    const T& operator[](unsigned i) const{
    	if (i>=size || 0==data) 
        	throw "Array subscript out of range.";
        return data[i];
    }
    // effective C++里条款３：尽可能使用const 里提到的技巧
    T& operator[](unsigned i){
		return const_cast<T &>(static_cast<const Array &>(*this)[i]);
    }
    
    // 不再支持原始指针操作
    //operator const T*()const{ return data;}
    //operator T*(){return data;}
private:
	Array(const Array&);
    Array &operator=(const Array &);

	unsigned size;
    T *data;
};

// 指针类
template<typename T>
class Pointer{
public:
    // 支持空指针
    Pointer():data(0),index(0){}
    // 我们首先允许指针指向数组的某个元素
    Pointer(Array<T> &a, unsigned i=0):data(&a),index(i){}
    
    //指针间的拷贝与赋值, 让编译器代为实现^^
    
    //我们允许针对指针进行解引用，及间接操作
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
