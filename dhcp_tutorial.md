Raspberry Pi DCHP Tutorial

Note: This is for Windows, but this is dealing mostly with OSI Level 3 Networking standards, so it won't be drastically different on your AppleBook Touch 2018 or whatever. 
			This tutorial is written as if the reader has no idea what almost anything does (because that was me), so apologies in advance if you find it too verbose. 

Requirements:
	Raspberry Pi's all with OSes installed
	Ability to connect ethernet cable to computer (port or adapter)
	Number of ethernet cables equal to number of Pi's, plus one more for computer
	DCHP Server software. For this tutorial, I used http://www.dhcpserver.de/cms/download/


-At this point, all Pi's are powered OFF

1. Connect Ethernet switch to computer
	Note: One end of ethernet cable connects to computer, the other end connects to the internet-comes-in-from-here port (possibly marked as LAN, 5, or 0)

2. Open the old style Control Panel, go to Network and Internet->View network status and tasks, and 
		note which connections, if any, that are listed next to "Unidentified network" (don't worry if you have no such category visible)

3. Power on the ethernet switch
	Note: You should see a new unidentified network show up here. Mine was called Local Area Connection, but 
	Erica's/Tierney's was "Ethernet." The name is irrelevant, all that matters is that a new network appears. 

4. Click the name of this new network.
	- In the new window, click 'Properties'
	- Click item labeled 'Internet Protocol Version 4 (TCP/IPv4)', then the 'Properties' box
	- Click the radial 'Use the following IP address'
	- Enter an IP that you're sure won't be used elsewhere.  Mine was 192.168.100.1, set the subnet mask as 255.255.255.0
	- Change nothing else. Click 'OK', then 'OK', then 'Close'

5. In the folder where the DCHPSRV was extracted, open 'dhcpwiz.exe'
	- Click 'Next >'
	- Select the network possessing the IP address that you set in step 4, then 'Next >'
		- Note: If its not displayed, try and repeat 4
	- Check the box for 'HTTP', click 'Next >'
	- The IP-Pool is the number of IP addresses that you want to be available for your switch to distribute to however 
		many devices you connect to it (but you have to start the pool no earlier than one address after your switch's address). Mine is from 192.168.100.2 - 254
	- Feel free to make the lease time a year, it can't hurt.  Click 'Next >'
	- Click 'Write INI file' (this should be in the same folder as the dhcpwiz.exe'), click 'Next >'.  A new window should open, but if it doesn't, open 'dhcpsrv.exe'

6. In the DHCP Server window (you'll probably have to click the 'Admin' button)
	- In 'Firewall exceptions', click 'Configure'
	- In 'Service,' click 'Install', then click 'Start'

7. Connect all Raspberry Pi's (still OFF). Now power them ON.
	 Note: If you've previously set up the Pi's to connect via WiFi, you have to undo that.
				- I didnt feel like using the GUI Raspbian, so before the next step, I SSHed into each of my Pi's and entered 
					'sudo rfkill block wifi' (I think that just disables it, but I have no idea how much damage it does).
	- Wait for up to 60 seconds for your Pi's to boot and get assigned an IP (you'll know if it worked because all of the ethernet lights will blink in unison' 

9. Navigate to 'http://127.0.0.1/dhcpstatus.xml' (this is why we checked the 'HTTP' box earlier'
	- If they connected, you'll see your Pi's in the client list. If they didn't, you'll have to stop here and troubleshoot the problem (or give up and withdraw from Stetson, choose wisely)

10. Back in the dhcpsrv directory, open the dhcpsrv.ini file
	- Starting AFTER the 'ROOT=...' line, you'll see each of your Pi's information
	- I overwrote my IP addresses to ###.###.###.11/12/13/14, but whatever you choose, these four IP's MUST match those from your Beowulf Cluster machinefile, so make them memorable
	- Save and close the .ini file, then SSH into each Pi and shut it down with 'sudo shutdown -h now'
		- Whenever possible, Raspberry Pi's should always be shut down before powered off
	- Power OFF the Pi's

11. Power ON the Pi's, 
	- Wait for the lights to blink again, then reopen the .INI file, and refresh the web page. Your IP addresses should be the ones that you overwrote. 
	- For further confirmation, SSH into pi@'your_specfied_ip_address'

12. Pat yourself on the back because you should be done.

-- What the heck is going on behind the scenes? --
	- Note: If you're good with networking, or you just don't care, no need to keep reading
		
	 DCHP is the service that hands out IP addresses from a pool whenever it receives a request from a device (aka node). When you powered on your Pi's, you had to wait a little while because your Pi needed to boot up, recognize the Ethernet cable, and send a request to your DHCP software.  All self-respecting Wireless Access Points have a DHCP server built into them.  DHCP great for every device except those that depend on consistant (static) IP addresses... like a Beowulf Cluster.  So you've just turned your computer into a DHCP server.  
	 The .INI file, like a Makefile, is a just file that holds the main program's build information.  All the dhcpwiz.exe did was write the .INI file for you. However, unlike a Makefile, the server doesn't like to run without its .INI (config file). The IP address that you set in Control Panel is the address of your Ethernet switch, and the reason you did that is because you want to tell your DCHP server "Hey pal, I have a network host connected to my computer at this address, so I need you to hand out IP's to any client that requests one from that access point (your switch)."
	 Something else, your Beowulf Cluster is not connected to the internet.  You can change some Windows settings to let it online through your computer (and its IP will be masked behind your computer, actually), but if you have all of the software installed on your Pi's already, the web isn't really necessary. From now on wherever you bring your cluster, as long as you bring your computer, you can just plug in the cluster and use it, as is, without having to waste time reconfiguring IP addresses. Finally, its not necessary to only use one computer between your group members. Your cluster will work seamlessly on any computer iff it has DCHP server software, and uses the SAME IP's throughout all of the steps (including the one from Control Panel).
