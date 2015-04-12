function sendTemperature( lasttemp, levelStatus, relayStatus )
    t1 = lasttemp / 10000
    t2 = (lasttemp >= 0 and lasttemp % 10000) or (10000 - lasttemp % 10000)
    print("Temp:"..t1 .. "."..string.format("%04d", t2).." C\n")
    -- conection to thingspeak.com
    print("Sending data to thingspeak.com")
    conn=net.createConnection(net.TCP, 0) 
    conn:on("receive", function(conn, payload) print(payload) end)
    -- api.thingspeak.com 184.106.153.149
    conn:connect(80,'184.106.153.149') 
    conn:send("GET /update?key=PGL2SMS81JUSLF98 &field1="..t1.."."..string.format("%04d", t2).."&field2="..levelStatus.."&field3="..relayStatus.." HTTP/1.1\r\n") 
    conn:send("Host: api.thingspeak.com\r\n") 
    conn:send("Accept: */*\r\n") 
    conn:send("User-Agent: Mozilla/4.0 (compatible; esp8266 Lua; Windows NT 5.1)\r\n")
    conn:send("\r\n")
    conn:on("sent",function(conn)
      print("Closing connection")
      conn:close()
    end)
    conn:on("receive",function(conn,data)
      print("receive:"..data)
      conn:close()
    end)
    
    conn:on("disconnection", function(conn)
        print("Got disconnection...")
    end)
end


function startServer()
    print("Create Server")
    if srv then srv:close() end
    srv=net.createServer(net.TCP)
    srv:listen(80,function(conn)
        conn:on("receive",function(conn,payload)
                print(payload)
                conn:send("<h1> Hello, NodeMcu.</h1>")
                conn:close();
            end)
    end)
    print("Server Created")
end


print("Setting up WIFI...")
wifi.setmode(wifi.STATION)
wifi.sta.config("Livebox-F444","EUREKA79")
wifi.sta.connect()

tmr.alarm(1, 10000, 1, function() 
	if wifi.sta.getip()== nil then 
		print("IP unavaiable, Waiting...") 
	else 
		tmr.stop(1)
		print("Config done, IP is "..wifi.sta.getip())
      print("MAC : "..wifi.sta.getmac())
--      print("IP is "..wifi.ap.getip())
      
      startServer()

      sendTemperature(234500,-1,-1)
	end 
end)

