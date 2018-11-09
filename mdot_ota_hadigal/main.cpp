/******************************************************************************
* Name:  main.cpp
* Author: Hrishikesh Adigal
* desc.: This code establishes connection between mdot and gateway and sends
        lora pkt. to gateway using mdot.
*******************************************************************************/
#include "mbed.h" //mbed os header
#include "mDot.h" //from libmdot library
#include "ChannelPlans.h"
#include <string>
#include <vector>

// creating global var for the configuration as per the conf file in the gateway
static std::string network_name = "MTCDT-floor3";
static std::string network_passphrase = "MTCDT-floor3";
static uint8_t fsb = 2;
static uint8_t jdelay = 1;
lora::ChannelPlan* plan = NULL; //HR: creating a NULL ptr of class type channelplan from the lora namespace --> defined in ChannelPlan.h
Serial pc(USBTX, USBRX);
static lora::NetworkType network_type = lora::PRIVATE_MTS;

void err_logging(mDot *dot, const char *msg, int32_t ret_val)
{
  printf("HR:Error Msg:%s - Ret Val:%ld, %s - %s\n",msg,ret_val,mDot::getReturnCodeString(ret_val).c_str(),dot->getLastError().c_str());
}

int main(void)
{
  // COM PORT SUPPORT
  pc.baud(115200);
  // US BAND TX and RX
  plan = new lora::ChannelPlan_US915();
  assert(plan); // HR: based on the plan defined in the ~/examples/example_config.h use assert method to use the plan.

  int32_t ret_val; //ret val
  mDot *dot; //ptr to class mdot
  std::vector<uint8_t> data;
  std::string msg = "hello world"; //hardcoding the message to be relayed accross the mDot

  // getting handle of mDot
  dot = mDot::getInstance(plan);
  assert(dot);
  //reset to default config to know the current state
  dot->resetConfig();
  dot->resetNetworkSession();
  // make sure library logging is turned on
  //dot->setLogLevel(mts::MTSLog::INFO_LEVEL);


  // now print the current library information to port
  printf("HR:Version:%s\n",dot->getId().c_str());
  //printing the information about the setting mdot to the values mentioned above

  printf("HR:Setting FSB to:%u\n",fsb);
  // setting fsb
  ret_val = dot->setFrequencySubBand(fsb);
  if(ret_val != mDot::MDOT_OK)
  {
    err_logging(dot,"failed to set the FSB on mdot",ret_val);
  }

  //setting net name
  printf("HR:Setting netwrok name\n");
  ret_val = dot->setNetworkName(network_name);
  if(ret_val != mDot::MDOT_OK)
  {
    err_logging(dot,"Error in setting the network_name",ret_val);
  }
  else
  {
    printf("HR:Successfully set network name\n");
  }

  //setting network_passphrase
  printf("HR:Setting the network passphrase..\n");
  ret_val = dot->setNetworkPassphrase(network_passphrase);
  if(ret_val != mDot::MDOT_OK)
  {
    err_logging(dot,"Failed to set network passphrase",ret_val);
  }
  else
  {
    printf("Successfully set passphrase\n");
  }

  //setting network type
  /*ret_val = dot->setPublicNetwork(network_type);
  if (ret_val != mDot::MDOT_OK)
  {
     err_logging(dot,"failed to set network type",ret_val);
  }
  else
  {
    printf("HR:Success in setting public network type\n");
  }*/

  //setting network join delay
  ret_val = dot->setJoinDelay(jdelay);
  if(ret_val != mDot::MDOT_OK)
  {
    err_logging(dot,"Failed to set join delay",ret_val);
  }
  else
  {
    printf("HR:Successfully set join delay\n");
  }

  // Attempting to join the network
  while((ret_val = dot->joinNetwork()) != mDot::MDOT_OK)
  {
    err_logging(dot,"Failed to join the network",ret_val);
    wait(2); //2sec wait before trying again
  }

  printf("HR:Successfully joined the gateway...\n");
  // formating str to be sent accross to the gateway using lora
  for(std::string::iterator itr = msg.begin(); itr != msg.end(); ++itr)
  {
    data.push_back((uint8_t) *itr); //formating the str to be sent accross gateway to unit_8 format
  }

  //sending data to gateway
  uint32_t f_flag = 0;
  while(true)
  {
    ret_val = dot->send(data); // ack is enabled by default
    if(ret_val != mDot::MDOT_OK)
    {
      err_logging(dot,"Failed to send data to gateway",ret_val);
      ++f_flag;
      if(f_flag == 3)
      {
        break;
      }
    }
    else
    {
      f_flag = 0;
      printf("HR:Successfully sent data to gateway....\n");
    }
    wait(3);
  }
  //return EXIT_SUCCESS;
  return 0;
}
