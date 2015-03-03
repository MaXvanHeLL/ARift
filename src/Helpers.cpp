#include <iostream>
#include <string.h>
#include <sstream>
#include <time.h>

void findAndReplaceAll(std::string& s,const std::string to_replace,const std::string replacement)
{
  size_t pos = s.find(to_replace);;
  while((int)pos > (-1))
  {
    s.replace(pos, to_replace.length(), replacement);
    pos = s.find(to_replace);
  }
}

std::string getTimeString()
{
  time_t raw_time;
  struct tm timeinfo;
  char buffer[20];
  time(&raw_time);
  localtime_s(&timeinfo, &raw_time);
  strftime(buffer, 20, "%F_%H-%M-%S", &timeinfo);

//  std::string time_str = ctime(&raw_time);
  std::string time_str = std::string(buffer);
  std::cout << "Time str " << time_str;
  time_str = time_str.erase(time_str.length()-1,1);
  findAndReplaceAll(time_str,std::string(" "),std::string("_"));
  findAndReplaceAll(time_str,std::string(":"),std::string("-"));
  return time_str;
}
