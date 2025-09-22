package main

import (
	"fmt"
	"os"
	"errors"
	"github.com/akamensky/argparse"
	service "github.com/tesinclair/live-wallpaper/internal"
)

func main(){
	argparser := argparse.NewParser("", "A modern and maintained live wallpaper for ubuntu-i3.")
	var filename *string = argparser.FilePositional(nil)

	if err := autoClose(); err != nil{
		log.Fatalf("There was an error automatically closing other instance: %v", err)
	}
	
	in := make(chan *
}

// Automatically closes any open instance
// will take a display once display choosing is 
// implemented
func autoClose() error{
}
