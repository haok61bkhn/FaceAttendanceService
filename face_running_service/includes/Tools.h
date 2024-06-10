#pragma once
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <random>
std::string RandomString(int length);
std::string GetTimeStampStr();
int GetTimeStamp();
bool CreateFolder(std::string& folderPath);

std::string ConvertTimeStampToDateTime(int timestamp);
std::string ConvertTimeStampToRTSPDateTime(int timestamp);
std::string GetDateTimeStr();

int GetDateTimeInt();

bool FileExists(const std::string& filename);

void Dist2Percent(float& dist, float& percent);
