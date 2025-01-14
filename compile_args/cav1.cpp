#include <iostream>

using namespace std;
__attribute__((deprecated("function old call"))) void f()
{
    printf("call f\n");
}

struct S
{
    short var[3];
} __attribute__((aligned(8)));

__attribute__((deprecated("old function call"))) void
f2()
{
    printf("fff\n");
}
int main()
{
    f();
    f2();
    S sss;
    size_t size = sizeof(sss);
    cout << size << endl;
    return 0;
}
