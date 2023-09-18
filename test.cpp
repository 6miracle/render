#include <iostream>
#include <cstdio>
#include <chrono>
#include <thread>

void c_out() {
   int sum = 0;
    for(int i = 1; i <= 100; ++i) {
        sum += i;
    }
    printf("sum = %d\n", sum);
}
void cpp_out() {
    int sum = 0;
    for(int i = 0; i <= 100; ++i) {
        sum += i;
    }
    std::cout << "sum = " << sum << '\n';
}       
double deltaTime = 0.0;
std::chrono::time_point<std::chrono::steady_clock> lastFrame;

void updateTime() {
    static std::chrono::time_point<std::chrono::steady_clock> lastFrame = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> currentFrame = std::chrono::steady_clock::now();
    deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentFrame - lastFrame).count() / 1000.0;
    lastFrame = currentFrame;
}
int main() {
   
    while(1) {
        updateTime();
        std::cout << "deltaTime = " << deltaTime <<'\n';
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    return 0;
}