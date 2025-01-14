#include <iostream>
#include <string>
#include <typeinfo>
#include <cstdint>
using std::cout;
using std::endl;
using std::string;

template <int n>
void funcPrint()
{
    cout << n << endl;
}

template <typename T>
void whichType()
{
    T val;
    cout << typeid(val).name() << endl;
}

template <typename T>
class clsTemp
{
public:
    T val;
    // clsTemp(){
    //     val = 1;
    // }
    void display()
    {
        cout << "From clsTemp,datatype:" << typeid(val).name() << endl;
    }
};

template <template <typename T> typename tmplClass>
class clsTempTempPara
{
public:
    tmplClass<int> tmp;
    // clsTempTempPara(){
    //     tmp = tmplClass();
    // }
    void print()
    {
        cout << "From clsTempTempPara" << endl;
        tmp.display();
    }
};

template <int... args>
class packtmpl1
{
};

template <typename... args>
class packtmpl2
{
};

template <template <typename T> typename... args>
class packtmpl3
{
};

template <typename T, typename U>
void funcTmpl(T t, U u)
{
    cout << "primary template" << t << " " << u << endl;
}

template <>
void funcTmpl(int t, float u)
{
    cout << "full specialization" << t << " " << u << endl;
};

template void funcTmpl<string, string>(string, string); // 显式实例化

template <typename T>
void func(T t) { cout << t << endl; }
// 显式实例化
template void func(int);

template <typename T, typename U>
struct clsTmpl
{
    void display() { cout << "primary template " << endl; }
};

template <typename U>
struct clsTmpl<int, U>
{
    void display() { cout << "partial specialization " << endl; }
};

template <>
struct clsTmpl<int, float>
{
    void display() { cout << "full specialization " << endl; }
};

int main()
{
    // static int a = 2;
    const int b = 3;
    // funcPrint<a>();
    funcPrint<b>();
    whichType<int>();
    whichType<float>();
    whichType<uint32_t>();

    // constexpr clsTemp<int> classTemp;
    // classTemp.display();
    clsTempTempPara<clsTemp> classTempTempPara;
    classTempTempPara.print();

    packtmpl1<1, 2, 3> pinst;
    packtmpl2<int, float, string> pinst2;
    packtmpl3<clsTemp> pinst3;

    int t = 1;
    float u = 2.0;
    funcTmpl(t, u);
    funcTmpl(t, "123");

    clsTmpl<string, int> ctm1;
    ctm1.display();
    clsTmpl<int, int> ctm2;
    ctm2.display();
    clsTmpl<int, float> ctm3;
    ctm3.display();

    return 0;
}