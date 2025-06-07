#include <iostream>
#include <cstring>
#include <cstdint>

#include "lib-tests/reader.test.h"
#include "lib-tests/decoder.test.h"
#include "lib-tests/streamer.test.h"

// OPT FLAGS
#define SILENT 0x01

#define CHECK_FLAG(libflags, flag) libflags & flag

void test_reader(int silent){
    std::cout << "Testing Reader" << std::endl;
    if (silent){
        std::cout << "Silent Activated\n\n";
    }
}
void test_decoder(int silent){
    std::cout << "Testing Decoder" << std::endl;
    if (silent){
        std::cout << "Silent Activated\n\n";
    }
}
void test_streamer(int silent){
    std::cout << "Testing Streamer" << std::endl;
    if (silent){
        std::cout << "Silent Activated\n\n";
    }
}

void printHelp(int errCode = 1){
    std::cout << "test-live-wallpaper [options]" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "\t--help\t\tDisplays this here help text" << std::endl;
    std::cout << "\t--silent\t\tRun tests, but only display fails" << std::endl;
    std::cout << "\t--lib [libraries]\t\tOnly runs tests for the given libraries (must come last)" << std::endl;
    std::cout << "\nE.g. test-live-wallpaper --silent --lib reader streamer || Silently tests reader and streamer libs" << std::endl;
    exit(errCode);
}

int main(int argc, char* argv[]){
    const int NUM_LIBS = 3;
    const char* LIBS[] = {"reader", "decoder", "streamer"};
    const uint8_t LIB_FLAGS[] = {0x01, 0x02, 0x04};

    uint8_t libflags = 0x00;
    uint8_t optflags = 0x00;

    // Parse Args
    for (int argidx = 1; argidx < argc; ++argidx){ // loop from first argument
        if (strcmp(argv[argidx], "--help") == 0){
            printHelp(0);

        }else if (strcmp(argv[argidx], "--silent") == 0){
            optflags |= SILENT; 

        }else if(strcmp(argv[argidx], "--lib") == 0){
            if (argc - argidx < 2){
                printHelp();
            }
            for (int i = argidx + 1; i < argc; ++i){
                for (int j = 0; j < NUM_LIBS; ++j){
                    if (strcmp(argv[i], LIBS[j]) == 0){
                        libflags |= LIB_FLAGS[j]; // Update libflags to show lib.
                    }
                }
                
            }
            if (libflags == 0x00){
                printHelp();
            }
            break; // escape the argparse
        }else {
            printHelp();
        }
    }

    // Begin Tests
    if (libflags != 0x00){ // testing if there are lib libflags
        for (int i = 0; i < NUM_LIBS; ++i){
            if (CHECK_FLAG(libflags, LIB_FLAGS[i])){
                switch(i){
                    case 0:
                        test_reader(CHECK_FLAG(optflags, SILENT));
                        break;
                    case 1:
                        test_decoder(CHECK_FLAG(optflags, SILENT));
                        break;
                    case 2:
                        test_streamer(CHECK_FLAG(optflags, SILENT));
                        break;
                };
            }
        }
        return 0;
    }

    test_reader(CHECK_FLAG(optflags, SILENT));
    test_decoder(CHECK_FLAG(optflags, SILENT));
    test_streamer(CHECK_FLAG(optflags, SILENT));
 
    return 0;
}
