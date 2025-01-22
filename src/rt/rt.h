#ifndef _RT_H
#define _RT_H
#include<chrono>
#include<string>
class RT
{
    std::chrono::time_point<std::chrono::high_resolution_clock> _start{};
    std::chrono::time_point<std::chrono::high_resolution_clock> _end{};
    std::string _msg{};
public:
    RT(const std::string&msg);
    ~RT();
};
#endif