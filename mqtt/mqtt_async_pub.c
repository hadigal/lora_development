#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include "MQTTClient.h"

#define ADDR "tcp://localhost:1883"
#define CLID "MDOTGATEWAYPUB"
#define TOPIC "testbedPub"
#define QOS 1
#define PAYLOAD "marconi"

const char *username = "mtcdt";
const char *pwd = "Q4opCvmQ";
static const char *fileName = "log_mqttPub.txt" ;// static to hide it from other files
static char fullPath[128] = "/home/adigal/mqtt_test/mqtt_pub/";
volatile MQTTClient_deliveryToken dt;
FILE *fObj;

static bool sigFlg = true; // set ot fasle when interrup is caught
static time_t secs;

// signal handler
static void intHandle(int dummy)
{
  sigFlg = false;
}

// setCallback functions
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
  time_t temp = time(NULL);
  //printf("temp:%s\n",asctime(localtime(&temp)));
  printf("\n::::::::::::::::Connection Lost::::::::::::::::\nReason:%s\n",reason);
  printf("timestamp:%s\n",asctime(localtime(&temp)));
  printf(":::::::::::::::::::::::::::::::::::::::::::::::\n");
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

  // declare the MQTTClient_publishMessage members;
  pubMsg.payload = PAYLOAD;
  pubMsg.payloadlen = strlen(PAYLOAD);
  pubMsg.qos = QOS;
  pubMsg.retained = 0;

  //setCallback() before connects
  MQTTClient_setCallbacks(obj,NULL,cLost,msgarvd,delv);

  //log file fullPath
  strcat(fullPath,fileName);
  if((access(fullPath,F_OK)) != -1)
  {
    printf("File %s already present @:%s .... overwriting the existing log\n",fileName,fullPath);
  }
  else
  {
    printf("Created a log for publish message timestamps @:%s\n",fullPath);
  }
  fObj = fopen(fullPath,"w");

  // implementing the signal handler
  struct sigaction sigAct;
  sigAct.sa_handler = intHandle;
  sigaction(SIGINT,&sigAct,NULL);

  while(sigFlg)
  {
    retVal = MQTTClient_connect(obj,&cntOpt);
    if(retVal != MQTTCLIENT_SUCCESS)
    {
      printf("Error in connect()\t Ret Val:%d\n",retVal);
      return EXIT_FAILURE;
    }

    // declare the MQTTClient_publishMessage members;
    dt = 0;
    MQTTClient_publishMessage(obj,TOPIC,&pubMsg,&local_dt);
    printf("Published MSG:%s to ClientID:%s to Topic:%s... waitng for delv. tocken\n",PAYLOAD,CLID,TOPIC);
    printf("local_dt:%d\n",local_dt);
    secs = time(NULL);
    printf("Time Stamp for successful delivery:%s",asctime(localtime(&secs)));
    fprintf(fObj,"%ld\n",secs);
    sleep(30);
  }
  printf("disconnect client obj\n");
  MQTTClient_disconnect(obj,10000);//release()
  printf("release the memory allocated for client\n");
  MQTTClient_destroy(&obj);//free()
  printf("close the client objr\n");
  printf("Closing the log file\n");
  fclose(fObj);
  return retVal;
}
