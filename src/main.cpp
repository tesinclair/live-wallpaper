#include <iostream>
#include "lib/backgroundstreamer.hpp"
#include "lib/reader.hpp"
#include "lib/decoder.hpp"

/// @Does displays usage message
void usage(){
    std::cout << "Usage:\nlive-wallpaper [filename]\nlive-wallpaper --help (-h)\n\nIt isn't hard." << std::endl;  
}

void checkReader(Reader& reader){
    if (reader.readError.err){
        std::cerr << reader.readError.errMsg << std::endl;
        exit(1);
    }
}

int main(int argc, char* argv[]){
    if (argc != 2){
        usage();
        return 1;
    }else if (argc == 2 && (argv[1] == "-h" || argv[1] == "--help")){
        usage();
        return 0;
    }

    Reader reader(argv[1]);
    checkReader(reader);

    videoTrack vTrack;

    reader.getVideoData(vTrack);
    checkReader(reader);

    std::cout << "\nVideo Data:\n";
    std::cout << "\tHeight: " << vTrack.height << std::endl;
    std::cout << "\tWidth: " << vTrack.width << std::endl;
    std::cout << "\tTimescale: " << vTrack.timescale << std::endl;
    std::cout << "\tDuration: " << vTrack.duration << std::endl;
    std::cout << "\tDuration (s): " << vTrack.durationSeconds << std::endl;

    //frame cframe;

    //while (!reader.get_isEOF()){
    //    cframe = reader.getCurrentVideoTrack();
    //    checkReader(&reader);
    //}

    checkReader(reader);
    
    return 0;
}
