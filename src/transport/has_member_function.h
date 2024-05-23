#include <type_traits>
#include <utility>
// #include <iostream>
// 辅助类型

template<typename T, typename Fun, Fun>
struct has_member_function;



template<typename T, typename Ret, typename... Args, Ret(T::*func)(Args...)>
class has_member_function<T, Ret(T::*)(Args...), func> {

private:
    typedef char YesType[1];
    typedef char NoType[2];

    template<typename U, U> struct really_has;

    template<typename C> static YesType& test(really_has<Ret(T::*)(Args...), func>*);
    template<typename C> static NoType& test(...);

public:
    static const bool value = sizeof(test<T>(0)) == sizeof(YesType);
};

// 特化支持 const 成员函数
template<typename T, typename Ret, typename... Args, Ret(T::*func)(Args...) const>
class has_member_function<T, Ret(T::*)(Args...) const, func> {
private:
    typedef char YesType[1];
    typedef char NoType[2];

    template<typename U, U> struct really_has;

    template<typename C> static YesType& test(really_has<Ret(T::*)(Args...) const, func>*);
    template<typename C> static NoType& test(...);

public:
    static const bool value = sizeof(test<T>(0)) == sizeof(YesType);
};


// template <typename, typename T>
// struct has_member;

// // 偏特化用于检测名为 foo 的成员函数
// template <typename C, typename Ret, typename... Args>
// struct has_member<C, Ret(Args...)> {
// private:
//     template <typename T>
//     static constexpr auto check(T*) -> typename std::is_same<
//         decltype(std::declval<T>().GetSeqNum(std::declval<Args>()...)),
//         Ret    // 返回类型
//     >::type;

//     template <typename>
//     static constexpr std::false_type check(...);

//     using type = decltype(check<C>(0));

// public:
//     static constexpr bool value = type::value;
// };


// // 用于检查类 T 是否具有特定的成员函数 foo
// template <typename T>
// constexpr bool has_foo_v = has_member<T, void()>::value;



// class ClassWithFoo {
// public:
//     void foo() {}
//     int bar(int) { return 0; }
// };

// class ClassWithoutFoo {
//     int x;
// };

// int main() {
//     std::cout << std::boolalpha;
//     // 检查ClassWithFoo是否有foo函数
//     std::cout << "ClassWithFoo has foo: " << has_member_function<ClassWithFoo, void(ClassWithFoo::*)(), &ClassWithFoo::foo>::value << std::endl;
//     // 检查ClassWithFoo是否有bar函数
//     std::cout << "ClassWithFoo has bar: " << has_member_function<ClassWithFoo, int(ClassWithFoo::*)(int), &ClassWithFoo::bar>::value << std::endl;
//     // 检查ClassWithoutFoo是否有foo函数
//     // std::cout << "ClassWithoutFoo has foo: " << has_member_function<ClassWithoutFoo, void(ClassWithoutFoo::*)(), &ClassWithoutFoo::foo>::value << std::endl;
//     return 0;
// }
// 示例类
// class ClassWithFoo {
// public:
//     void foo() {}
// };

// class ClassWithoutFoo {};

// int main() {
//     static_assert(has_foo_v<ClassWithFoo>, "ClassWithFoo should have foo");
//     static_assert(!has_foo_v<ClassWithoutFoo>, "ClassWithoutFoo should not have foo");

//     return 0;
// }
