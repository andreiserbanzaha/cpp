#include <iostream>
#include <thread>
#include <future>
#include <chrono>

int calculatePower(int number)
{
    std::this_thread::sleep_for(std::chrono::seconds{2});
    return number * number;
}

int main()
{
    int number = 7;
    std::future<int> f = std::async(calculatePower, number);

    std::cout << "Calculation started!\n";
    std::cout << number << " * " << number << " = " << f.get() << '\n';
    std::cout << "Calculation finished!\n";

    return 0;
}
