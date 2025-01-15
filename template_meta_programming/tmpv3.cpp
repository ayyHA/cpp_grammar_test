#include <cstdlib>
#include <iostream>
#include <typeinfo>

template <typename T> struct type_identity {
  using type = T;
};

template <typename T, T v> struct integral_constant {
  using value_type = T;
  static constexpr T value = v;
  using type = integral_constant;
  constexpr operator value_type() const noexcept { return value; }
  value_type operator()() const noexcept { return value; }
};

template <bool B> struct bool_constant : integral_constant<bool, B> {};

using true_type = bool_constant<true>;
using false_type = bool_constant<false>;

template <typename T> struct is_integral : false_type {};
template <> struct is_integral<int> : true_type {};
template <typename T> constexpr bool is_integral_v = is_integral<T>::value;

template <typename T> struct is_float_point : false_type {};
template <> struct is_float_point<float> : true_type {};
template <typename T>
constexpr bool is_float_point_v = is_float_point<T>::value;

template <bool B, typename T = void> struct enable_if : type_identity<T> {};

template <typename T> struct enable_if<false, T> {};

// enable_if_t<false> is ill-formed while in function template overload
// resolution will make SFINAE we can use SFINAE to control overload sets.
template <bool B, typename T = void>
using enable_if_t = typename enable_if<B, T>::type;

// function template
template <typename T> enable_if_t<is_integral_v<T>> func(T) { // #1
  std::cout << "INTINTINT" << std::endl;
};
template <typename T> enable_if_t<is_float_point_v<T>> func(T) { // #2
  std::cout << "FLOATFLOATFLOAT" << std::endl;
};

template <typename... Args> struct always_true : true_type {};
template <typename... Args>
constexpr bool always_true_v = always_true<Args...>::value;

// Because don't have short-circuit evaluation,we can use 'always_true_v' to use
// function template parameter to postpone enable_if_t's initialization when
// initialization class template.
// When choose which function template is better, return type make SFINAE, here
// is immediate context~
template <typename T> struct S {
  template <typename U>
  static enable_if_t<always_true_v<U> && is_integral_v<T>, int> foo(U u) {
    std::cout << u << " INTINT" << std::endl;
  }
  template <typename U>
  static enable_if_t<always_true_v<U> && is_float_point_v<T>, int> foo(U u) {
    std::cout << u << " FLOATFLOAT" << std::endl;
  }
};

int main() {
  func(1);    // in overload resolution,#2 make SFINAE,choose #1
  func(1.0f); // in overload resolution,#1 make SFINAE,choose #2
  S<int>::foo(111);
  return 0;
}