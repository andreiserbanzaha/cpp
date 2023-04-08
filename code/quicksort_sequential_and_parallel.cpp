#include <iostream>
#include <algorithm>
#include <list>
#include <future>

template <typename T>
std::list<T> sequentialQuicksort(std::list<T> input)
{
    if (input.empty())
    {
        return input;
    }

    std::list<T> result;

    // take the first element as pivot by slicing it off the list
    result.splice(result.begin(), input, input.begin());

    // take a reference to the pivot to avoid copying
    T const& pivot = *result.begin();

    // divide into 2 lists (less than, and greater than the pivot)
    auto dividePoint = std::partition(
            input.begin(),
            input.end(),
            [&](T const& t){ return t < pivot; });

    std::list<T> lowerPart;
    lowerPart.splice(lowerPart.end(), input, input.begin(), dividePoint);

    auto newLower = sequentialQuicksort(std::move(lowerPart));
    auto newHigher = sequentialQuicksort(std::move(input));

    result.splice(result.end(), newHigher);
    result.splice(result.begin(), newLower);

    return result;
}

template <typename T>
std::list<T> parallelQuicksort(std::list<T> input)
{
    if (input.empty())
    {
        return input;
    }

    std::list<T> result;

    // take the first element as pivot by slicing it off the list
    result.splice(result.begin(), input, input.begin());

    // take a reference to the pivot to avoid copying
    T const& pivot = *result.begin();

    // divide into 2 lists (less than, and greater than the pivot)
    auto dividePoint = std::partition(
            input.begin(),
            input.end(),
            [&](T const& t){ return t < pivot; });

    std::list<T> lowerPart;
    lowerPart.splice(lowerPart.end(), input, input.begin(), dividePoint);

    std::future<std::list<T>> newLower{std::async(&parallelQuicksort<T>,
            std::move(lowerPart))};

    auto newHigher = parallelQuicksort(std::move(input));

    result.splice(result.end(), newHigher);
    result.splice(result.begin(), newLower.get());

    return result;
}

int main()
{
    std::list<int> list{ 10, 52, 13, 4, 64, 23, 88, 123, 6, 94 };

    auto resSequential = sequentialQuicksort(list);
    auto resParallel = parallelQuicksort(list);

    std::cout << "Sequential:\n";
    for(auto it = resSequential.begin(); it != resSequential.end(); ++it)
    {
        std::cout << *it << ' ';
    }
    std::cout << '\n';

    std::cout << "Parallel:\n";
    for(auto it = resParallel.begin(); it != resParallel.end(); ++it)
    {
        std::cout << *it << ' ';
    }
    std::cout << '\n';

    return 0;
}
