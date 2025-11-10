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
	"github.com/tesinclair/live-wallpaper/services"
	x11 "github.com/tesinclair/live-wallpaper/xgb"
	"net"
	"os"
	"os/signal"
	"syscall"
	"time"
)

var (
	Host     string = "127.0.0.1"
	Port     int = 31893
	Protocol string = "tcp"
	Delay    int = 100
	Filename string = ""

	IsDaemon bool = false
)

var (
	tmpPath = os.TempDir() + "/live_wallpaper_tmp"
)

const (
)

func main() {
	sigCh := make(chan os.Signal)
	killedCh := make(chan bool)

	if os.Getend("DAEMON") != ""{
		IsDaemon = true
	}

	// The daemon does not take cla
	if !IsDaemon{
		argparse(os.Args);
	}

	prot, addr := parseConfig()


	// TODO(tesinclair): add this to a log file or journalctl or smth
	fmt.Println("Starting instance of live-wallpaper.")
	fmt.Println("Starting server: ", addr)

	/*
	addr := fmt.Sprintf("%v://%v:%v", Protocol, Host, Port)
	prot := services.TCP
	switch Protocol {
	case "tcp":
		break
	case "udp":
		prot = services.UDP
	}
	*/

	// receiving channels
	start := make(chan bool)
	kill := make(chan bool)
	input := make(chan string)

	// sending channel
	err := make(chan error)

	go listenDaemon(start, kill, input, err)

	kill_server := make(chan bool)

	for{
		select{
		case <-start == true:
			server := services.CreateServer(Host, addr, Port, prot)
			go server.Serve(err, kill_server)

		case <-kill == true:
			kill_server <-true

		case in := <-input:
			/*
			if testServer()
			if input != ""
				create and stream decoder
				
				decoder := services.CreateDecoder(fname, addr)
				// TODO(tesinclair): implement this in xgb
				sWidth, sHeight, err := x11.GetDefaultScreenSize()
				if err != nil {
					log.Fatalln("Failed to get screen size: ", err)
				}
				decoder.SetScreenDimensions(sWidth, sHeight)
				

			else
				send a no server error
			*/
		}
	}
}

func setupDaemon() error {
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

func argparse(args []string){
	if len(args) < 2{
		printHelp()
		panic("Not enough arguments")
	}

	if args[1] == "--start"{
		// TODO(tesinclair): start up the daemon
		os.Exit(0)
	}

	if args[1] == "--restart"{
		// TODO(tesinclair): send a signal to dbus asking the service to die
		// 					 starts up a new daemon.
	}

	if args[1] == "--reload"{
		// TODO(tesinclair): send a signal to dbus asking the service to reload
		os.Exit(0)
	}

	if args[1] == "--kill"{
		// TODO(tesinclair): send signal to dbus telling the service to die
		os.Exit(0)
	}

	if args[1] == "--config-gen"{
		// TODO(tesinclair): generate a config file in ~/.config/live-wallpaper/config.ini
		
		/*
		"The hostname to run the streamer on. Not validated."
		Host = "127.0.0.1"

		"A port for the streamer to use."
		Port = 31893

		"The protocol to run the streamer over."
		Protocol = "tcp"

	 	"The delay between probes."
		Delay = 100
		*/
		os.Exit(0)
	}

	if args[1] == "--input"{
		if len(args) < 3{
			printHelp()
			panic("Not enough arguments.")
		}
		//TODO(tesinclair): send a message to dbus {"open": args[2]}
	}
	
}

func printHelp(){
	fmt.Printf(`
	Usage: %v COMMAND

	Commands:
		--start [OPTIONS]: Starts the live-wallpaper daemon; nothing if already alive.
		--reload [OPTIONS]: reloads the configuration file.
		--restart [OPTIONS]: Kills the living daemon and starts another.
		--kill: Kills the living daemon; nothing if no daemon is alive.
		--input filename: Sends the given file to the daemon to display.

		--config-gen: generate a blank config file in ~/.config/live-wallpaper/config.ini

	Options:
		--config: The location of the config file [NOT IMPLEMENTED YET]

	Config:
		The config file is optional as the default values work well for any normal person.
		The daemon searches ~/.config/live-wallpaper/config.ini.
		The config file can be automatically generated with the --config-gen flag.

	`, os.Args[0])
	os.Exit(0)
}
