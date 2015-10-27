#include <iostream>
using namespace std;

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
    
    // 支持指针操作(考虑const的情况)
    operator const T*()const{ return data;}
    operator T*(){return data;}
private:
	Array(const Array&);
    Array &operator=(const Array &);

	unsigned size;
    T *data;
};

int main()
{
	// 创建一个空数组
	Array<int> ai;
	// 创建一个指定大小的数组
	Array<int> ai1(100);
	// 创建一个包含数组的数组
	Array<Array<int> > ai3;
	
	//数组的存取值操作
	int i = 0;
	for (i=0; i!=100; i++) {
		ai1[i] = i;
	}

	for (i=0; i!=100; i++) {
		cout << ai1[i] << " " ;
	}
	cout << endl << endl;

	
	// 数组与指针
	int *ip = ai1;
	for (i=0; i!=100; i++) {
		*ip++ = 100-1-i;
	}

	ip = ai1;
	for (i=0; i!=100; i++) {
		cout << *ip++ << " " ;
	}
	cout << endl << endl;

	// const 数组
	const Array<int> cai(100);
	for (i=0; i!=100; i++) {
		cout <<cai[i] << " "; 
	// 我们可以读取数组的值，但无法给他赋值
	//	cai[i] = i; // ERROR
	}
	cout << endl;
	

	return 0;
}
