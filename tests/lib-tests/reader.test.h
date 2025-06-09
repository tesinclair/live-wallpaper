#pragma once

#include <iostream>

#include "utils/mock.h"
#include "reader.h"

void testReader(int silent);

// Ensures the reader is created and initialised properly
void testConstruct(char* testfile);

void testGetVideoData(char* testfile, videoTrack testTrack);
void testGetCurrentFrame(char* testfile, char* testFrame);

// Ensures the file is closed on destruction
void testDestruct(char* testfile);
