#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace std;
using voidFunc = void (*)();

class Father
{
public:
    Father()
    {
        cout << "constructor Father" << endl;
    }
    virtual ~Father()
    {
        cout << "deconstructor Father" << endl;
    }
    virtual void print()
    {
        cout << "Father::printf" << endl;
    }
    virtual void print2()
    {
        cout << "Father::print2" << endl;
    }
};

class Son1 : public Father
{
public:
    Son1()
    {
        cout << "constructor Son1" << endl;
    }

    ~Son1()
    {
        cout << "deconstror Son1" << endl;
    }

    void print()
    {
        cout << "Son1::print" << endl;
    }
};

class Son2 : public Father
{
public:
    Son2()
    {
        cout << "constructor Son2" << endl;
    }
    ~Son2()
    {
        cout << "deconstructor Son2" << endl;
    }
    void print()
    {
        cout << "Son2::print" << endl;
    }
    void print2()
    {
        cout << "Son2:print2" << endl;
    }
};

class TestSizeA
{
public:
    int x;
    void func()
    {
        cout << "TestSizeA func" << endl;
    }
};

class TestSizeB
{
public:
    int x;
    virtual void func()
    {
        cout << "TestSizeB func" << endl;
    }
};

class TestInitC
{
public:
    int x;
    TestInitC()
    {
        memset(this, 0, sizeof(x) + sizeof(uintptr_t *));
    }
    virtual void func()
    {
        cout << "TestInitC func" << endl;
    }
};

int main()
{
    Father *f = new Son1();
    f->print();
    delete f;
    f = new Son2();
    f->print();
    delete f;
    Father f_;
    cout << sizeof(f_) << endl;
    TestSizeA a;
    TestSizeB b;
    cout << sizeof(a) << endl;
    cout << sizeof(b) << endl;
    cout << &a.x << endl;
    cout << reinterpret_cast<void *>(&a) << endl;
    cout << &b.x << endl;
    cout << reinterpret_cast<void *>(&b) << endl;
    TestInitC c;
    c.func();
    // TestInitC *c_ = new TestInitC();
    // c_->func();
    Father *fptr = new Son2();
    uintptr_t *func = reinterpret_cast<uintptr_t *>(fptr);
    uintptr_t *funcc = reinterpret_cast<uintptr_t *>(*func);
    voidFunc func1 = reinterpret_cast<voidFunc>(funcc[3]);
    cout << hex << funcc[0] << endl;
    func1();
    delete fptr;
}