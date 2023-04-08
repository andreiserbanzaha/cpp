#include <iostream>

template <typename T>
struct Foo
{
    template <typename M>
    void setName(M name);
};

template <typename T>
template <typename M>
void Foo<T>::setName(M name)
{
}

int main()
{
    Foo<int> f;
    std::string name{"andrei"};
    f.setName<std::string>(name);

    return 0;
}
