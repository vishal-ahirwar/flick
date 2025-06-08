#pragma once
#include <string>
class Unzip
{
      private:
	static int copyData(struct archive* ar, struct archive* aw);

      public:
	static bool unzip(const std::string& archivePath, const std::string& outputDir);
};
