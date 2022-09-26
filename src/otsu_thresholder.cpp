#include "otsu_thresholder.hpp"

void OtsuThresholder::applyThresholding()
{    
    for (const auto & entry : fs::directory_iterator(this->sPath))
    {
        std::string fileName = entry.path().filename().string();

        if(fileName.substr(fileName.size()-3).compare("png") == 0)
        {
            std::string outputNameOtsu = this->outputFileName + "_" + fileName.substr(0, fileName.size()-4) + "_otsu.jpg";
            std::string outputNameCombined = this->outputFileName + "_" + fileName.substr(0, fileName.size()-4) + "_combined.jpg";
        
            std::cout << "------------------ Parameters -------------------" << std::endl;
            std::cout << "Path = " << sPath << std::endl;
            std::cout << "Output 1 = " << outputNameOtsu << std::endl;
            std::cout << "Output 2 = " << outputNameCombined << std::endl;
            std::cout << "-------------------------------------------------" << std::endl;

            // Read image into Mat
            cv::Mat image = cv::imread(entry.path().string(), cv::IMREAD_GRAYSCALE);

            // Get histogram and mean
            std::vector<double> hist = calculateHistogram(image);
            double mean = getHistogramMean(hist);

            int optThresh = calculateOptimalThreshold(hist, mean);
            
            // Apply threshold to output image
            cv::Mat output = image.clone();

            for(int i = 0; i < output.rows; ++i)
            {
                for(int j = 0; j < output.cols; ++j)
                {
                    output.at<uchar>(i,j) = output.at<uchar>(i,j) > optThresh ? 255 : 0;
                }
            }

            // Create combined image to view results side-by-side
            cv::Mat combinedImage(image.rows, 2*image.cols, CV_8U);
            cv::Mat left(combinedImage, cv::Rect(0, 0, image.cols, image.rows));
            image.copyTo(left);
            cv::Mat center(combinedImage, cv::Rect(image.cols, 0, image.cols, image.rows));
            output.copyTo(center);

            // Save results (write to file)
            cv::imwrite(outputNameOtsu, output);
            cv::imwrite(outputNameCombined, combinedImage);

            // Show combined image
            cv::imshow("(1) Original : (2) Thresholded", combinedImage);
            cv::waitKey(0);
        }
    }
}
/// @brief Calculates the optimal threshold
/// @param hist Histogram of greyvalues (normalized)
/// @param mean Mean of histogram
/// @return Returns the optimal threshold (between 0-255)
int OtsuThresholder::calculateOptimalThreshold(std::vector<double>& hist, double& mean)
{
    double maxVarSquared = 0.0;
    int optThresh = 0;

    // Skip zeroes at the beginning of histogram
    int firstIndex = getFirstNonzeroIndex(hist);

    for(int t = firstIndex; t < 256; ++t)
    {
        double weight1 = calculateWeight1(t, firstIndex, hist);
        double mean1 = calculateMean1(t, firstIndex, hist);
        double mean2 = (mean - weight1*mean1)/(1-weight1);

        double varSquared = weight1*(1.0-weight1)*pow((mean1-mean2), 2);

        if(varSquared > maxVarSquared)
        {
            optThresh = t;
            maxVarSquared = varSquared;
        }
    }

    return optThresh;
}

/// @brief Calculates the normalized greyvalue histogram
/// @param image Input greyscale image
/// @return A vector of P values
std::vector<double> OtsuThresholder::calculateHistogram(cv::Mat& image)
{
    std::vector<double> hist(256);
    int numPixels = image.total();

    for(int i = 0; i < image.rows; ++i)
    {
        for(int j = 0; j < image.cols; ++j)
        {
            hist.at(image.at<uchar>(i,j)) = hist.at(image.at<uchar>(i,j)) + 1;
        }
    }

    std::transform(hist.begin(), hist.end(), hist.begin(), [&numPixels](auto &c){ return c/numPixels; });

    return hist;
}

/// @brief Calculate the mean of the histogram
/// @param hist Histogram of greyvalues (normalized)
/// @return Returns the mean
double OtsuThresholder::getHistogramMean(std::vector<double>& hist)
{
    double mean = 0.0;

    for(int i = 0; i < hist.size(); ++i)
    {
        mean = mean + (i*hist.at(i));
    }

    return mean;
}

/// @brief Recursively calculates weight of Class 1
/// @param t Greyvalue
/// @param firstNZIndex First greyvalue where P(t) is not zero
/// @param hist Histogram of greyvalues (normalized)
/// @return Returns the weight of Class 1
double OtsuThresholder::calculateWeight1(int t, int& firstNZIndex, std::vector<double>& hist)
{
    if (t == 0) // terminating condition
    {
        return hist.at(firstNZIndex);
    }
    else
    {
        return calculateWeight1(t-1, firstNZIndex, hist) + hist.at(t);
    }
}

/// @brief Recursively calculates mean of Class 1
/// @param t Greyvalue
/// @param firstNZIndex First greyvalue where P(t) is not zero
/// @param hist Histogram of greyvalues (normalized)
/// @return Returns the mean of Class 1
double OtsuThresholder::calculateMean1(int t, int& firstNZIndex, std::vector<double>& hist)
{
    if (t == 0) // terminating condition
    {
        return 0;
    }
    else
    {
        return (calculateWeight1(t-1, firstNZIndex, hist)*calculateMean1(t-1, firstNZIndex, hist) + (double)t*hist.at(t))/calculateWeight1(t, firstNZIndex, hist);
    }
}

/// @brief Determines the first greyvalue where P(t) is not zero
/// @param hist Histogram of greyvalues (normalized)
/// @return Returns the first greyvalue where P(t) is not zero
int OtsuThresholder::getFirstNonzeroIndex(std::vector<double>& hist)
{
    int index = 0;

    while(hist.at(index) == 0)
    {
        index++;
    }

    return index;
}