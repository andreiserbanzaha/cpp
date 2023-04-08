#include <iostream>
#include <type_traits>

template <typename T>
void func(T&& val)
{
}

int main()
{
    func(42);			// int&&

    int a = 10;
    func(a);			// int&

    const int x = 20;
    func(x);			//	int const&

    return 0;
}
