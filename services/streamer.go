package services 

import (
	"net"
	"bufio"
	"github.com/tesinclair/live-wallpaper/util"
	x11 "github.com/tesinclair/live-wallpaper/xgb"
	"github.org/x/sync/errgroup"

)

const (
	TCP uint8 = iota
	UDP
)

var validParams = []uint8{TCP, UDP}

type Server interface{
	Serve(chan error, chan bool)
}

type TCPServer struct{
	prot string
	port uint
	host string
	addr string
}

type UDPServer struct{
	port uint
	host string
}

func CreateServer(h, a string, p uint, prot uint8) (*Server, error){
	if !Contains(validParams, prot){
		return nil, util.InvalidParam
	}
	if prot == TCP{
		return &TCPServer{
			prot: "tcp4",
			port: p,
			host: h,
			addr: a
		}, nil
	}else if prot == UDP{
		return &UDPServer{
			port: p,
			host: h
		}, nil
	}

	return nil, util.NotImplemented
}


/*
	Never send data to ch. Ch is meant to recieve logs
	about the state of handled clients.

	Send true to kill to close all children, and return,
	will return with error util.ServerKilled

*/
func (s *TCPServer) Serve(errCh chan SState, killed chan bool){
	// Establish connection to x11
	ln, err := net.Listen(s.prot, s.addr)
	if err != nil{
		errCh<- err
	}
	defer ln.Close()

	errCh<- nil
	close(errCh)
	
	g := new(errgroup.Group)
	g.SetLimit(1)
	alive = make(chan int) // closed when dead
	for {
		select{
		case <-killed == true:
			close(alive)
			g.Wait() 
			close(ch)
			return util.ServerKilled
		default:
		}

		conn, err := ln.Accept()
		if err != nil{
			return err
		}

		if g.TryGo(func() error{
			handleClient(conn, alive) 
		})
	}
}

func (s *UDPServer) Serve(errCh chan error, killed chan bool){
	errCh<- util.NotImplemented
	close(ch)
}

func handleClient(c net.Conn, alive chan bool){
	xConn, err := x11.Open()
	if err != nil{
		c.Close()
		return
	}
	defer xConn.Close()
	defer c.Close()

	screenWidth, screenHeight := c.GetScreenSize()

	if _, err := c.Write([]byte(fmt.Sprintf("%v:%v\0", screenWidth, screenHeight))); err != nil{
		return
	}
	r := bufio.NewReader(c)
	widthC, heightC := strings.Split(string(r.ReadBytes('0xD8')), ":"))
	width, err1 := uint(strconv.ParseInt(widthC))
	height, err2 := uint(strconv.ParseInt(heightC))
	if err1 != nil || err2 != nil{
		return
	}

	frame := 0
	for{
		frame %= 30 // check every 30 frames
		if frame == 0{
			select{
			case _, ok := <- alive; !ok:
				return
			default:
			}
		}
		// TODO: check when the stream ends
		// cos this shouldn't be every frame
		r.ReadBytes('0xD8')
		frame := r.ReadBytes('0xFF')
		
		// for now always do 3 channels
		xFrame, err := x11.CreateXFrame(frame, width, height, 3)
		if err != nil{
			return
		}
		if err := conn.Send(xFrame); err != nil{
			return
		}
		frame++
	}
}
