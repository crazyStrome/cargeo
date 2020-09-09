package main

import (
	"fmt"
	"math"

	"github.com/gin-gonic/gin"
)

func main() {
	// fmt.Println("hhhh")
	// fmt.Println(gps84ToGcj02(38.88486388888889, 121.52287222222222))
	router := gin.Default()
	router.GET("/", func(c *gin.Context) {
		var lat = c.Query("lat")
		var lon = c.Query("lon")
		fmt.Println(lat, lon)
	})
	router.Run(":8080")
}

// Gps 保存地点的经纬度
type Gps struct {
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
