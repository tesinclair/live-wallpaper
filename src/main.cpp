/**
    @Notes:
        - excessive logging: Most of the logging will be removed when the first release is ready. This is to avoid bugs.

        - Test file: should be in the root project directory, used for debugging and the automatic file choice when the DEBUG macro is defined 

        - Logging: disabled by default in release mode: See lib/shared/lw_logging.hpp for details. 
                   Verbose can be defined with the verbose flag during building to trigger logging during release. 
                   Silent can be defined with the silent flag during building to turn off logging during debug.
 */


/**
    TODO: 
        - Ensure that if the program is rerun the previous background streamer is killed ✅
        - Add arg parsing for non debug mode
 */

#include <iostream>
#include <cstdio>
#include <filesystem>
#include <atomic>
#include <chrono>

#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

#include "lib/backgroundstreamer.hpp"
#include "lib/reader.hpp"
#include "lib/decoder.hpp"

#include "lib/shared/lw_logging.hpp"
#include "lib/shared/lw_types.hpp"
#include "lib/shared/lw_errors.hpp"

#define LOCK_NAME "/lw_instance_lock"
#define SHM_NAME "/lw_sh"
#define SEM_TIMEOUT 10

#ifdef DEBUG
#define MAX_STREAM_ITER 5 // defines the number of times to stream the background in debug mode
#endif


static sem_t* instanceLock;
static std::atomic<int>* shouldShutdown;
static bool destroy = false;

/// @Does displays usage message
void usage(){
    printf("Usage:\nlive-wallpaper [filename]\nlive-wallpaper --help (-h)\n\nIt isn't hard.\n");  
}

void test(const err& err){
    LOG_INFO("Main:test", "Testing <%s>", err->errContext);
    if (err->err){
        LOG_ERROR(err->errContext, err->errno, err->errMsg);
        printf("An error occurred in %s. <%ld>: %s", err->errContext, err->errno, err->errMsg);
        exit(err->errno);
    }
    LOG_INFO("Main:test", "Finished testing <%s> with no errors", err->errContext);
}

err shutdown(){
    LOG_WARN("Main:shutdown", "Shutdown triggered. Cleaning up...");
    LOG_WARN("Main:shutdown", "Leaving shared memory...");
    munmap(shouldShutdown, sizeof(std::atomic<int>));
    if (destroy){
        shm_unlink(SHM_NAME);
    }
    LOG_WARN("Main:shutdown", "Releasing semaphore...");
    sem_post(instanceLock);
    sem_close(instanceLock);
    if (destroy){
        sem_unlink(LOCK_NAME);
    }
    LOG_INFO("Main:shutdown", "Clean up completed");
    LOG_INFO("Main:shutdown", "Exiting...");
    exit(0);
}

static void startup_loadMMap(err* startupError, int value){
    LOG_INFO("Main:startup_loadMMap", "Starting to load shared memory");

    LOG_INFO("Main:startup_loadMMap", "Opening shared memory...");
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1){
        *startupError = {"Main:startup_loadMMap", true, errno, "Failed to open shared memory"};
        return;
    }
    LOG_INFO("Main:startup_loadMMap", "Successfully opened shared memory");

    LOG_INFO("Main:startup_loadMMap", "Truncating shared memory...");
    if (ftruncate(shm_fd, sizeof(std::atomic<int>)) != 0){
        *startupError = {"Main:startup_loadMMap", true, errno, "Failed to truncate shared memory"};
        return;
    }
    LOG_INFO("Main:startup_loadMMap", "Successfully truncated shared memory");

    LOG_INFO("Main:startup_loadMMap", "Creating memory map...");
    void* ptr = mmap(NULL, sizeof(std::atomic<int>), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED){
        *startupError = {"Main:startup_loadMMap", true, errno, "Failed to create the memory map"};
        return;
    }
    LOG_INFO("Main:startup_loadMMap", "Successfully created memory map");
    
    LOG_INFO("Main:startup_loadMMap", "Assigning memory map to shouldShutdown...");
    shouldShutdown = new(ptr) std::atomic<int>(value);
    if (shouldShutdown == nullptr){
        *startupError = {"Main:startup_loadMMap", true, BAD_POINTER, "Failed to assign memory map to shouldShutdown"};
        return;
    }
    LOG_INFO("Main:startup_loadMMap", "Successfully assigned memory map to shouldShutdown.");

    LOG_INFO("Main:startup_loadMMap", "Closing shared memory file descriptor...");
    if (close(shm_fd) != 0){
        *startupError = {"Main:startup_loadMMap", true, errno, "Failed to close shared memory file descriptor"};
        return;
    }
    LOG_INFO("Main:startup_loadMMap", "Successfully closed shared memory file descriptor");
    LOG_INFO("Main:startup_loadMmap", "Successfully loaded memory mapping. Returning to Main:startup...");
}

