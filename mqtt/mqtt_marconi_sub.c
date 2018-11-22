#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "MQTTClient.h"

#define ADDRESS     "tcp://localhost:1883"
//#define ADDRESS "tcp://130.191.166.2:1883"
//#define CLIENTID    "MISTClientSub"
#define CLIENTID "MDOTGATEWAYSUB"
//#define TOPIC       "mist"
#define TOPIC       "testbedSub"
#define QOS         1
#define TIMEOUT     10000L

const char *username = "mtcdt";
const char *password = "Q4opCvmQ";
char *log_file_name_sub = "log_mqtt_marconi_sub.txt";
char full_path[128] = "/home/adigal/mqtt_test/";
FILE *file_obj;

//fp = fopen(full_path,"w");

volatile MQTTClient_deliveryToken deliveredtoken;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
  printf("Message with token value %d delivery confirmed\n", dt);
  deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
  int i;
  char* payloadptr;
  time_t secs = time(NULL);
  //writing the time in secs to file to plot a scatter graph
  fprintf(file_obj,"%ld\n",secs);
  
  printf("Message arrived\n");
  printf("     topic: %s\n", topicName);
  printf("   message: ");
  payloadptr = message->payload;
  for(i=0; i<message->payloadlen; i++)
  {
      putchar(*payloadptr++);
  }
  putchar('\n');
  printf("Time of arrival:%s\n",asctime(localtime(&secs)));
  MQTTClient_freeMessage(&message);
  MQTTClient_free(topicName);
  return 1;
}

void connlost(void *context, char *cause)
{
  printf("\nConnection lost\n");
  printf("     cause: %s\n", cause);
}

int main(int argc, char* argv[])
{
  MQTTClient client;
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  int rc;
  int ch;
  MQTTClient_create(&client, ADDRESS, CLIENTID,
                    MQTTCLIENT_PERSISTENCE_NONE, NULL);
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 1;
  conn_opts.username = username;
  conn_opts.password = password;
  
  //log file
  strcat(full_path,log_file_name_sub);
  //FILE *file_obj;
  if(access(full_path, F_OK) != -1)
  {
        printf("File:%s exists; appending the existing file\n",full_path);
  }
  file_obj = fopen(full_path,"w");

  MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
  if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
  {
    printf("Failed to connect, return code %d\n", rc);
    exit(EXIT_FAILURE);
  }
  printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
         "Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);
  MQTTClient_subscribe(client, TOPIC, QOS);
  do
  {
    ch = getchar();
  } while(ch!='Q' && ch != 'q');
  MQTTClient_disconnect(client, 10000);
  MQTTClient_destroy(&client);
  fclose(file_obj);
  return rc;
}

