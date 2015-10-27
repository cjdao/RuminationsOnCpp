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

### 