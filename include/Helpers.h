#include <string.h>
#ifndef HELPERS_H_
#define HELPERS_H_

void findAndReplaceAll(std::string& s,const std::string to_replace,const std::string replacement);

std::string getTimeString(const char*);
std::string getTimeString();
#endif // HELPERS_H_
