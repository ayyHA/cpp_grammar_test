#include <cstdlib>
#include <iostream>
#include <type_traits>
#include <typeinfo>
/*==================== EXAMPLE 1 enable_if ====================*/
/*
  enablt_if can choose overload sets based on logic.
*/
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

/*
    enable_if_t<false> is ill-formed while in function template overload
  resolution will make SFINAE we can use SFINAE to control overload sets.
*/
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

/*
    Because don't have short-circuit evaluation,we can use 'always_true_v' to
  use function template parameter to postpone enable_if_t's initialization when
  initialization class template.
    When choose which function template is better, return type make SFINAE, here
  is immediate context~
*/
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

/*==================== EXAMPLE 2 void_t ====================*/
/*
  void_t is a alias template, it can help us achieve hasattr function
  to check your T has T::type or not.
*/
template <typename... Args> using void_t = void;

template <typename T, typename U = void> struct has_member_type : false_type {};

template <typename T>
struct has_member_type<T, void_t<typename T::type>> : true_type {};

template <typename T>
constexpr bool has_member_type_v = has_member_type<T>::value;

/*==================== EXAMPLE 3 declval ====================*/
/*
  declval is a function template, we can use this function to make a fake
  variable in compile time.
  ONLY DECLARATION, NOT DEFINITION!
*/
template <typename T, typename U> struct is_same : false_type {};
template <typename T> struct is_same<T, T> : true_type {};
template <typename T, typename U>
constexpr bool is_same_v = is_same<T, U>::value;

template <typename T> enable_if_t<is_integral_v<T>, int> func3(T){};

template <typename T> enable_if_t<is_float_point_v<T>, float> func3(T){};

/* here use type_traits's template, ipt "T" opt "T&&" */
template <typename T> std::add_rvalue_reference_t<T> declval() noexcept;

template <typename T> struct S3 {
  decltype(func3<T>(declval<T>())) value_;
};

int main() {
  /* =========== EXAMPLE 1 enable_if =========== */
  std::cout << "enable_if: " << std ::endl;
  func(1);    // in overload resolution,#2 make SFINAE,choose #1
  func(1.0f); // in overload resolution,#1 make SFINAE,choose #2
  S<int>::foo(111);

  /* =========== EXAMPLE 2 void_t =========== */
  std::cout << "void_t: " << std ::endl;
  std::cout << has_member_type_v<int> << std::endl;
  std::cout << has_member_type_v<true_type> << std::endl;

  /* =========== EXAMPLE 3 declval =========== */
  std::cout << "declval: " << std::endl;
  std::cout << is_same_v<decltype(S3<int>().value_), int> << std::endl;
  std::cout << is_same_v<decltype(S3<float>().value_), float> << std::endl;
  return 0;
}