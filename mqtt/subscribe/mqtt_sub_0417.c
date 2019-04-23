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

#define TOPIC1       "loraSub"
// #define TOPIC2	"lora/00-80-00-00-00-00-fe-37/up"
// #define TOPIC3 "lora/00-80-00-00-04-00-98-02/up"
// #define TOPIC4 "lora/00-80-00-00-04-00-98-02/up"

#define QOS         1
#define TIMEOUT     10000L

const char *username = "mtcdt";
const char *password = "Q4opCvmQ";
char *log_file_name_sub = "log_mqtt_marconi_sub.txt";
char *gw_tmst = "gw_tmst_log.txt";
char *txTmDot = "txTime_dot_log.txt";

/*
char *log_file_name_sub1 = "log_mqtt_marconi_sub_mdot1.txt";
char *log_file_name_sub2 = "log_mqtt_marconi_sub_mdot2.txt";
char *log_file_name_sub3 = "log_mqtt_marconi_sub_xdot1.txt";
char *log_file_name_sub4 = "log_mqtt_marconi_sub_xdot2.txt";
*/

char full_path[128] = "/home/adigal/mqtt_test/";
char full_path2[128] = "/home/adigal/mqtt_test/";
char full_path3[128] = "/home/adigal/mqtt_test/";
FILE *file_obj;
FILE *file_obj2;
FILE *file_obj3;

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
  char *devTmst;
  devEUI = (char *)calloc(24,sizeof(*devEUI));
  uint8_t devItr = 0;
  devTmst = (char *)calloc(28,sizeof(*devTmst));
  uint8_t devTmstItr = 0;
  uint32_t itr2 = 0;
  int32_t pLen = message->payloadlen;
  uint16_t sub = pLen - 62;

  printf("\n=========== Message arrived ===========\n");
  printf("TOPIC: %s\n", topicName);
  printf("MESSAGE LEN:%d\n",pLen);
  printf("MESSAGE: ");

  payloadptr = message->payload;
  // printf("PAYLOAD LEN:%lu\n",message->payloadlen);
  // printing msg to console
  //for(i = 0; i < message->payloadlen && payloadptr[i] != '|'; i++)
  int devStop = 264;
  uint32_t itr = 0;

  char arrMsg[242];
  //payloadptr[i]
  uint8_t flg = 0;
  char *strUdef = (char *)calloc(10,sizeof(*strUdef));
  char *pl = (char *)calloc(11,sizeof(*pl));
  uint16_t plItr = 0;

  uint8_t itr1 = 0;
  //for(i = 0; i < message->payloadlen && payloadptr[i] != '|'; i++)
  //for(i = 0; i < message->payloadlen; i++)
  for(i = 0; i < message->payloadlen; i++)
  {
    if(itr1 < 9)
    {
      strUdef[itr1++] = *payloadptr;
    }

    if(*payloadptr == '|')
    {
      ++i;
      *payloadptr++;
      break;
    }

    if((i >= sub) && (plItr < 10))
    {
      printf("\n*payloadptr:%c\ti:%d\n",*payloadptr,i);
      *(pl+plItr) = *payloadptr;
      ++plItr;
    }
    putchar(*payloadptr++);
    // if(*payloadptr++ == '|')
    // {
    //   flg = 1;
    //   continue;
    // }
    //
    // if(flg == 0)
    // {
    //   putchar(*payloadptr++);
    // }
    // else if(flg == 1)
    // {
    //   devEUI[itr++] = *payloadptr++;
    // }
  }
  printf("DEBUG:::::::val of pl:%s\n",pl);

  /*************** remove this *****************/

  // for(;i < message->payloadlen; ++i)
  // {
  //   if(*payloadptr == '|')
  //   {
  //     break;
  //   }
  //   if(devItr < 23)
  //   {
  //     devEUI[devItr++] = *payloadptr;
  //   }
  // }
  //
  // for(;i < message->payloadlen; ++i)
  // {
  //   if(*payloadptr == '|')
  //   {
  //     break;
  //   }
  //   if(devItr < 9)
  //   {
  //     devTmst[devTmstItr++] = *payloadptr;
  //   }
  // }

  //++i;

  //saving dev id to char buff
  //for(size_t itr = 0; i < message->payloadlen && payloadptr[i] != '|'; i++)
  printf("\nDEBUG:::::::val of i:%d\n",i);
  int devLen = i+23;

  // while(itr < 24 && payloadptr != NULL)
  // {
  //   devEUI[itr++] = *payloadptr++;
  // }

  //printf("\nDEBUG::::::::::::payload:%s\n",payloadptr+i+1);

  //snprintf(payloadptr+i,24,"%s",devEUI);
  //for(; i < devLen; i++)
  printf("\n-------------DEBUG::::::::::::payload:%s\n",payloadptr+i+1);

  while(itr < 23 && i < devLen)
  {
	   //printf("\npayloadptr[%d]:%c\n",i,payloadptr[i]);
	   //devEUI[itr] = payloadptr[i];
     devEUI[itr] = *payloadptr;
     putchar(*payloadptr++);
     ++itr;
     // printf("current char[%d]:%c\n",i,*(payloadptr);
     // devEUI[itr++] = *payloadptr++;
     i++;
     //++itr;
  }
  //printf("\n-------------DEBUG::::::::::::payload:%s\n",payloadptr+i+1);
  ++i;

  devLen = i+9;
  while(i < devLen)
  {
    if(*payloadptr == '|')
    {
      ++i;
      *payloadptr++;
      break;
    }

    ++i;
    *payloadptr++;
  }

  devLen = i+27;
  printf("devLen:%d\ti:%d\n",devLen,i);
  while(devTmstItr < 27 && i < devLen)
  {
	   //printf("\npayloadptr[%d]:%c\n",i,payloadptr[i]);
	   //devEUI[itr] = payloadptr[i];
     //devTmst[devTmstItr++] = *payloadptr;
     devTmst[devTmstItr] = *payloadptr;
     putchar(*payloadptr++);
     ++devTmstItr;
     // printf("current char[%d]:%c\n",i,*(payloadptr);
     // devEUI[itr++] = *payloadptr++;
     i++;
     //++itr;
  }

  printf("\nDEBUG::::::::::::devEUI:%s\n",devEUI);
  printf("\nDEBUG::::::::::::devTmst:%s\n",devTmst);

  printf("\n\nDEBUG:STRUDEF::::%s\n",strUdef);
  if(strncmp(strUdef,"undefined",10) == 0)
  {
    printf("CAUGHT EXCEPTION....\n");
    free(devEUI);
    free(devTmst);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
  }

  /************************ TEMP ************************************

  ************************ TEMP ************************************/

  printf("\nEND DEV ID:%s\n",devEUI);

  //struct tm timeEpoch = {0};
  ++i;

  // saving estimated ToA to char buff
  // for(size_t itr = 0; i < message->payloadlen && payloadptr[i] != '|'; i++)
  // {
  //   //printf("\n======>payloadptr[%d]:%c\n",i,payloadptr[i]);
  //   devTOA[itr] = payloadptr[i];
  //   ++itr;
  // }


  // uncomment this later

  // itr = 0;
  // int toaLen = message->payloadlen;
  // while(itr < 5 && i < toaLen)
  // {
  //   //printf("\n======>payloadptr[%d]:%c\n",i,payloadptr[i]);
  //   printf("current char:%c\n",payloadptr[i]);
  //   devTOA[itr++] = payloadptr[i++];
  //   ++itr;
  // }

  //int mili;
  //int val = sscanf(devTOA,"%d-%d-%dT%d:%d:%d.%dZ", &timeEpoch.tm_year, &timeEpoch.tm_mon, &timeEpoch.tm_mday,&timeEpoch.tm_hour,&timeEpoch.tm_min,&timeEpoch.tm_sec,&mili);
  //printf("Val:%d\n",val);
  // if(val != 7)
  // {
  //   printf("Error in parsing\n");
  // }

  //time_t t = mktime(&timeEpoch);

  //printf("---->time since epoch:%ld\n",t);

  //printf("\nTime on Air:%s\n",devTOA);

  //fprintf(file_obj,"Device ID:%s | Msg Time: %ld | TOA:%s\n",devEUI,secs, devTOA);

  fprintf(file_obj,"Device ID:%s | Msg Time: %ld\n",devEUI,secs);
  fprintf(file_obj2,"Device ID:%s | Msg Time: %s\n",devEUI,devTmst);
  fprintf(file_obj3,"Device ID:%s | Tx Time: %s\n",devEUI,pl);

  printf("Time of arrival:%s\n",asctime(localtime(&secs)));
  free(devEUI);
  free(devTmst);
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
  strcat(full_path2,gw_tmst);
  strcat(full_path3,txTmDot);

  if(access(full_path, F_OK) != -1)
  {
        printf("File:%s exists; appending the existing file\n",full_path);
  }

  if(access(full_path2, F_OK) != -1)
  {
        printf("File:%s exists; appending the existing file\n",full_path2);
  }

  if(access(full_path3, F_OK) != -1)
  {
        printf("File:%s exists; appending the existing file\n",full_path3);
  }


  file_obj = fopen(full_path,"w");

  file_obj2 = fopen(full_path2,"w");

  file_obj3 = fopen(full_path3,"w");

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
