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
  time(&raw_time);
  std::string time_str = ctime(&raw_time);
  time_str = time_str.erase(time_str.length()-1,1);
  findAndReplaceAll(time_str,std::string(" "),std::string("_"));
  findAndReplaceAll(time_str,std::string(":"),std::string("-"));
  return time_str;
}
