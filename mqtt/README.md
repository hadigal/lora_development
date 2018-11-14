# This repo maintains all the mqtt applications written in C for marconi server and conduit gateway
- For more info follow this link: https://github.com/hadigal/lora_development#2-mqtt-client-server-development
- I created a flow on NodeRed application of conduit gateway to send the message published from marconi to mDot
- Used mqtt input node and configured the node to the values for marconi server hosting the mqtt broker
- Used a lora output node with DevID configured for the desired mDot
- The string message was then received in one of the receive windows by the mdot.
- The flow for the nodeRed is as seen below:
  * ![NodeRed Flow sheet. On  the right most colum we can see the debug log showing the message from marconi](https://github.com/hadigal/lora_development/blob/master/mqtt/marconi_mdot_logs/nodered_mqtt_lora.png)

- A snippet of screen log for mdot is as below:
  * `[INFO] sleeping 10sPreparing frame
  [INFO] Configure radio for TX
  [INFO] Configu[INFO] successfully 2 cB
  [INFO] Packet for 01c3cc05
  [INFO] Packet Received : Port: 1 FCnt: 0000001b Size: 13 ACK: 0 DUP: 0
  Rx data: hello marconi [68656c6c6f206d6172636f6e69]
  [INFO] Packet RSSI: -29 dB SNR: 72 cB
  [INFO] successfow 1
  [INFO] light: 1648 [0x0670]
  [INFO] Pre[INFO] Rx Window 1
  NFO] light: 1648 [0x0670]
  [INFO] Preparing frame
  [INFO] Configure radio for TX
  [INFO] Configure radio for TX
  [INFO] Rx Window 1
  or until interrupt on DIO7 pin
  [INFO] application will resume after waking up�O] Rx Window 1
  [INFO] RxDone 26 bytes RSSI: -28 dB SNR: 65 cB
  [IN: 0 DUP: 0
  Rx data: hello marconi [68656c6c6f206d6172636f6e69]
  [I[INFO] light: 1664 [0x0680]
  [INFO] Preparing frame
  [INFO] Configure radio for TX
  [INFO] Configure radio for TX`

- The screenshot of terminals on marconi and my machine with screen accessing mdot device is as seen below:
  * ![Terminal logs: left(marconi terminal) right(screen log for mdot)](https://github.com/hadigal/lora_development/blob/master/mqtt/marconi_mdot_logs/mqtt_marconi_gateway_mdot.png)
