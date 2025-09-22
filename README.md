# Live Wallpaper

## Introduction

A really simple and to the point live wallpaper background streamer.

Written in Go, this uses x11 and ffmpeg bindings to decode an mp4 file and
stream it to the root window.

Right now, because GNOME sucks, and I don't want to deal with it, this is only for i3
but if this gets enough stars I will find some workaround to get it to work with nautilus.

#### Why?

I wrote this because there is literally nothing else... I don't know whether
ubuntu users just hate fun, but the best alternative hasn't had a patch in 7 years...
and even the alternatives are built on it (<-TODO)

## Installation

Probably going to add to snap, but until then just clone the repo:

`git clone --depth=1 https://github.com/tesinclair/live-wallpaper`

Grab the modules and compile:

```bash
go get (TODO)
go build
```

## Usage

Right now there aren't many features... I have just made it to do what I want it to do:

`live-wallpaper filename &`

### Technical (probably should at least skim)

A detailed technical description can be found in doc/tech.md, and a flowchart can be found
in doc/flow.png.

But here is a brief overview, and all you really need to know.

The live-wallpaper command starts an instance of live wallpaper. This:
1. Tries to create a temp buffer in (TODO)
2. If the buffer fails, then an instance is active, and that instance is safely ended and closed (the buffer is then retried)
3. Starts a streamer goroutine which waits for frames
4. Decodes a block of frames
5. Sends the frames to the streamer
6. Streams the frames and cleans
7. Repeat from 4.

# Future

I actually do have a lot of features planned for example:

- Add ability to customise background fit (stretch, fit, fill, (not tile, because who tf uses tile))
- Allow user to specify display - this will make it work on multiple monitors
- Allow fps customisation - defaults to 30fps right now.
