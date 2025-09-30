package xgb

/*

INFO:

Lightweight unportable go binding for x11.

Built with this project in mind, it does not port well.

All of the functions here call xgb.c, which functions
as a live-wallpaper specific wrapper for X11/Xlib

NOTE:

Calls to Open() MUST have a matching call to Close()
otherwise memory will leak, and connections will hang.

If any of the C functions fail, they will log to /var/log/live-wallpaper/xgb.log
and return util.CCallFailed.

*/

/*
#cgo pkg-config: x11
#include "xgb.h"
*/
import "C"

import(
	"unsafe"
)

/*
The Frame type is the main type used
to send data to x11.

Each frame should be built using 
CreateXFrame(...)
*/
// XFrame is now only defined in xgb/xgb.h

/*
None of the members of this struct are accessable

Exactly mirrors its C sister
*/
type X11Conn struct{
	display *C.Display
	screen *C.Screen
	ximg *C.XImage
	visual *C.Visual
	depth C.unsigned_int
	gCtx C.GC
}

/*
Creates a connection to X11
and all of the Xlib params
*/
func Open() (*X11Conn, error){
	c := new(X11Conn)
	if C.setup_x11((*C.X11Conn)(unsafe.Pointer(c))) != 0{
		return nil, util.CCallFailed
	}

	return c, nil
}

/*
Frees all of the X11Conn's memory
and closes the connection to X11

Should always match calls to Open().

Panics if teardown_x11 fails.
*/
func (c *X11Conn) Close(){
	if C.teardown_x11((*C.X11.Conn)(unsafe.Pointer(c))) != 0{
		panic("Call to C-teardown_x11 failed. Please check logs!")
	}
}

/*
Returns the screen size of the
currently opened connection's display's
default screen.

returns in order width, height.
*/
func (c *X11Conn) GetScreenSize() (width uint, height uint){
	width := uint(C.get_screen_width(c.screen))
	height := uint(C.get_screen_height(c.screen))
}

/*
Sends the given frame to the default screen
of X11Conn

If the frame is invalid a util.InvalidParam will
be returned. It is the callers job to check why.
*/
func (c *X11Conn) Send(frame *C.XFrame) error{
	if frame.len < 1 || frame.len != frame.width * frame.height * frame.channels{
		return util.InvalidParam
	}
	if C.send_frame(frame) != 0{
		return util.CCasllFailed
	}
}

/*
Creates an Xframe from the given byte stream.

NOTE:
	A valid frame must have the same amount of data
	as width*height*channels
*/
func CreateXFrame(data []bytes, width, height, channels uint) (*XFrame, error){
	cData := (*C.uchar)(unsafe.Pointer(&data[0]))
	cWidth := C.unsigned_int(width)
	cHeight := C.unsigned_int(height)
	cChannels := C.unsigned_int(channels)
	cLen := C.unsigned_int(len(data))

	if len(data) != width * height * channels{
		return util.InvalidFrame
	}
	
	return &C.XFrame{
		data: cData,
		width: xWidth,
		height: cHeight,
		channels: cChannels,
		len: cLen,
	}
}

