/******************************************************************************
* Name:  main.cpp
* Author: Hrishikesh Adigal
* desc.: This code establishes connection between mdot and gateway and sends
        lora pkt. to gateway using mdot.
*******************************************************************************/
#include "mbed.h" //mbed os header
#include "mDot.h" //from libmdot library
#include <string>
#include <vector>

// creating global var for the configuration as per the conf file in the gateway
static std::string network_name = "MTCDT-floor3";
static std::string network_passphrase = "MTCDT-floor3";
static uint8_t fsb = 2;
static uint8_t jdelay = 1;

void err_logging(mDot *dot, const char *msg, int32_t ret_val)
{
  printf("Error Msg:%s\nRet Val:%ld\t%s\n%s\n",msg,ret_val,mDot::getReturnCodeString(ret_val).c_str(),dot->getLastError().c_str());
}

int main(void)
{
  int32_t ret_val; //ret val
  mDot *dot; //ptr to class mdot
  std::vector<uint8_t> data;
  std::string msg = "hello world"; //hardcoding the message to be relayed accross the mDot

  // getting handle of mDot
  dot = mDot::getInstance();
  //reset to default config to know the current state
  dot->resetConfig();
  // now print the current library information to port
  printf("Version:%s\n",dot->getId().c_str());
  //printing the information about the setting mdot to the values mentioned above
  printf("Setting FSB to:%u\n",fsb);
  // setting fsb
  ret_val = dot->setFrequencySubBand(fsb);
  if(ret_val != mDot::MDOT_OK)
  {
    err_logging(dot,"failed to set the FSB on mdot",ret_val);
  }
  printf("Setting netwrok name\n");
  //setting net name
  ret_val = dot->setNetworkName(network_name);
  if(ret_val != mDot::MDOT_OK)
  {
    err_logging(dot,"Error in setting the network_name",ret_val);
  }
  else
  {
    printf("Successfully set network name\n");
  }
  printf("Setting the network passphrase..\n");
  //setting network_passphrase
  ret_val = dot->setNetworkPassphrase(network_passphrase);
  if(ret_val != mDot::MDOT_OK)
  {
    err_logging(dot,"Failed to set network passphrase",ret_val);
  }
  else
  {
    printf("Successfully set passphrase\n");
  }
  //setting network join delay
  ret_val = dot->setJoinDelay(jdelay);
  if(ret_val != mDot::MDOT_OK)
  {
    err_logging(dot,"Failed to set join delay",ret_val);
  }
  else
  {
    printf("Successfully set join delay\n");
  }
  // Attempting to join the network
  while((ret_val = dot->joinNetwork()) != mDot::MDOT_OK)
  {
    err_logging(dot,"Failed to join the network",ret_val);
    wait(2); //2sec wait before trying again
  }
  printf("Successfully joined the gateway...\n");
  // formating str to be sent accross to the gateway using lora
  for(std::string::iterator itr = msg.begin(); itr != msg.end(); ++itr)
  {
    data.push_back((uint8_t) *itr); //formating the str to be sent accross gateway to unit_8 format
  }
  //sending data to gateway
  while(true)
  {
    ret_val = dot->send(data); // ack is enabled by default
    if(ret_val != mDot::MDOT_OK)
    {
      err_logging(dot,"Failed to send data to gateway",ret_val);
    }
    else
    {
      printf("Successfully sent data to gateway\n");
    }
    wait(3);
  }
  //return EXIT_SUCCESS;
  return 0;
}
