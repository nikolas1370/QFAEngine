#define VMA_IMPLEMENTATION // whit this creates thing that is necessary for vk_mem_alloc
#include "VulkanSuff.h"
#include <Render/Time.h>
#include <chrono>
#include <thread>
#include <Windows.h>

void QFASleep(float delay)
{
    if (delay <= 0)
        return;
    using namespace std::chrono_literals;
    auto t = QTime::GetSystemTime();
    float _delay = delay - 1.0f;// possible latency 1ms+
    if (_delay > 0)
        std::this_thread::sleep_for(std::chrono::milliseconds((int)floor(_delay)));

    while (true)
    {
        _delay = delay - ((float)(QTime::GetSystemTime() - t) / 10000.0f); //  "/ 10000.0f" convert from 1/10 microseconds to 1ms
        if (_delay <= 0)
            return;

        std::this_thread::sleep_for(std::chrono::microseconds(0));
    }
}