package services

import (
	ffmpeg "github.com/u2takey/ffmpeg-go"
)

type Decoder struct {
	file         string
	server       string
	screenHeight uint
	screenWidth  uint
	// TODO(tesinclair): add filter options more than just crop
}

/*
* @param fname: must be the absolute path to the input file
 */
func CreateDecoder(fname string, addr string) *Decoder {
	return &Decoder{
		file:   fname,
		server: addr,
	}
}

func (d *Decoder) SetScreenDimensions(w uint8, h uint8) {
	d.screenHeight = h
	d.screenWidth = w
}

func (d *Decoder) Decode() {
	err := ffmpeg.Input(d.file, ffmpeg.KwArgs{"re": ""}).
		Filter("crop", ffmpeg.Args{
			fmt.Sprintf("%v:%v", d.screenWidth, d.screenHeight),
		}).
		Output(d.server, ffmpeg.KwArgs{
			"c:v": "libx264",
			"b:v": "2000k",
			"f":   "mpegts",
		}).
		Run()
	if err != nil {
		panic(err)
	}
}
