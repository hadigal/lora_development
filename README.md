# OPTIMIZED PLACEMENT OF LORA GATEWAY AND NODES #

## **Multiconnect mdot - conduit setup and communication:**

### **1. Configuring the conduit gateway:**
- ssh into the mlinux based conduit gateway:
  `ssh admin@<conduit_IP_addr>`
- Look for `/opt/lora/lora-network-server.conf.sample` file under `/opt/lora` directory
- Copy the file to `/var/config/lora/` directory. If this **lora** directory is
  not present; create a new using following command: `mkdir /var/config/lora`
- Now use **cp** command to copy the conf file `/var/config/lora` directory: `cp /opt/lora/lora-network-server.conf.sample /var/config/lora/lora-network.`
- Open the config file and configure the below mentioned keys according to your requirements:
  * frequencySubBand
  * under network settings: name and passphrase.
  * Note the name and passpharse to use it later to configure the mdot.
  * Use any bash based editor of your choice to edit these files.
    * e.g. `vim lora-network-server.conf`
    * save and exit after editing.
- Once you have finished editing the restart the conduit gateway:
  * `/etc/init.d/lora-network-server restart`; can also use linux based `systemctl restart` command

### **2. Configuring the mdot**
1. After connecting the mdot module securely with the mdot developer board; connect it using USB to connect to PC.
2. Make sure you flash the AT command firmware on to the developer board first. The flashing procedure is mentioned separately below.
3. Check if the device is enumrated in the /dev directory for linux machine; it should be visible as `/dev/ttyASM0` and `/dev/ttyXRUSB`. I used screen to access the device.
   - `screen /dev/ttyASM0 115200` 115200 is the baud rate.
4. following are the set of commands needed to configure the developer board:
   - `mosquitto_sub -t /lora/+/up`
   - `at&v` : list current configured settings for the mdot. Make changes as per the
      conduit config file
   - `at+fsb=7` : set frequencySubBand for mdot to certain value as per conduit config
   - `at+ni=1,<network_username_set>` : to set the mdot username to conduit config
   - `at+nk=1,<network_passphrase_set>`: passpharse
   - `at&w` : save to memory
   - `at2` : reset and start with the changes
   - `at+join` : join the mdot to the gateway.

5. Now on the conduit side check whether mdot has joined successfully or not:
   - `lora-query --node-list` :  this should list out the connected mdot node
   - verify the same with the reference id
6. use mqtt service to open the conduit to receive the data received from mdot using lora
   - `mosquitto_sub -t /lora/+/up`
   - on the mdot side to check if the connection is still active before sending:
   - `at+njs` : returns **1** if connected else **0**
   - Now to send data use the following command:
     * `at+send=<msg>` : spreading factor decides the payload; higher the sf lower the payload
   - Now on conduit side again you can see the information from the packet received
   - This information contains all the fields from the network params used for connection along with msg sent which is encoded in **BASE64** format. Convert this to ASCII for a readable format message.

### **3. Configuring the eclipse for mdot development**
- You can use offfline or online ide as per your needs.
- For Offline:
  * clone the git repo for libmdot-mbed5: `git clone https://github.com/MultiTechSystems/libmDot-mbed5.git`
  * This repo is for Dot library which provides a LoRaWan certified stack for LoRa
  communication using MultiTech mDot devices.
  * Instructions for configuring the eclipse for development is mentioned here:
    - https://os.mbed.com/teams/MultiTech/wiki/Using-Eclipse-and-mbed-5-for-MTS-Devices
    - follow the step by step process to setup the development environment.
    - Once you are done try out some examples from the following address:
	* https://os.mbed.com/teams/MultiTech/code/Dot-Examples/