err startup(){
    err startupError{"Main:startup", false, NOERR, ""};

    LOG_INFO("Main:startup", "Beginning startup. Attempting to open semaphore...");
    instanceLock = sem_open(LOCK_NAME, O_CREAT, 0644, 1);

    if (instanceLock == SEM_FAILED){
        LOG_WARN("Main:startup", "Failed to create semaphore. Expect an error.");
        startupError = {"Main:startup", true, FAILED_INSTANCE_LOCK, "Failed to create the instance process lock."};
        return startupError;
    }
    LOG_INFO("Main:startup", "Succesfully created or located semaphore.");

    LOG_INFO("Main:startup", "Attempting to acquire semaphore");

    // If there is currently a running instance
    if (sem_trywait(instanceLock) != 0){
        LOG_WARN("Main:startup", "Semaphore could not be acquired. Advising other instance to shutdown...");

        if (errno != EAGAIN){
            startupError = {"Main:startup", true, FAILED_AQUIRE_SEMAPHORE, "Failed to acquire semaphore"};
            return startupError;
        }

        LOG_INFO("Main:startup", "Attempting to open shared memory");
        startup_loadMMap(&startupError, 1);
        if (startupError.err)
            return startupError;
   
        LOG_INFO("Main:startup", "Created/Accessed shared memory. Told other instance to shut down.");

        const timespec ts{std::chrono::system_clock::to_time_t(
                    std::chrono::system_clock::now() + std::chrono::seconds(SEM_TIMEOUT)
                ), 0};
        if (sem_timedwait(instanceLock, &ts) != 0){
            LOG_WARN("Main:startup", "Semaphore aquisition timed out. Possibly the other instance refused to shut down? Expect an error.");
            startupError = {"Main:startup", true, SEMAPHORE_TIMEOUT, "Semaphore aquisition timed out."};
            return startupError;
        }

        LOG_INFO("Main:startup", "Semaphore acquired, other instance shutdown succesfully.");
        LOG_WARN("Main:startup", "Setting shutdown to false.");

        shouldShutdown->store(0);

        LOG_INFO("Main:startup", "Set shutdown to false. Returning...");
        return startupError;
    }
    LOG_INFO("Main:startup", "Semaphore acquired. This is the only instance. Opening shared memory.");
    startup_loadMMap(&startupError, 0);
    if (startupError.err)
        return startupError;
    
    LOG_INFO("Main:startup", "Opened memory and set shutdown to false. Returning...");

    return startupError;
}

// Any argparse issues usage is printed.
void argParse(int argc, char* argv[]){
    if (argc < 2){
        printUsage();
        exit(1);
    }

    if (argv[1] == "--destroy"){
        destroy = true;
    }
}

int main(int argc, char* argv[]){
    argParse(argc, argv);

    err startupError = startup();
    test(startupError);

#ifdef DEBUG
    LOG_INFO("Main:main", "Starting main process in DEBUG mode");
    std::filesystem::path filepath = std::filesystem::path(ROOT) / "live-wallpaper.mp4"; // filepath is the example file (should be placed in proj root)
    LOG_INFO("Main:main", "Generated filepath: %s.", filepath.string());
#elifdef VERBOSE 
    LOG_INFO("Main:main", "Starting main process with VERBOSE logging enabled");
#else
    LOG_INFO("Main:main", "Starting main process..."); // incase logging is turned on perminantly
#endif
#ifndef DEBUG
    printf("Not Implemented\n");
#endif

#ifdef DEBUG // TODO: add argparse to else statement above, so we can remove this DEBUG
    LOG_INFO("Main:main", "Creating Reader object.");
    Reader reader(filepath);
    test(reader.readerError);
    LOG_INFO("Main:main", "Reader object created successfully.");

    LOG_INFO("Main:main", "Creating the Streamer object.");
    Streamer streamer;
    test(streamer.streamerError);
    LOG_INFO("Main:main", "Streamer object created successfully.");

#if defined(DEBUG) || defined(VERBOSE)
    size_t i = 1;
#endif

    LOG_INFO("Main:main", "Entering the main streaming loop.");

    while (true){ // Loop goes forever unless shutdown is called
#ifdef DEBUG
        LOG_INFO("Main:main-loop", "Current Interation: %ld of %d", i, MAX_STREAM_ITER);
#else
        LOG_INFO("Main:main-loop", "Current Interation: %ld.", i);
#endif

#if defined(DEBUG) || defined(VERBOSE)
        size_t j = 1;
#endif

        LOG_INFO("Main:main-loop", "Getting the first packet...");
        AVPacket* ptk = reader.getNextPacket();
        test(reader.readerError);
        LOG_INFO("Main:main-loop", "Got first packet.");

        LOG_INFO("Main:main-loop", "Entering the frame streaming loop.");

        while (!reader.get_isEOF()){
#if defined(DEBUG) || defined(VERBOSE)
            LOG_INFO("Main:main-floop", "Current Iteration: %ld", j);
#endif
            LOG_INFO("Main:main-floop", "Streaming decoded frame to background.");
            streamer.stream(&pkt);
            test(streamer.streamerError);
            LOG_INFO("Main:main-floop", "Successfully streamed the frame.");

            LOG_INFO("Main:main-floop", "Getting the next frame.");
            AVPacket* ptk = reader.getNextPacket();
            test(reader.readerError);
            LOG_INFO("Main:main-floop", "Successfully retrieved the next frame.");

#if defined(DEBUG) || defined(VERBOSE)
            LOG_INFO("Main:main-floop", "Finishing Iteration: %ld", j);
            j++;
#endif
        }
#if defined(DEBUG) || defined(VERBOSE)
        LOG_INFO("Main:main-loop", "Finishing Iteration :%ld", i);
#endif
#ifdef DEBUG
        if (i > MAX_STREAM_ITER){
            LOG_INFO("Main:main-loop", "End of debug session reached. Shutting down...");
            shutdown();
        }
#endif

        LOG_INFO("Main:main-loop", "Finished First stream of background video.");
        LOG_INFO("Main:main-loop", "Testing for shutdown instruction.");

        if (shouldShutdown->load()){ // returns 1 if shutdown is true, else 0
            LOG_INFO("Main:main-loop", "Other instance of live-wallpaper running. Shutting down current.");
            shutdown();
        }

        LOG_INFO("Main:main-loop", "No other instance active. Continuing...");
        LOG_INFO("Main:main-loop", "Resetting to beginning of video.");

        reader.restart();
        test(reader.readerError);

        LOG_INFO("Main:main-loop", "Successfully restarted the background video.");
    }
#endif
}
