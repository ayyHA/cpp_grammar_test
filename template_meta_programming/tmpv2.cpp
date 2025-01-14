#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <typeinfo>
#include <vector>

/* ========================TMPV2 1=========================== */
template <typename T> struct is_reference {
  static constexpr bool value = false;
};

template <typename T> struct is_reference<T &> {
  static constexpr bool value = true;
};

template <typename T> struct is_reference<T &&> {
  static constexpr bool value = true;
};

template <typename T> struct remove_reference {
  using type = T;
};

template <typename T> struct remove_reference<T &> {
  using type = T;
};

template <typename T> struct remove_reference<T &&> {
  using type = T;
};

template <typename T>
void check_type_info(typename remove_reference<T>::type check_var) {

  if (typeid(check_var) == typeid(int))
    std::cout << "input is int" << std::endl;
  else if (typeid(check_var) == typeid(float))
    std::cout << "input is float" << std::endl;
  else if (typeid(check_var) == typeid(double))
    std::cout << "input is double" << std::endl;
}

/* ========================TMPV2 2=========================== */
template <bool b> struct bool_ {
  static constexpr bool value = b;
};

template <typename T> struct is_reference2 {
  using type = bool_<false>;
};

template <typename T> struct is_reference2<T &> {
  using type = bool_<true>;
};

template <typename T> struct is_reference2<T &&> {
  using type = bool_<true>;
};

template <typename T, T v> struct integral_constant {
  static constexpr T value = v;
  using value_type = T;
  using type = integral_constant;
  constexpr operator value_type() const noexcept {
    std::cout << "implicit casting ";
    return value;
  } // operator casting
  value_type operator()() const noexcept {
    return value;
  } // operator overloadding

  value_type operator+(const T &v2) noexcept {
    std::cout << "opeator+:" << value << std::endl;
    value += v2;
    return value;
  }
};

// alias template
template <bool b> using bool_constant = integral_constant<bool, b>;

using true_type = bool_constant<true>;
using false_type = bool_constant<false>;

template <typename T> struct is_reference3 {
  using type = false_type;
};

template <typename T> struct is_reference3<T &> {
  using type = true_type;
};

template <typename T> struct is_reference3<T &&> {
  using type = true_type;
};

/* ========================TMPV2 3=========================== */
template <typename T> struct is_reference4 : false_type {};
template <typename T> struct is_reference4<T &> : true_type {};
template <typename T> struct is_reference4<T &&> : true_type {};

template <typename T> struct is_digit_or_reference : is_reference4<T> {};

template <> struct is_digit_or_reference<int> : true_type {};

template <> struct is_digit_or_reference<float> : true_type {};

template <> struct is_digit_or_reference<double> : true_type {};

template <typename T> struct type_identity {
  using type = T;
};

template <typename T> struct remove_reference2 : type_identity<T> {};

template <typename T> struct remove_reference2<T &> : type_identity<T> {};

template <typename T> struct remove_reference2<T &&> : type_identity<T> {};

/*variable template _v*/
template <typename T> constexpr bool is_reference_v = is_reference4<T>::value;

/*alias template _t*/
template <typename T> using is_reference_t = typename is_reference4<T>::type;

/* ========================TMPV2 4=========================== */
template <typename T, typename U> struct is_same : false_type {};

template <typename T> struct is_same<T, T> : true_type {};

template <typename T, typename U>
constexpr bool is_same_v = is_same<T, U>::value;

/* 写这个的时候这个继承,我都忘了前面写了这个 */
template <typename T, typename U, typename... Rest>
struct is_one_of
    : bool_constant<is_one_of<T, U>::value || is_one_of<T, Rest...>::value> {};

template <typename T, typename U> struct is_one_of<T, U> : is_same<T, U> {};

template <typename Inst, template <typename...> typename Tmpl>
struct is_instantiation_of : false_type {};

template <typename... Args, template <typename...> typename Tmpl>
struct is_instantiation_of<Tmpl<Args...>, Tmpl> : true_type {};

template <bool b, typename T, typename F>
struct conditional : type_identity<T> {};

template <typename T, typename F>
struct conditional<false, T, F> : type_identity<F> {};

template <bool b, typename T, typename F>
using conditional_t = typename conditional<b, T, F>::type;

/* 通过conditional实现短路求值(short-circuit-evaluation) */
template <typename T, typename U, typename... Rest>
struct is_one_of2
    : conditional_t<is_same_v<T, U>, true_type, is_one_of2<T, Rest...>> {};

template <typename T, typename U>
struct is_one_of2<T, U>
    : conditional_t<is_same_v<T, U>, true_type, false_type> {};
// 以下为对is_one_of和is_one_of2的短路求值测试
// template <int i> using integer_constant = integral_constant<int, i>;

// template <typename T, typename U, typename... Rest>
// struct is_one_of_2_test
//     : integer_constant<is_one_of_2_test<T, U>::value ||
//                        is_one_of_2_test<T, Rest...>::value> {};

// template<typename T,typename U>
// struct is_one_of_2_test<T,U> :
//

/* ========================TMPV2 5=========================== */
template <typename T> struct rank : integral_constant<int, 0> {};

template <typename T>
struct rank<T[]> : integral_constant<int, rank<T>::value + 1> {};

