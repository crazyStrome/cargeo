package main

import (
	"fmt"
	"math"
	"net"
	"os"
	"strconv"
	"strings"

	log "github.com/sirupsen/logrus"
)

func init() {
	// 设置日志格式为json格式
	log.SetFormatter(&log.JSONFormatter{})

	// 设置将日志输出到标准输出（默认的输出为stderr，标准错误）
	// 日志消息输出可以是任意的io.writer类型
	log.SetOutput(os.Stdout)

	// 设置日志级别为warn以上
	log.SetLevel(log.InfoLevel)
}
func main() {
	server, err := net.Listen("tcp", ":8080")
	if err != nil {
		log.Error("open socket error: ", err)
		return
	}
	log.Info("server is listening...")

	for {
		conn, err := server.Accept()

		if err != nil {
			fmt.Println("connection error: ", err)
			continue
		}
		log.Info("get an connection")
		go func(c net.Conn) {
			if c == nil {
				log.Error("unused socket conn")
				return
			}
			defer c.Close()
			buf := make([]byte, 1024)
			log.Info(conn.RemoteAddr().String(), " connect to server...")
			for {
				cnt, err := c.Read(buf)
				if cnt == 0 || err != nil {
					log.Warn("connection closed or something error: ", err)
					// c.Close()
					break
				}
				var content = string(buf[:cnt])
				if len(content) > 50 || content[:3] != "Car" {
					log.Warn("wrong request and data: ", content, " from ", conn.RemoteAddr().String())
					return
				}
				var gps = parseGps(content)
				if gps.Car != "" {
					log.Info("get gps info: ", gps)
				}
				// log.Debug("received: ", content)
			}
		}(conn)
	}
}
func parseGps(data string) (gps Gps) {
	var cs = strings.Split(data, ",")
	if len(cs) == 0 {
		log.Warn("no useful data from device")
		return
	}
	var lat, err = strconv.ParseFloat(strings.Split(cs[1], ":")[1], 64)
	if err != nil {
		log.Warn("lat format error: ", lat, err)
		return
	}
	lon, err := strconv.ParseFloat(strings.Split(cs[2], ":")[1], 64)
	if err != nil {
		log.Warn("lon format error: ", lon, err)
		return
	}
	return Gps{
		Car: strings.Split(cs[0], ":")[1],
		Lat: lat,
		Lon: lon,
	}
}

// Gps 保存地点的经纬度
type Gps struct {
	Car string
	Lat float64 // 纬度
	Lon float64 // 经度
}

func gps84ToGcj02(lat, lon float64) Gps {
	var a = 6378245.0
	var ee = 0.00669342162296594323
	var dLat = transformLat(lon-105.0, lat-35.0)
	var dLon = transformLon(lon-105.0, lat-35.0)
	var radLat = lat / 180.0 * math.Pi
	var magic = math.Sin(radLat)
	magic = 1 - ee*magic*magic
	var sqrtMagic = math.Sqrt(magic)
	dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * math.Pi)
	dLon = (dLon * 180.0) / (a / sqrtMagic * math.Cos(radLat) * math.Pi)
	var mgLat = lat + dLat
	var mgLon = lon + dLon
	return Gps{
		Lat: mgLat,
		Lon: mgLon,
	}
}
func transformLat(x, y float64) float64 {
	var ret = -100.0 + 2.0*x + 3.0*y + 0.2*y*y + 0.1*x*y + 0.2*math.Sqrt(math.Abs(x))
	ret += (20.0*math.Sin(6.0*x*math.Pi) + 20.0*math.Sin(2.0*x*math.Pi)) * 2.0 / 3.0
	ret += (20.0*math.Sin(y*math.Pi) + 40.0*math.Sin(y/3.0*math.Pi)) * 2.0 / 3.0
	ret += (160.0*math.Sin(y/12.0*math.Pi) + 320*math.Sin(y*math.Pi/30.0)) * 2.0 / 3.0
	return ret
}
func transformLon(x, y float64) float64 {
	var ret = 300.0 + x + 2.0*y + 0.1*x*x + 0.1*x*y + 0.1*math.Sqrt(math.Abs(x))
	ret += (20.0*math.Sin(6.0*x*math.Pi) + 20.0*math.Sin(2.0*x*math.Pi)) * 2.0 / 3.0
	ret += (20.0*math.Sin(x*math.Pi) + 40.0*math.Sin(x/3.0*math.Pi)) * 2.0 / 3.0
	ret += (150.0*math.Sin(x/12.0*math.Pi) + 300.0*math.Sin(x/30.0*math.Pi)) * 2.0 / 3.0
	return ret
}
