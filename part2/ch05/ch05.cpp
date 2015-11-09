#include <iostream>
#include <vector>
using namespace std;

// 继承树
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


// 代理类
class Surrogate{
public:
        Surrogate():p(0) {}   
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
        Super *p;            
};

int main()
{
	vector<Surrogate> v;
	v.push_back(Super());
	v.push_back(Sub1());
	v.push_back(Sub2());

	v[0].f();
	v[0]->f();
	(*v[0]).f();
	v[0].get()->f();

	v[1].f();
	v[1]->f();
	(*v[1]).f();
	v[1].get()->f();

	v[2].f();
	v[2]->f();
	(*v[2]).f();
	v[2].get()->f();
	cout << endl;	

	vector<Surrogate> v1=v;
	v.erase(v.begin(), v.end());
	v1[0].f();
	v1[1].f();
	v1[2].f();
	cout << endl;	
	
	vector<Surrogate> v2;
	v2 = v1;
	v1.erase(v1.begin(), v1.end());
	v2[0].f();
	v2[1].f();
	v2[2].f();
	cout << endl;	

	return 0;
}
