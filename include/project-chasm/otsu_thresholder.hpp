#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#define _USE_MATH_DEFINES

#include "opencv2/opencv.hpp"
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <numeric>
#include <vector>
#include <cmath>
#include <stdint.h>
#include <functional> 
namespace fs = std::filesystem;

class OtsuThresholder
{
    private:
    
        std::string sPath;
        std::string outputFileName;

    public:

        OtsuThresholder(const std::string &sPath, const std::string &outputFileName) : sPath(sPath), outputFileName(outputFileName) {}
        ~OtsuThresholder() {}

        // Main functions
        void calculateOptimalThreshold();
        std::vector<double> calculateHistogram(cv::Mat& image);
        double getHistogramMean(std::vector<double>& hist);
        double q1(int t, std::vector<double>& hist);
        double mean1(int t, std::vector<double>& hist);
};