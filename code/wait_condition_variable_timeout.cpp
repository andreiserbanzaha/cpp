#include <iostream>
#include <condition_variable>
#include <chrono>
#include <mutex>

std::condition_variable cv;
std::mutex mtx;
bool done;

bool waitLoop()
{
    auto const timeout = std::chrono::steady_clock::now() +
        std::chrono::milliseconds{ 10 };

    int i = 0;
    std::unique_lock<std::mutex> lk{ mtx };

    while(!done && i < 1000000)
    {
        if(cv.wait_until(lk, timeout) == std::cv_status::timeout)
        {
            std::cout << "TIMEOUT AT: " << i << '\n';
            break;
        }
        ++i;
    }
    return done;
}

int main()
{
    waitLoop();

    return 0;
}
