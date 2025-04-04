#ifndef _RT_H
#define _RT_H
#include<chrono>
#include<string>
class RT
{
    std::chrono::time_point<std::chrono::high_resolution_clock> mStart{};
    std::chrono::time_point<std::chrono::high_resolution_clock> mEnd{};
    std::string mMsg{};
public:
    RT(const std::string&msg);
    ~RT();
};
#endif