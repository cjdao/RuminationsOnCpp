#include <iostream>
using namespace std;

template <typename T>
class Array{
public:
	Array():data(0),size(0){}
	Array(unsigned n):data(new T[n]), size(n){}
	~Array(){delete [] data;}	
	T& operator[](unsigned i) {return data[i];};
	const T& operator[](unsigned i)const {return data[i];};
	operator T*() {return data;}//数组可以转换成指向元素类型的指针	
	operator const T*() const{return data;}//数组可以转换成指向元素类型的指针	
private:
	Array(const Array&){}
	Array& operator=(const Array&){}
	T *data;
	unsigned size;
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

	cout << endl;

	return 0;
}
