#include "otsu_thresholder.hpp"

void OtsuThresholder::calculateOptimalThreshold()
{
    // mean^2 = q1(t)*[1 − q1(t)]*[μ1(t) − μ2(t)]^2
    
    for (const auto & entry : fs::directory_iterator(this->sPath))
    {
        std::string fileName = entry.path().filename().string();

        if(fileName.substr(fileName.size()-3).compare("png") == 0)
        {
            std::string outputNamePrewitt = this->outputFileName + "_" + fileName.substr(0, fileName.size()-4) + "gradientMagnitude.jpg";
            std::string outputNamePrewittNMS = this->outputFileName + "_" + fileName.substr(0, fileName.size()-4) + "_PrewittNMS.jpg";
            std::string outputNameCombined = this->outputFileName + "_" + fileName.substr(0, fileName.size()-4) + "_combined.jpg";
        
            std::cout << "------------------ Parameters -------------------" << std::endl;
            std::cout << "Path = " << sPath << std::endl;
            std::cout << "Output 1 = " << outputNamePrewitt << std::endl;
            std::cout << "Output 2 = " << outputNamePrewittNMS << std::endl;
            std::cout << "Output 3 = " << outputNameCombined << std::endl;
            std::cout << "-------------------------------------------------" << std::endl;

            // Read image into Mat
            cv::Mat image = cv::imread(entry.path().string(), cv::IMREAD_GRAYSCALE);

            std::vector<double> hist = calculateHistogram(image);
            double mean = getHistogramMean(hist);

            double maxVarSquared = 0.0;
            int optThresh = 0;

            for(int t = 0; t < 256; ++t)
            {
                double q1v = q1(t, hist);
                double mean1v = mean1(t, hist);
                double mean2v = (mean - q1v*mean1v)/(1-q1v);

                double varSquared = q1v*(1.0-q1v)*pow((mean1v-mean2v), 2);

                if(varSquared > maxVarSquared)
                {
                    optThresh = t;
                    maxVarSquared = varSquared;
                }
            }
            std::cout << optThresh << std::endl;

            // cv::Mat outputPrewitt;
            // cv::Mat outputPrewittNMS;

            // prewittEdgeDetectorWithNMS(image, outputPrewitt, outputPrewittNMS);

            // // Create combined image to view results side-by-side
            // cv::Mat combinedImage(image.rows, 3*image.cols, CV_8U);
            // cv::Mat left(combinedImage, cv::Rect(0, 0, image.cols, image.rows));
            // image.copyTo(left);
            // cv::Mat center(combinedImage, cv::Rect(image.cols, 0, image.cols, image.rows));
            // outputPrewitt.copyTo(center);
            // cv::Mat right(combinedImage, cv::Rect(2*image.cols, 0, image.cols, image.rows));
            // outputPrewittNMS.copyTo(right);

            // // Save results (write to file)
            // cv::imwrite(outputNamePrewitt, outputPrewitt);
            // cv::imwrite(outputNamePrewittNMS, outputPrewittNMS);
            // cv::imwrite(outputNameCombined, combinedImage);

            // // Show combined image
            // cv::imshow("(1) Original : (2) Gradient Magnitude : (3) Prewitt Edge Detection with NMS", combinedImage);
            // cv::waitKey(0);
        }
    }
}


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

double OtsuThresholder::getHistogramMean(std::vector<double>& hist)
{
    double mean = 0.0;

    for(int i = 0; i < hist.size(); ++i)
    {
        mean = mean + (i*hist.at(i));
    }

    return mean;
}

double OtsuThresholder::q1(int t, std::vector<double>& hist)
{
    if (t == 0) // terminating value
    {
        return hist.at(0);
    }
    else
    {
        return q1(t-1, hist) + hist.at(t);
    }
}

double OtsuThresholder::mean1(int t, std::vector<double>& hist)
{
    if (t == 0) // terminating value
    {
        return 0;
    }
    else
    {
        return (q1(t-1, hist)*mean1(t-1, hist) + t*hist.at(t))/q1(t, hist);
    }
}