template <typename T, int N>
struct rank<T[N]> : integral_constant<int, rank<T>::value + 1> {};

template <typename T> constexpr int rank_v = rank<T>::value;

template <typename T, unsigned int N = 0>
struct extent : integral_constant<int, 0> {};

template <typename T> struct extent<T[], 0> : integral_constant<int, 0> {};

template <typename T, unsigned int N>
struct extent<T[], N> : extent<T, N - 1> {};

template <typename T, unsigned int I, unsigned int N>
struct extent<T[I], N> : extent<T, N - 1> {};

template <typename T, unsigned int I>
struct extent<T[I], 0> : integral_constant<int, I> {};

// test rank
int main() {
  /* ========================TMPV2 1=========================== */
  std::cout << is_reference<int>::value << std::endl;
  std::cout << is_reference<int &>::value << std::endl;
  std::cout << is_reference<int &&>::value << std::endl;

  int &&rm_ref1 = 11;
  float rm = 2.2f;
  float &rm_ref2 = rm;
  check_type_info<int &&>(rm_ref1);
  check_type_info<float &>(rm_ref2);
  check_type_info<decltype(rm)>(rm);

  /* ========================TMPV2 2=========================== */
  /*
      为了规范化,只返回type用作,已经有的value则封装类模板,类模板返回实例(就是一个具体的类),内部有value
   */
  std::cout << "is_reference2: " << std::endl;
  std::cout << is_reference2<int &&>::type::value << std::endl;
  std::cout << is_reference2<float>::type::value << std::endl;
  std::cout << is_reference2<int &>::type::value << std::endl;

  std::cout << "integral_constant: " << std::endl;
  std::cout << integral_constant<int, 1>::type::value << std::endl;
  std::cout << integral_constant<int, 2>::type() << std::endl;
  std::cout << integral_constant<int, 3>::type()() << std::endl;

  std::cout << "is_reference3: " << std::endl;
  std::cout << is_reference3<int &>::type::value << std::endl;
  std::cout << is_reference3<float &&>::type() << std::endl;
  std::cout << is_reference3<double>::type()() << std::endl;

  /* ========================TMPV2 3=========================== */
  std::cout << "is_reference4: " << std::endl;
  std::cout << is_reference4<int &>::value << std::endl;
  std::cout << is_reference4<float &&>() << std::endl;
  std::cout << is_reference4<double>()() << std::endl;

  std::cout << "is_digit_or_reference: " << std::endl;
  std::cout << is_digit_or_reference<int &>::value << std::endl;
  std::cout << is_digit_or_reference<std::string>()() << std::endl;

  std::cout << "remove_reference2: " << std::endl;
  check_type_info<int &&>(rm_ref1);

  std::cout << "variable template and alias template: " << std::endl;
  if (typeid(is_reference_t<int &>) == typeid(true_type))
    std::cout << "true_type " << is_reference_v<int &> << std::endl;
  else if (typeid(is_reference_t<int &>) == typeid(false_type))
    std::cout << "false_type " << is_reference_v<int &> << std::endl;

  /* ========================TMPV2 4=========================== */
  std::cout << "is_same: " << std::endl;
  std::cout << is_same_v<int, float> << std::endl;
  std::cout << is_same_v<int, int> << std::endl;

  std::cout << "is_one_of: " << std::endl;
  int tmpv2_4 = 1;
  std::cout << is_one_of<decltype(tmpv2_4), float, double, int>::value
            << std::endl;
  std::cout << is_one_of<decltype(tmpv2_4), float, double>::value << std::endl;

  std::vector<int> tmpv2_v1;
  std::list<int> tmpv2_l1;
  std::cout << "is_instantiation_of: " << std::endl;
  std::cout << is_instantiation_of<decltype(tmpv2_v1), std::vector>::value
            << std::endl;
  std::cout << is_instantiation_of<decltype(tmpv2_l1), std::vector>::value
            << std::endl;

  std::cout << "conditional: " << std::endl;
  std::cout << (typeid(conditional_t<false, int, float>) == typeid(float))
            << std::endl;
  std::cout << (typeid(conditional_t<true, int, float>) == typeid(float))
            << std::endl;

  std::cout << "is_one_of2: " << std::endl;
  std::cout << is_one_of2<decltype(tmpv2_4), int, float, double>::value
            << std::endl;
  std::cout << is_one_of<decltype(tmpv2_4), int, float, double>::value
            << std::endl;

  // is_one_of_2_test尚未实现,如何测试短路求值

  /* ========================TMPV2 5=========================== */
  int tmpv2_arr1[2][3][4];
  std::cout << "rank: " << std::endl;
  std::cout << rank_v<decltype(tmpv2_arr1)> << std::endl;
  std::cout << rank_v<int[][3]> << std::endl;
  std::cout << rank<int[][4]>::value << std::endl;
  // 如何测试递归?

  std::cout << "extent: " << std::endl;
  std::cout << extent<decltype(tmpv2_arr1), 1>::value << std::endl;
  std::cout << extent<int[][2], 0>::value << std::endl;
  std::cout << extent<int[][2], 1>::value << std::endl;
  if (is_same<int[2][3][4], decltype(tmpv2_arr1)>::value)
    std::cout << "SAME" << std::endl;
  return 0;
}