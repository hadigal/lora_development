# OPTIMIZED PLACEMENT OF LORA GATEWAY AND NODES #
# **--------------------------------------------------------------------------------**
## **Multiconnect mdot - conduit setup and communication:**
## **--------------------------------------------------------------------------------**

## **Configuring the conduit gateway:**
## **--------------------------------------------------------------------------------**

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

## **Configuring the mdot**
## **--------------------------------------------------------------------------------**

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

## **Configuring the eclipse for mdot development**
## **--------------------------------------------------------------------------------**
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
