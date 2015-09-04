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

std::string getTimeString(const char* format)
{
  time_t raw_time;
  struct tm timeinfo;
  char buffer[20];
  time(&raw_time);
  localtime_s(&timeinfo, &raw_time);
  strftime(buffer, 20, format, &timeinfo);

  std::string time_str = std::string(buffer);
  findAndReplaceAll(time_str, std::string(" "), std::string("_"));
  findAndReplaceAll(time_str, std::string(":"), std::string("-"));
  return time_str;
}

std::string getTimeString()
{
  return getTimeString("%Y-%m-%d_%H-%M-%S");
}