#include <string.h>
#include <opencv2/core/core.hpp>

#ifndef HELPERS_H_
#define HELPERS_H_

void findAndReplaceAll(std::string& s,const std::string to_replace,const std::string replacement);

std::string getTimeString(const char*);
std::string getTimeString();


void rotate(cv::Mat& src, double angle, cv::Mat& dst);
#endif // HELPERS_H_
