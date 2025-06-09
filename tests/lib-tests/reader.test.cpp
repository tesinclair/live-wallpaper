#include "reader.test.h"

void testReader(int silent){
    char* filename = generateFilename();
    MP4TestData testData = mockMP4(filename);
    if (testData.track.version == (uint8_t)-1){ // returns max uint8 if errors
        std::cout << "Failed to generate Mock MP4 files. Aborting..." << std::endl;
        exit(1);
    }

    if (silent){
        testConstruct(filename);
        testGetVideoData(filename, testData.track);
        testGetCurrentFrame(filename, testData.frame);
        testDestruct(filename);
        return;
    }
    std::cout << "Testing Construction of Reader Class" << std::endl;
    testConstruct(filename);
    std::cout << "\033[32mPassed.\033[00m" << std::endl;
    std::cout << "Testing Retrieval of Video Data" << std::endl;
    testGetVideoData(filename, testData.track);
    std::cout << "\033[32mPassed.\033[00m" << std::endl;
    std::cout << "Testing Retrieval of Current Frame" << std::endl;
    testGetCurrentFrame(filename, testData.frame);
    std::cout << "\033[32mPassed.\033[00m" << std::endl;
    std::cout << "Testing Destruction of Reader Class" << std::endl;
    testDestruct(filename);
    std::cout << "\033[32mPassed.\033[00m" << std::endl;
}

void testContruct(char* testfile){
}

void testGetVideoData(char* testfile, videoTrack testTrack){
}

void testGetCurrentFrame(char* testfile, char* testFrame){
}

void testDestruct(char* testfile){
}
