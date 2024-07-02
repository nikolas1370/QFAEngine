#pragma once
#define QFAEXPORT __declspec(dllexport)

#define stopExecute(message) {\
std::cout << (message);   \
__debugbreak(); \
while (true);   \
}

// delay in millisecond
void QFAEXPORT QFASleep(float delay);

