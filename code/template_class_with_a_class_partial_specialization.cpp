#include <vector>

template <typename T>
class Foo
{
};

template <typename T>
class Foo<std::vector<T>>
{
};

int main()
{
    Foo<int> fooInt;
    Foo<std::vector<int>> fooVecInt;

    return 0;
}
