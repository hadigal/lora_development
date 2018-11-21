#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "MQTTClient.h"

#define ADDR "tcp://localhost:1883"
#define CLID "MDOTGATEWAYPUB"
#define TOPIC "testbed"
#define QOS 1
#define PAYLOAD "marconi"

const char *username = "mtcdt";
const char *pwd = "Q4opCvmQ";
static const char *fileName = "log_mqttPub.txt" ;// static to hide it from other files
static const char fullPath[128] = "/home/adigal/mqtt_test/mqtt_pub";
volatile MQTTClient_deliveryToken dt;

static bool sigFlg = true; // set ot fasle when interrup is caught

static void intHandle(int dummy)
{
  sigFlg = false;
}

void delv(void *cliObj, MQTTClient_deliveryToken fdt)
{
  printf("Message with token:%d delivered\n",fdt);
  dt = fdt;
}

int msgarvd(void *cliObj, char *topic, int topicLen, MQTTClient_message *msg)
{
  printf("Doing Nothing\n");
  return 1;
}

void cLost(void *cliObj, char *reason)
{
  printf("\n::::::::::::::::Connection Lost::::::::::::::::\nReason:%s\n",reason);
}

int main(void)
{
  MQTTClient obj; // just a void ptr used on create() call
  // MQTTClient_connectOptions_initializer is macro defined in MQTTClient.h
  MQTTClient_connectOptions cntOpt = MQTTClient_connectOptions_initializer;
  MQTTClient_message pubMsg = MQTTClient_message_initializer; // struct initialized by macro
  MQTTClient_deliveryToken local_dt; // local token used for pub()
  int retVal;
  MQTTClient_create(&obj,ADDR,CLID,MQTTCLIENT_PERSISTENCE_NONE,NULL);

  // init the struct params
  cntOpt.keepAliveInterval = 20;
  cntOpt.cleansession = 1;
  cntOpt.username = username;
  cntOpt.password = pwd;

  //setCallback() before connects
  MQTTClient_setCallbacks(obj,NULL,cLost, msgarvd, delv);
  retVal = MQTTClient_connect(obj,&cntOpt);
  if(retVal != MQTTCLIENT_SUCCES)
  {
    printf("Error in connect()\t Ret Val:%d\n",retVal);
    return EXIT_FAILURE;
  }

  // declare the MQTTClient_publishMessage members;
  pubMsg.payload = PAYLOAD;
  pubMsg.payloadlen = strlen(PAYLOAD);
  pubMsg.qos = QOS;
  pubMsg.retained = 0;

  dt = 0;
  MQTTClient_publishMessage(obj,TOPIC,&pubMsg,&local_dt);
  printf("Published MSG:%s to ClientID:%s to Topic:%s... waitng for delv. tocken\n",PAYLOAD,CLID,TOPIC);
  while(dt != local_dt);
  MQTTClient_disconnect(obj,10000);//release()
  MQTTClient_destroy(&obj);//free()
  return retVal;
}
