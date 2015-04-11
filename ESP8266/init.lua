wifi.setmode(wifi.STATION)
print("Setting up WIFI...")
wifi.setmode(wifi.STATION)
--modify according your wireless router settings
wifi.sta.config("Livebox-F444","EUREKA79")
wifi.sta.connect()
tmr.alarm(1, 1000, 1, function() 
	if wifi.sta.getip()== nil then 
		print("IP unavaiable, Waiting...") 
	else 
		tmr.stop(1)
		print("Config done, IP is "..wifi.sta.getip())
		dofile("ds1820.lua")
	end 
end)