#include <cstdlib>
#include <iostream>
#include <tuple>
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

/*==================== EXAMPLE 4 add_l/rvalue_reference ====================*/
/*
  Some data type like "void",don't have void&,so we need use SFINAE to choose
  which type is legal, reference or itself. Also when we use like "char",both
  function are legal,through partial order rule,we will choose the better
  one.Because we use "...", an old c-style variadic parameter in another
  parameter list, it's less suitable than the other.
  More about "..." detail info can see "cstdarg"
*/
template <typename T> type_identity<T &> try_add_lvalue_reference(int);
template <typename T> type_identity<T> try_add_lvalue_reference(...);

template <typename T> type_identity<T &&> try_add_rvalue_reference(int);
template <typename T> type_identity<T> try_add_rvalue_reference(...);

template <typename T>
struct add_lvalue_reference : decltype(try_add_lvalue_reference<T>(0)) {};

template <typename T>
struct add_rvalue_reference : decltype(try_add_rvalue_reference<T>(0)) {};

template <typename T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

template <typename T>
using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

/*==================== EXAMPLE 5 is_copy_assignable ====================*/
/*
  Through 'copy_assign_t' to get the data type,if opeartor= is allowed to
  invoke.Via void_t to choose true_type or false type,which is similar with
  'has_member_type'.
*/
template <typename T>
using copy_assign_t = decltype(declval<T &>() = declval<const T &>());

template <typename T, typename U = void>
struct is_copy_assignable : false_type {};

template <typename T>
struct is_copy_assignable<T, void_t<copy_assign_t<T>>> : true_type {};

template <typename T, typename U = void>
constexpr bool is_copy_assignable_v = is_copy_assignable<T, U>::value;

struct S5 {
  S5 &operator=(const S5 &) = delete; // through '=delete' to abandon operator=
};

/*==================== EXAMPLE 6 self-definition tuple ====================*/
/*
  class template tuple written by recursive inherit, via initialize function to
  achieve class template arguments deduction(CTAD),match partial specialization,
  while argument is empty,match primary template.
*/
template <typename... Args> struct tuple {
  tuple(Args...) {}
};

template <typename T, typename... Args>
struct tuple<T, Args...> : tuple<Args...> {
  T value_;
  tuple(T v, Args... args) : value_(v), tuple<Args...>(args...) {}
};

/*
  Through recursive methods to destruct tuple,like 'rank' to destruct array.
*/
template <unsigned int N, typename Tmpl> struct tuple_element;
/* recursive inheritance */
template <unsigned int N, typename T, typename... Args>
struct tuple_element<N, tuple<T, Args...>>
    : tuple_element<N - 1, tuple<Args...>> {};

/* We can through tuple_element<>::type and tuple_element<>::__tuple_type to get
 * some data type. */
template <typename T, typename... Args>
struct tuple_element<0, tuple<T, Args...>> : type_identity<T> {
  using __tuple_type = tuple<T, Args...>;
};

template <unsigned int N, typename Tmpl>
using tuple_element_t = typename tuple_element<N, Tmpl>::type;

template <unsigned int N, typename... Args>
tuple_element_t<N, tuple<Args...>> &get(tuple<Args...> &t) {
  using tuple_type = typename tuple_element<N, tuple<Args...>>::__tuple_type;
  return static_cast<tuple_type &>(t).value_;
}

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

  /* =========== EXAMPLE 4 add_l/rvalue_reference =========== */
  std::cout << "add_l/rvalue_reference: " << std::endl;
  std::cout << is_same_v<int &, add_lvalue_reference_t<int>> << std::endl;
  std::cout << is_same_v<void, add_rvalue_reference_t<void>> << std::endl;
  std::cout << is_same_v<float &&, add_rvalue_reference_t<float>> << std::endl;

  /* =========== EXAMPLE 5 is_copy_assignable =========== */
  std::cout << "is_copy_assignable: " << std::endl;
  std::cout << is_copy_assignable_v<int> << std::endl;
  std::cout << is_copy_assignable_v<S5> << std::endl;

  /* =========== EXAMPLE 6 tuple =========== */
  std::cout << "tuple: " << std::endl;
  // tuple<char,float,float,char> t = tuple('1', 2.2f, 3.3f, 'a');
  auto t = tuple('1', 2.2f, 3.3f, 'a', std::string("😄"));
  std::cout << std::is_same_v<tuple_element<1, decltype(t)>::__tuple_type,
                              tuple<float, float, char>> << std::endl;
  std::cout << is_same_v<char, tuple_element_t<3, decltype(t)>> << std::endl;
  std::cout << get<4>(t) << std::endl;
  return 0;
}