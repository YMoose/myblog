# C++ 模板（泛型编程与模板元编程）

- 函数模板与类模板
- 模板特化与偏特化
- SFINAE
- Concepts（C++20）
- 变参模板
- constexpr / consteval
- CRTP

```c++
template<typename T>
class Vector {
private:
    T* elem;                            // elem 指向含有 sz 个 T 元素的数组
    int sz;
public:
    explicit Vector(int s);
    ~Vector() { delete[] elem; }

    T& operator[](int i);               // 对于非常量的 Vector
    const T& operator[](int i) const;   // 对于常量的 Vector
    int size() const { return sz; }
};
```

> Guideline：如果成员函数不会改变对象的状态，把成员函数声明为const

类型或函数前缀 template<typename T> 指明 T 是该申明的形参，语义类似于数学证明中的“对于所有类型 T ”。

模板是一种编译时机制，在编译过程中，遇到有具体的类型作为模板参数的模板时，就会触发**模板实例化**，针对这个具体类型生成一套代码。

在大多数情况下模板仅对满足特定条件的模板参数有意义，所以要对类型 T 定义约束，只有类型 T 满足这种约束时，才可以真正作为模板参数实例化，此类模板参数称为**受限模板参数**，这种约束语义上类似于数学证明中的“对于所有满足某某条件的类型 T ”。这种机制在 C++20 中通过**概念**实现。

> 模板函数可以作为类的成员函数，但不能是其 virtual 函数。编译器不可能知道模板的所有实例，所以不可能为模板函数生成 vtbl。vtable 是在编译类定义的地方生成的，那时编译器看不到别的翻译单元中模板函数是否被实例化，所以没法知道要有多少个虚函数，没法生成vtable。

模板提供了一种将算法在编写时不与具体的类型绑定，到编译时才绑定的能力，即**泛型编程**。通过此能力

> 所谓谓词就是一个可以调用并返回true或false的（函数）对象

## 给容器类型内部元素指定具体类型
## 参数化标准库中的类型与算法