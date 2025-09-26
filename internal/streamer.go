package services 

import (
	"net"
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
			handleClient(conn, alive, xConn) 
		})
	}
}

func (s *UDPServer) Serve(errCh chan error, killed chan bool){
	errCh<- util.NotImplemented
	close(ch)
}

func handleClient(c net.Conn, alive chan bool, x x11.Conn){
	for{
		select{
		case _, ok := <- alive; !ok:
			c.Close()
			return
		default:
		}
	}
}



