# Template C++ - 泛型编程与模板元编程

> 基本的、好用的、有意义的概念更多时候是（在使用过程中）被发现而非（在设计时）发明出来的。谨慎地尝试使用泛型编程与模板元编程，

object-oriented 和 templates 目的都是为了支持数据和操作的解耦，都支持接口(interfaces)和多态（polymorphism）。只不过两者的维度不太一样。
- object-oriented 的接口是显式的，以函数签名为中心。多态则是通过 virtual 函数发生于运行期。是服务于一种类型设计上的抽象，通过设计基类和派生类两者的语义相似性，使得数据类型设计上具备相似的可抽象的接口。
- template 的接口是隐式的，以有效表达式为中心。多态则是通过 template 实例化和函数重载解析发生于编译期。是服务于一种算法设计上的抽象，通过将算法操作本身与具体类型的相关性解耦，使得算法可以和脱离数据类型别抽象出来。

- 模板特化与偏特化
- SFINAE
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

## 奇技淫巧

1. 当不知道模板类的具体类型时
```c++
// 声明一个模板类QWE，但不定义它
template <class T>
class QWE;

...
// 当使用QWE<T>时，T需要是一个具体的类型，否则会编译错误，这时报错会告诉你T的类型。
QWE<T> a;
```

2. 可以利用析构函数来清理不含析构函数的对象
类似与 golang 中的 defer
```C++
template <class F>
struct Final_action {
    explicit Final_action(F f) :act(f) {}
    ~Final_action() { act();}
    F act;
}

template <class F>
[[nodiscard]] auto finally(F f)
{
    return Final_action{f};
}

void use_file(const char* name)
{
    FILE* f = fopen(name, "r");                 // C API 资源，没有析构函数
    if (!f) return;
    auto closer = finally([f]{ fclose(f); });   // 在声明处登记清理动作

    // ... 随便使用 f ...

    if (error_occurred) return;                 // ① 提前 return：fclose 自动执行
    // 若中途抛异常：栈回溯时 closer 析构，fclose 照样执行
}                                               // ② 正常离开：fclose 自动执行
```

## Concepts（C++20）

是对泛型编程和模板元编程中类型的一种约束。
todo