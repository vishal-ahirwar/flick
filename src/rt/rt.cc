#include<rt/rt.h>
#include<cstdio>
#include<constants/colors.hpp>

RT::RT(const std::string&msg):_msg{msg}
{
    _start=std::chrono::high_resolution_clock::now();
};
RT::~RT()
{
    _end=std::chrono::high_resolution_clock::now();
    std::chrono::duration<double>elaped_time{_end-_start};
    fprintf(stdout,"Time taken by %s %.2fs\n",_msg.c_str(),elaped_time.count());
};