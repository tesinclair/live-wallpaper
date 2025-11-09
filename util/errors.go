package util

import (
	"errors"
)

var (
	NotImplemented = errors.New("I ain't done that yet!")
	InvalidParam   = errors.New("I don't even know what that means!")
)
