
<<==========================>>
Info:

This is the design for a background streamer, that takes video data from an mp4
(No more than 20-30 seconds) and streams it to the device background


Conventions:
  - On return of "err", an err struct should be returned. Defined by state: success/failure, errMsg: string/null

<<===========================>>

Top level:

1. mp4 Reader

2. mp4 Decoder

3. Background Streamer

4. main


## Reader

### Public:

**Functions**:

*constructor*:
- Takes: Filename
- Does: opens file, sets variables
- Returns: object 

*destructor*:
- If the fd is still active, remove it.
- Useful in case read doesn't get called for whatever reason

*read*:
- takes: none
- returns: video track: struct { sps: bytes, pps: bytes, frames: \[ {offset, size, timestamp} \] }

### Private:

**Functions**:

*close*:
- Safely closes the fd
- Should be called from read

*parseVideoBoxes*:
- enumerates all video boxes

**Variables**:

- Filename
- fileD - Binary fileD object (ifstream)

## Decoder 

### Public:

**Functions**:

*constructor*:
- Takes: bytestream
- Returns: object 

*decodeFrame*:
- takes: none
- returns: frameData

### Private:

**Functions**:

*updateFramePointer*:
- Takes: index of next frame
- Returns: frameCursor

**Variables**:

- bytestream - stream of byte data from file
- frameCursor- current frame location

## BackgroundStreamer

### Public:

**Functions**:

*constructor*:
- Takes: decoder instance
- Returns: object 

*stream*:
- takes: none
- Does: gets the next frame data and renders it to the background until getNextFrame returns null
- returns: err

### Private:

**Functions**:

*getNextFrame*:
- Takes: None
- Does: fetches the next decoded frame from the decoder
- Returns: next frame data / null if no frame returned

*renderFrame*:
- Takes: Framedata
- Returns: err

**Variables**:

- decoder - the current decoder instance

