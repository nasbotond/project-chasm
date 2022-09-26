#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <string>

#include "edge_detector.hpp"

int main(int argc, char* argv[]) 
{
    if (argc < 3) 
    {
        std::cerr << "Usage: " << argv[0] << " <path to directory containing images> <output file prefix>" << std::endl;
        return 1;
    }

    std::string sPath = argv[1];
    std::string outputFileName = argv[2];

    // Check if path is valid
    if(!std::filesystem::exists(sPath)) 
    {
        std::cout << "Path is not valid!" << std::endl;
        return -1;
    }

    // Input
    EdgeDetector detector = EdgeDetector(sPath, outputFileName);    

    try 
    {
        detector.getEdges();
    } 
    catch(const std::exception &e) 
    {
        std::cerr << e.what() << '\n';
        return -1;
    }

    return 0;
}