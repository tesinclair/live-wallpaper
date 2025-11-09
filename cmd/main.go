/*

Flow:
Main grabs the stream using ffmpeg and directs to
a tcp server running on port 31922 (because Saleem said so).
The server sends each frame in the stream to the X11 root window.

*/

package main

import (
	"errors"
	"fmt"
	"github.com/akamensky/argparse"
	"github.com/tesinclair/live-wallpaper/services"
	x11 "github.com/tesinclair/live-wallpaper/xgb"
	"net"
	"os"
	"os/signal"
	"syscall"
	"time"
)

var (
	Host     *string
	Port     *int
	Protocol *string
	Delay    *int
)

var (
	tmpPath = os.TempDir() + "/live_wallpaper_tmp"
)

/*
TODO:
	- Add flags:
		- Port, Host, protocol, Delay
		- BG fit options
		- FPS
*/

type prog struct {
	mu   sync.Mutex
	kill bool
}

func main() {
	p := new(prog)
	sigCh := make(chan os.Signal)
	killedCh := make(chan bool)
	signal.Notify(sigCh, syscall.SIGINT, syscall.SIGTERM)

	go p.cleanupWatcher(sigCh, killedCh)

	argparser := argparse.NewParser("", "A modern and maintained live wallpaper for ubuntu-i3.")
	fname := argparse.File("i", "input", os.O_RDONLY, 0, &argparse.Options{
		Required: true,
		Help:     "The mp4 input file to stream.",
	})
	Host = argparser.String("", "host", &argparse.Options{
		Help:    "The hostname to run the streamer on. Not validated.",
		Default: "127.0.0.1",
	})
	Port = argparser.Int("", "port", &argparse.Options{
		Help:    "A port for the streamer to use.",
		Default: 31893,
	})
	Protocol = argparser.Selector("", "protocol", []string{"tcp", "udp"}, &argparse.Options{
		Help:    "The protocol to run the streamer over.",
		Default: "tcp",
	})
	Delay = argparser.Int("d", "delay", &argparse.Options{
		Help:    "The delay between probes.",
		Default: 100,
	})
	if len(os.Args) < 2 {
		fmt.Print(argparser.Usage(nil))
		os.Exit(1)
	}
	if err := setAlive(); err != nil {
		panic("Failed to wake up.")
	}
	if os.Args[1] == "--kill" {
		os.Exit(0) // setAlive() will kill all other instances
	}
	if err := argparser.Parse(os.Args); err != nil {
		fmt.Print(argparser.Usage(err))
		os.Exit(1)
	}

	addr := fmt.Sprintf("%v://%v:%v", *Protocol, *Host, *Port)

	fmt.Println("Starting instance of live-wallpaper.")
	fmt.Println("Starting server: ", addr)

	prot := services.TCP
	switch Protocol {
	case "tcp":
		break
	case "udp":
		prot = services.UDP
	}
	server := services.CreateServer(*Host, addr, *Port, prot)

	err := make(chan error)
	kill := make(chan bool)
	go server.Serve(err, kill)

	if <-err != nil {
		log.Fatalln("Failed to prepare server: ", ready.err)
	}
	fmt.Println("Server started successfully.")

	decoder := services.CreateDecoder(fname, addr)
	// TODO(tesinclair): implement this in xgb
	sWidth, sHeight, err := x11.GetDefaultScreenSize()
	if err != nil {
		log.Fatalln("Failed to get screen size: ", err)
	}
	decoder.SetScreenDimensions(sWidth, sHeight)

	for {
		// TODO(tesinclair): This needs to be reconsidered...
		if p.shouldClose {
			kill <- true // send to server to kill client
			for {
				ln, err := net.Listen(server.prot, addr)
				if err != nil {
					time.Sleep(*Delay)
					continue
				}
				ln.Close()
				killedCh <- true
				goto ret
			}
		}
		time.Sleep(*Delay)
	}
ret:
	fmt.Println("Server shut down successfully. Exiting.")
	return 0
}

func setAlive() error {
	for {
		if _, err := os.Stat(tmpPath); errors.Is(err, os.ErrNotExist) {
			f, err := os.OpenFile(tmpPath, os.O_CREATE|os.O_RDWR, 0600)
			if err != nil {
				return fmt.Errorf("Failed to set alive: %w", err)
			}
			fmt.Fprintf(f, "%d", os.GetPid())
			f.Close()
			return
		}

		pPidBytes, err := os.ReadFile(tmpPath)
		if err != nil {
			return fmt.Errorf("Failed to read living processes pid: %w", err)
		}
		pPid, err := strconv.Atoi(strings.TrimSpace(pPidBytes))
		if err != nil {
			return fmt.Errorf("Failed to convert living processes pid: %q, to an int: %w", pPidBytes, err)
		}

		p, err := os.FindProcess(pPid)
		if err != nil {
			return fmt.Errorf("Failed to get the previous instances pid: %w", err)
		}
		if err := p.Signal(syscall.Signal(0)); err != nil {
			return fmt.Errorf("Failed to probe the living instances pid %q: %w", pPid, err)
		}

		if err := p.Signal(syscall.SIGTERM); err != nil {
			return fmt.Errorf("Failed to terminate process with pid %q: %w. May need to be done manually", pPid, err)
		}

		for {
			if err != p.Signal(syscall.Signal(0)); err != nil { // error means file has been deleted
				break
			}
			time.Sleep(*Delay)
		}
	}
}

func (p *prog) cleanupWatcher(sig chan os.signal, ready chan bool) {
	<-sig
	fmt.Println("Received kill command. Shutting down...")
	p.mu.Lock()
	p.kill = true
	p.mu.Unlock()
	for {
		if <-ready == true {
			os.Remove(tmpPath)
			break
		}
	}
}
