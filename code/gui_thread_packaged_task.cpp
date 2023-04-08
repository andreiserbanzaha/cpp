#include <deque>
#include <mutex>
#include <future>
#include <thread>
#include <utility>

std::mutex m_mtx;
std::deque<std::packaged_task<void()>> m_tasks;

bool guiShutdownMessageReceived();
void getAndProcessGUIMessage();

void guiThread()
{
    while (!guiShutdownMessageReceived())
    {
        getAndProcessGUIMessage(); // poll GUI messages to handle
        std::packaged_task<void()> task;

        {
            std::lock_guard<std::mutex> lk{m_mtx};
            if (m_tasks.empty())
            {
                continue; // loop again
            }
            task = std::move(m_tasks.front());
            m_tasks.pop_front();
        }
        task();
    }
}

std::thread guiBackgroundThread(guiThread);

template <typename Func>
std::future<void> postTaskForGUIThread(Func f)
{
    std::packaged_task<void()> task(f);
    std::future<void> res = task.get_future();

    std::lock_guard<std::mutex> lk{m_mtx};
    m_tasks.push_back(std::move(task));
    return res;
}
