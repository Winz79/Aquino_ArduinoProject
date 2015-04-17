-- Tel--l the chip to connect to the access point
--wifi.setmode(wifi.STATIONAP)
wifi.setmode(wifi.STATION)
print('set (mode='..wifi.getmode()..')')
print('MAC: ',wifi.sta.getmac())
print('chip: ',node.chipid())
print('heap: ',node.heap())


local cfg={}
cfg.ssid="ESP-"..node.chipid()
cfg.pwd="ESP-"..node.chipid()
wifi.ap.config(cfg)
cfg = nil

-- Compile server code and remove original .lua files.
-- This only happens the first time afer the .lua files are uploaded.

local compileAndRemoveIfNeeded = function(f)
   if file.open(f) then
      file.close()
      print(f)
      node.compile(f)
      file.remove(f)
   end
end

local serverFiles = {'httpserver.lua', 'httpserver-request.lua', 'httpserver-static.lua', 'httpserver-error.lua'}
for i, f in ipairs(serverFiles) do compileAndRemoveIfNeeded(f) end

compileAndRemoveIfNeeded = nil
serverFiles = nil

-- Connect to the WiFi access point. Once the device is connected,
-- you may start the HTTP server.

local joincounter = 0

tmr.alarm(0, 3000, 1, function()

   if wifi.sta.getip() == nil and joincounter < 5 then
      wifi.sta.config("Livebox-F444","EUREKA79")
      wifi.sta.connect()
      print("Connecting to AP...")
      joincounter = joincounter +1
   else 
      tmr.stop(0)
      -- print('IP: ',wifi.sta.getip())
      -- Uncomment to automatically start the server in port 80
      joincounter = nil
      collectgarbage()
      dofile("httpserver.lc")(80)
   end

end)

function sendTemperature( lasttemp, levelStatus, relayStatus )
    t1 = lasttemp / 100
    t2 = (lasttemp >= 0 and lasttemp % 100) or (100 - lasttemp % 100)
    print("Temp:"..t1 .. "."..string.format("%02d", t2).." C\n")
    -- conection to thingspeak.com
    print("Sending data to thingspeak.com")
    conn=net.createConnection(net.TCP, 0) 
    --conn:on("receive", function(conn, payload) print(payload) end)
    -- api.thingspeak.com 184.106.153.149
    conn:connect(80,'184.106.153.149') 
    conn:send("GET /update?key=PGL2SMS81JUSLF98 &field1="..t1.."."..string.format("%02d", t2).."&field2="..levelStatus.."&field3="..relayStatus.." HTTP/1.1\r\n") 
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
