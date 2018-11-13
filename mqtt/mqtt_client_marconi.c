/******************************************************************************
* Name: mqtt_client_marconi.c
* Author: Hrishikesh Adigal
* Help for compiling : Use following handle [-lpaho-mqtt3c] while compling to link with the mqtt paho c headers
* Date: 11/11/2018
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"

// defining the macros for holding the mqtt client socket values
#define ADDRESS "tcp://localhost:1883" /*protocol://<ip_add>:<port_no>*/
//#define ADDRESS "tcp://130.191.166.2:1883"
#define CLIENTID "MDOTGATEWAY" /*Client id passed to the server when it connects to it*/
#define TOPIC "testbed" /*mqtt connection topic*/
#define QOS 1 /*Level upto which the broker will try sending message*/
#define TIMEOUT 10000L 
#define PAYLOAD "hello world" //payload of char array type

// set uname and passphrase to avoid misuse of the mqtt client
const char *uname = "mtcdt";
const char *passphrase = "Q4opCvmQ";

volatile MQTTClient_deliveryToken delivered_token;

int main(int argc, char *argv[])
{
  /* creating a MQTT client obj --> valid on success of create() call below*/
  MQTTClient client_obj;
  /*
   * creating a structure of type MQTTClient_connectOptions initialize it with the macro defined in MQTTClient.h
  * This deines the several setting required to mangae client connection!
  * MQTTClient_connectOptions_initializer initialized to default values set in this marco for all the structure members. This step can be skipped if we are initializing all the members properly
  */
  MQTTClient_connectOptions client_conn = MQTTClient_connectOptions_initializer;
  /* create a struct of type MQTTClient_message
  * Strucutre for mqtt message with members representing payload attributes like len, qos, payload, etc.
  * The members of the structure are initialized to default values
  */
  MQTTClient_message cli_pub_msg = MQTTClient_message_initializer;
  /* used for ack of successful msg. delivery. This is a int value */
  MQTTClient_deliveryToken ack;
  // var to hold ret value
  int ret_val;
  /* Creating a client object ready for connection with the defined values and presistence */
  MQTTClient_create(&client_obj,ADDRESS,CLIENTID,MQTTCLIENT_PERSISTENCE_NONE,NULL);
  client_conn.keepAliveInterval = 20; //20 secs max allowed no communication interval
  client_conn.cleansession = 1; // boolean  val which controls the behaviour of client and server at connection and disconnection time
  client_conn.username = uname; // authentication params to avoid misuse of the broker
  client_conn.password = passphrase; 
  /* Connect() attempts to connect the created client to the mqtt server. If setCallback() must be called before if we want to enable notifications and async messages */
  ret_val = MQTTClient_connect(client_obj,&client_conn);
  /* check for successful connect()
  * retvalues(-ve):
  *     1: Connection refused: Unacceptable protocol version
  *	2: Connection refused: Identifier rejected
  *	3: Connection refused: Server unavailable
  *	4: Connection refused: Bad user name or password
  *	5: Connection refused: Not authorized
  *	6-255: Reserved for future use
  */
  if(ret_val != MQTTCLIENT_SUCCESS)
  {
    printf("Failed to connect!!!\nReturn Value:%d\n",ret_val);
    return EXIT_FAILURE;
  }

  //now initialize the pub message structure
  cli_pub_msg.payload = PAYLOAD; // defined above can be user ip or even a input from a file based on some data analysis at the marconi server
  cli_pub_msg.payloadlen = strlen(PAYLOAD); //len of the supported utf-8 string
  cli_pub_msg.qos = QOS;
  cli_pub_msg.retained = 0;

  //publish message to the topic at the mqtt broker to be subscribed and received by desired client
  // this is like send() in POSIX sockets. A mqtt_deliveryToken is issued for the message after successful delivery
  MQTTClient_publishMessage(client_obj,TOPIC,&cli_pub_msg,&ack);
  printf("waiting to publish msg:%s to client_id:%s for %d secs\n",PAYLOAD,CLIENTID,(int)(TIMEOUT/1000));
  /*waiting for publish Complete. Here a block mechanism is implemented until the application is in use i.e message message delivered successfully or unitl specified timeout
  * This is only used in the synchro mode to sync the main thread exe with complete pub of message.
  */
  ret_val = MQTTClient_waitForCompletion(client_obj,ack,TIMEOUT);
  // now print ack for pub
  printf("Ack token for payload:%d\n",ack);
  // now disconnect operation after Successful publish
  printf("disconnection operation for given client\n");
  /* This function attempts to disconnect the client from the MQTT server. 
  * In order to allow the client time to complete handling of messages that are in-flight when this function is called, a timeout period is specified. 
  * When the timeout period has expired, the client disconnects even if there are still outstanding message acknowledgements. 
  * The next time the client connects to the same server, any QoS 1 or 2 messages which have not completed will be retried;
  *  depending on the cleansession settings for both the previous and the new connection */
  MQTTClient_disconnect(client_obj,10000);
  printf("Destroying the created client pub obj\n");
  MQTTClient_destroy(&client_obj); // free the memory allocated for the client during create()
  return EXIT_SUCCESS;
}
