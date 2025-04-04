#ifndef _DOWNLOADER_
#define _DOWNLOADER_
#include<string>
class Downloader
{
    public:
    static void download(const std::string&url,const std::string&filePath);
};
#endif