- **Note:** We can also use [arm-mbed-cli](https://github.com/ARMmbed/mbed-cli#using-mbed-cli) based development instead of configuring Eclipse.

## **Solution if you are not able to reflash the mdot**
- This happens for the lora module code especially eg. the code to tx and rx using lora and mDot
- To reprogram follow the below steps:
  1. Need to delete the  node from the gateway(conduit):
     * `lora-query -n` -- This will give you a list of mdot nodes connected to the gateway
  2. Now run the following command to delete the desired node from the above list:
     - FYI the node id is printed on the mdot module. cmd: `lora-query -x device <mdot/xdot_node_id>`
     - On success you will get the following message on the terminal: **{status:"success"}**

## **Dynamically updating spreading factor(data rate) for the mdot**
- The data rate is directly related to the SF; it decides the amount of data which can be transmitted across to the gateway using the conduit.
- Higher the SF means lower the BW and data rate.
- For US channel (915 MHz) band the following spreading factors are available:
  * SF10BW125 = DR0; PL --> 11B
  * SF9BW125 = DR1; PL --> 53B
  * SF8BW125 = DR2; PL --> 129B
  * SF7BW125 = DR3; PL --> 242B
  * SF8BW500 = DR4; PL --> 242B
### **1. Using the AT command firmware on the mdot**
- Flash the mdot with the any of the two versions of AT fw: 3.0.0 or 3.1.0 as per the process mentioned above.
- Open the COM to input the set of commands `screen /dev/ttyXRUSB0 115200`
- The following are the set of commands used to update the SF:
  * `ATI`: Displays the firmware version flashed on the mdot
  * `AT&V`: Displays the current settings for the mDot
  * Aside: We need to set the settings at mdot as per the settings set in lora-network-server.conf in the gateway to join mdot with gateway
  * Need to set frequencySubBand, joindelay, network_name, network_passphrase_set.
  * `AT+FSB=<FSB_SET_IN_CONF_FILE>` this sets the frequencySubBand as per the input
  * `AT+NI=1,<GATEWAY_NW_NAME>` this sets the gateway network network_name
  * `AT+NK=1,<GATEWAY_NW_PASSPHRASE>` this sets the gateway network passpharse
  * `AT&W`: This saves the current changes to memory
  * `ATZ`: resets the mDot
  * `AT+JOIN`: If all the above mentioned values are correct as per the conf file the mdot successfully joins with the gateway. On success it displays "successfully joined"
  * `AT+NJS`: if the return val is 1 the  is joint to a gateway else 0.
  * Now if want to send any data of larger size; change to spreading factor to higher value.
  * `AT+SEND=<MSG_FOR_CONDUIT>`  this is used to send data to GATEWAY
  * If the data size is higher than the size which the current data rate(SF) can handle message wont be relayed across; you will receive an message saying: **Data rate exceeds max size for given SF**
  * To change the SF use following commands:
    1. `AT+TXDR?`: This displays the current SF set for the mdot.
    2. `AT+TXDR=<DR1-DR4>` to increase the PL size to above mentioned data sizes
    3. `AT&W`: save the settings to memory
    4. `ATZ`: reset the mdot by your choice if does not affect even if it is not done. Remember to rejoin the gateway using above mentioned command after reset.
  * All this can be viewed on the gateway as well by querying the node list to see the joined node: `lora-query -n`
  * To view the data packets received use mqtt broker to subscribe to the incoming lora topic: `mosquitto_sub -t lora/+/up`
- The below screen-shot shows the affect of change in SF:
  * Initial SF set to SF10BW125:
  ![Initial SF set to SF10BW125:](https://github.com/hadigal/lora_development/blob/master/mdot_log_files/dynamic_sf_update/at_sf_high.png)
  * After decreasing SF set to SF8BW125:
  ![After decreasing SF set to SF8BW125:](https://github.com/hadigal/lora_development/blob/master/mdot_log_files/dynamic_sf_update/at_sf_dec_affect.png)

### **2. mqtt client server development**
- **What is MQTT?**
  * MQTT stands for MQ Telemetry Transport. It is a publish/subscribe, extremely simple and lightweight messaging protocol, designed for constrained devices and low-bandwidth, high-latency or unreliable networks. The design principles are to minimize network bandwidth and device resource requirements whilst also attempting to ensure reliability and some degree of assurance of delivery. These principles also turn out to make the protocol ideal of the emerging “machine-to-machine” (M2M) or “Internet of Things” world of connected devices, and for mobile applications where bandwidth and battery power are at a premium.
- We are developing a publish application in C using mqtt paho for marconi sever which will publish message to conduit server to get SF update value information.
- The current application "./mqtt_client_marconi.c" is capable of sending a string message to the gateway.
- This application will be extended to send SF update value information.
- Please find the Instructions to install mqtt paho client for C library here: https://www.eclipse.org/paho/clients/c/
- The below screen-shot shows the application publishing message to mqtt broker on the right and conduit gateway subscribing to the published message:
  * left(conduit sub) right(marconi pub):
  ![left(conduit sub) right(marconi pub)](https://github.com/hadigal/lora_development/blob/master/mqtt/mqtt_send_msg_to_gateway.png)
