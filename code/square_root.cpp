#include <iostream>
#include <cmath>
#include <stdexcept>

double squareRoot(double x)
{
    if (x < 0)
    {
        throw std::out_of_range("x < 0");
    }
    return std::sqrt(x);
}

int main()
{
    try
    {
        std::cout << squareRoot(-1) << '\n';
    }
    catch(std::out_of_range& e)
    {
        std::cout << e.what() << '\n';
    }

    return 0;
}
