#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include "MQTTClient.h"

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID "MDOTGATEWAYSUB"

#define TOPIC1       "lora/+/up"
// #define TOPIC2	"lora/00-80-00-00-00-00-fe-37/up"
// #define TOPIC3 "lora/00-80-00-00-04-00-98-02/up"
// #define TOPIC4 "lora/00-80-00-00-04-00-98-02/up"

#define QOS         1
#define TIMEOUT     10000L

const char *username = "mtcdt";
const char *password = "Q4opCvmQ";
char *log_file_name_sub = "log_mqtt_marconi_sub.txt";
/*char *log_file_name_sub1 = "log_mqtt_marconi_sub_mdot1.txt";
char *log_file_name_sub2 = "log_mqtt_marconi_sub_mdot2.txt";
char *log_file_name_sub3 = "log_mqtt_marconi_sub_xdot1.txt";
char *log_file_name_sub4 = "log_mqtt_marconi_sub_xdot2.txt";*/

char full_path[128] = "/home/adigal/mqtt_test/";
FILE *file_obj;

char path[55];

volatile MQTTClient_deliveryToken deliveredtoken;

uint64_t dots;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
  printf("Message with token value %d delivery confirmed\n", dt);
  deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
  time_t secs = time(NULL);
  int i;
  char* payloadptr;
  char *devEUI;
  char *devTOA;
  devEUI = (char *)calloc(18,sizeof(*devEUI));
  devTOA = (char *)calloc(5,sizeof(*devTOA));
  uint8_t flg = 0;
  uint32_t currItr;
  uint8_t devItr = 0;
  uint8_t temp_itr = 0;

  //sprintf(full_path,"%s",log_file_name_sub);

  /*if(access(full_path, F_OK) != -1)
  {
        printf("File:%s exists; appending the existing file\n",full_path);
  }*/

  //file_obj = fopen(full_path,"w");

  //writing the time in secs to file to plot a scatter graph

  printf("\n=========== Message arrived ===========\n");
  printf("TOPIC: %s\n", topicName);
  printf("MESSAGE LEN:%d\n",message->payloadlen);
  printf("MESSAGE: ");

  payloadptr = message->payload;
  
  for(i=0; i < message->payloadlen && payloadptr[i] != '|'; i++)
  {   
      putchar(*payloadptr++);
      /*if(flg == 0)
      {
      	putchar(*payloadptr++);
      }
      else if(flg == 1)
      {
	devEUI[devItr++] = payloadptr[i];
      }
      else if(flg == 2)
      {
	devTOA[temp_itr++] = payloadptr[i];
      }*/ // corrupting memory
  }

  ++i;
  snprintf(devEUI,17,"%s",payloadptr + i);
  i += 17;
  snprintf(devTOA,5,"%s",payloadptr + i);
  printf("\nToA:%s\n",devTOA);
  printf("device ID:%s\n",devEUI);

  /*for(size_t itr = 0; itr < strlen(devEUI); ++itr)
  {
	putchar(devEUI[itr]);
  }*/
  //putchar('\n');
 
  //sscanf(devEUI,"%lu",&dots);
  //printf("\nDEV EUI:%lu\n",dots);
  
  /*char *nullChar;
  errno = 0;

  long temp2 = strtol(devEUI,&nullChar,10);
  
  if(errno == ERANGE || *nullChar != '\0' || devEUI == nullChar)
  {
     printf("ERROR in conversion");
  }

  printf("TEMP2:%ld\n",temp2);*/
  
  fprintf(file_obj,"PAYLOAD:%s | Device ID:%s Time: %ld | TOA:%s\n",(char *)message->payload,devEUI,secs, devTOA);
  
  printf("Time of arrival:%s\n",asctime(localtime(&secs)));
  free(devEUI);
  free(devTOA);
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
  //strcat(full_path,log_file_name_sub);
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

  printf("Subscribing to topic %s\nfor mqtt client %s using QoS%d\n\n"
         "Press Q<Enter> to quit\n\n", TOPIC1, CLIENTID, QOS);
  MQTTClient_subscribe(client, TOPIC1, QOS);

  /*
  printf("Subscribing to topic %s\nfor mqtt client %s using QoS%d\n\n"
         "Press Q<Enter> to quit\n\n", TOPIC2, CLIENTID, QOS);
  MQTTClient_subscribe(client, TOPIC2, QOS);

  printf("Subscribing to topic %s\nfor mqtt client %s using QoS%d\n\n"
         "Press Q<Enter> to quit\n\n", TOPIC3, CLIENTID, QOS);
  MQTTClient_subscribe(client, TOPIC3, QOS);

  printf("Subscribing to topic %s\nfor mqtt client %s using QoS%d\n\n"
         "Press Q<Enter> to quit\n\n", TOPIC4, CLIENTID, QOS);
  MQTTClient_subscribe(client, TOPIC4, QOS);
  */

  do
  {
    ch = getchar();
  } while(ch!='Q' && ch != 'q');
  MQTTClient_disconnect(client, 10000);
  MQTTClient_destroy(&client);
  fclose(file_obj);
  return rc;
}
