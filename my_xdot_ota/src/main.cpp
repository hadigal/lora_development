/******************************************************************************
* Name:  main.cpp
* Author: Hrishikesh Adigal
* Desc.: This code establishes connection between xdot and gateway and sends
*        lora pkt. to gateway using xdot.
*******************************************************************************/
//#include "mbed.h" //mbed os header
//#include "mDot.h" //from libmdot library
//#include "ChannelPlans.h"
//#include "mDotEvent.h"
#include "loraEvent.h"

time_t temp_secs;
uint32_t pkt_count =0;
//mDot* dot;

// creating global var for the configuration as per the conf file in the gateway
static std::string network_name = "MTCDT-floor3";
//static std::string network_name = "MTCDT-19760557";
//static std::string network_passphrase = "MTCDT-19760557";
static std::string network_passphrase = "MTCDT-floor3";
static uint8_t fsb = 2;
static uint8_t jdelay = 1;
lora::ChannelPlan* plan = NULL; //HR: creating a NULL ptr of class type channelplan from the lora namespace --> defined in ChannelPlan.h
static lora::NetworkType network_type = lora::PRIVATE_MTS;
static uint8_t network_skey[] = { 0xDE, 0xB1, 0xDA, 0xD2, 0xFA, 0xD3, 0xBA, 0xD4, 0xFA, 0xB5, 0xFE, 0xD6, 0xAB, 0xBA, 0xDB, 0xBA };
static uint8_t ack = 0;
static bool adr = true;
static bool deep_sleep = false;
static uint32_t tx_pwr = 21; // set tx power for the device to 21dbm


mDot* dot = NULL;

Serial pc(USBTX, USBRX);


void err_logging(mDot *dot, const char *msg, int32_t ret_val)
{
  logError("HR:Error Msg:%s - Ret Val:%ld, %s - %s\n",msg,ret_val,mDot::getReturnCodeString(ret_val).c_str(),dot->getLastError().c_str());
}

// Helper function to support rejoining to the network
void join_network()
{
    // var to hold join count
    int32_t join_attempts = 0;
    // initialzing the ret var to MDOT_ERROR mdot_ret_code enum  where it is
    // initialzed to value -1024
    int32_t ret = mDot::MDOT_ERROR;

    // attempt to join the network
    while (ret != mDot::MDOT_OK)
    {
        logInfo("ATTEMPT #: %d to join network", ++join_attempts);
        ret = dot->joinNetwork(); // This will help to set the loop criteria for breaking
        if (ret != mDot::MDOT_OK)
        {
            logError("FAILED TO JOIN THE NW; ERROR CODE:%d ERROR STR:%s", ret, mDot::getReturnCodeString(ret).c_str());
            // in some frequency bands we need to wait until another channel is available before transmitting again
            // inducing wait for channel to be available before Tx
            uint32_t delay_s = (dot->getNextTxMs() / 1000) + 1;
            // using wait() if delay is < 5 sec
            if (delay_s < 5)
            {
                logInfo("WAIITNG FOR %lu s UNTIL NEXT FREE CH", delay_s);
                wait(delay_s); // using arm platform specific mbed os wait()
            }
            // INDUCING sleep() if delay is > 5 sec
            else
            {
                logInfo("SLEEPING for %lu s UNTIL NEXT FREE CHANNEL", delay_s);
                dot->sleep(delay_s, mDot::RTC_ALARM, false);
            }
        }
    }
    return;
}

// Display general information for statistics at every hotplug of the mdot/xdot
// device i.e when either of the device is turned on the first time.
void display_config()
{
    // display configuration and library version information
    // using the vendor and arm mbed os specific functions for the same
    logInfo("=====================");
    logInfo("DEVICE CONFIGURATION");
    logInfo("=====================");
    logInfo("LIB VERSION --------- %s", dot->getId().c_str());
    logInfo("DEVICE ID/EUI ------------ %s", mts::Text::bin2hexString(dot->getDeviceId()).c_str());
    logInfo("DEFAULT DEVICE CHANNEL PLAN ----- %s", mDot::FrequencyBandStr(dot->getDefaultFrequencyBand()).c_str()); // default is se to US 915MHz band
    logInfo("CURRENT CHANNEL PLAN ----- %s", mDot::FrequencyBandStr(dot->getFrequencyBand()).c_str());
    if (lora::ChannelPlan::IsPlanFixed(dot->getFrequencyBand()))
    {
        logInfo("frequency sub band ------- %u", dot->getFrequencySubBand());
    }

    std::string network_mode_str("Undefined");
    uint8_t network_mode = dot->getPublicNetwork();
    if (network_mode == lora::PRIVATE_MTS)
        network_mode_str = "Private MTS";
    else if (network_mode == lora::PUBLIC_LORAWAN)
        network_mode_str = "Public LoRaWAN";
    else if (network_mode == lora::PRIVATE_LORAWAN)
        network_mode_str = "Private LoRaWAN";
    logInfo("PUBLIC NETWORK TYPE ----------- %s", network_mode_str.c_str());

    logInfo("=============================");
    logInfo("DEVICE CREDENTIAL CONFIG INFO");
    logInfo("=============================");
    logInfo("CURR OPERATING CLASS  ------------- %s", dot->getClass().c_str());
    logInfo("NETWORK JOIN MODE -------- %s", mDot::JoinModeStr(dot->getJoinMode()).c_str());

    // displaying the detials for current set configuration for the device
    logInfo("NETWORK NAME(id) ------------- %s", dot->getNetworkName().c_str());
    logInfo("NETWORK PASSPHRASE ----------- %s", dot->getNetworkPassphrase().c_str());
    logInfo("NETWORK EUI -------------- %s", mts::Text::bin2hexString(dot->getNetworkId()).c_str());
    logInfo("NETWORK PASSKEY -------------- %s", mts::Text::bin2hexString(dot->getNetworkKey()).c_str());

    logInfo("========================");
    logInfo("COMMUINCATION PARAM");
    logInfo("========================");
    logInfo("ACKS --------------------- %s ACK ATTEMPTS:%u", dot->getAck() > 0 ? "on" : "off", dot->getAck());
    logInfo("CURRENT TX DR SET -------------- %s", mDot::DataRateStr(dot->getTxDataRate()).c_str());
    logInfo("CURR TX PWR ----------------- %lu dBm", dot->getTxPower());
    logInfo("ANTENNA GAIN ------------- %u dBm", dot->getAntennaGain());
    logInfo("LBT ---------------------- %s", dot->getLbtTimeUs() ? "on" : "off");
    if (dot->getLbtTimeUs())
    {
        logInfo("LBT time ----------------- %lu us", dot->getLbtTimeUs());
        logInfo("LBT threshold ------------ %d dBm", dot->getLbtThreshold());
    }
}


int main(void)
{
  //Invoking custom event handler for rx data
  LoraEvent radio_event;
  // COM PORT SUPPORT
  pc.baud(115200);
  // setting log level to trace i.e. the highest level for printing the log messages
  mts::MTSLog::setLogLevel(mts::MTSLog::TRACE_LEVEL);

  // Selecting the US BAND as channel plan for TX and RX
  plan = new lora::ChannelPlan_US915();
  // error handling to check if the plan selected if US 915 or not
  assert(plan);

  int32_t ret_val; //ret val

  // getting handle of mDot. This returns a ptr to mdot class
  dot = mDot::getInstance(plan);
  // error handling to check if the ptr returned is NULL
  assert(dot);

  dot->setEvents(&radio_event);

  // check if startup mode is standby
  if (!dot->getStandbyFlag())
  {
    logInfo("mbed-os library version: %d", MBED_LIBRARY_VERSION);

    // start from a known state
    logInfo("defaulting Dot configuration");
    // reset to default config to know the current state
    dot->resetConfig(); // reset to factory setting at every boot ie. start
    dot->resetNetworkSession(); // reset the current nw session
    // make sure library logging is turned on
    dot->setLogLevel(mts::MTSLog::INFO_LEVEL); // setting log level to Info; this prints INFO messages

    // update the device nw configuration
    if (dot->getJoinMode() != mDot::OTA)
    {
        logInfo("CHANGING NW MODE TO OTA");
        // setting the device the OTA mode
        if (dot->setJoinMode(mDot::OTA) != mDot::MDOT_OK)
        {
            logError("FAILED TO SET NW MODE TO OTA");
        }
    }

    // now print the current library information to COM port
    logInfo("HR:NW ID VERSION:%s\n",dot->getId().c_str());
    //printing the information about the setting mdot to the values mentioned above

    logInfo("HR:Setting FSB to:%u\n",fsb);
    // setting fsb
    ret_val = dot->setFrequencySubBand(fsb);
    if(ret_val != mDot::MDOT_OK)
    {
        err_logging(dot,"failed to set the FSB on mdot",dot->setFrequencySubBand(fsb));
    }
    else
    {
        logInfo("SUCCESSFULLY SET FSB TO:%u",fsb);
    }

    //setting nw name
    logInfo("HR:Setting netwrok name...",);
    ret_val = dot->setNetworkName(network_name);
    if(ret_val != mDot::MDOT_OK)
    {
        err_logging(dot,"Error in setting the network_name",ret_val);
    }
    else
    {
        logInfo("HR:Successfully set network name:%s\n",network_name.c_str());
    }

    //setting network_passphrase
    logInfo("HR:Setting the network passphrase...");
    ret_val = dot->setNetworkPassphrase(network_passphrase);
    if(ret_val != mDot::MDOT_OK)
    {
        err_logging(dot,"Failed to set network passphrase",ret_val);
    }
    else
    {
        logInfo("Successfully set nw passphrase:%s",network_passphrase.c_str());
    }

    //setting network type
    ret_val = dot->setPublicNetwork(network_type);
    if (ret_val != mDot::MDOT_OK)
    {
        err_logging(dot,"failed to set network type",ret_val);
    }
    else
    {
        logInfo("HR:Successfully set public network type: PRIVATE_MTS");
    }

    // setting the ack val
    if (dot->getAck() != ack)
    {
        logInfo("CHANGING ACK FROM %u TO %u", dot->getAck(), ack);
        if (dot->setAck(ack) != mDot::MDOT_OK)
        {
            logError("FAILED TO SET ACK TO %u", ack);
        }
    }
    // configure network link checks
    // network link checks are a good alternative to requiring the gateway to ACK every packet and should allow a single gateway to handle more Dots
    // check the link every count packets
    // declare the Dot disconnected after threshold failed link checks
    // for count = 3 and threshold = 5, the Dot will ask for a link check response every 5 packets and will consider the connection lost if it fails to receive 3 responses in a row
    // update_network_link_check_config(3, 5);
    uint8_t current_link_check_count = dot->getLinkCheckCount();
    uint8_t current_link_check_threshold = dot->getLinkCheckThreshold();

    if (current_link_check_count != 3)
    {
        logInfo("changing link check count from %u to %u", current_link_check_count, 3);
        if (dot->setLinkCheckCount(3) != mDot::MDOT_OK)
        {
            logError("failed to set link check count to %u", 3);
        }
    }

    if (current_link_check_threshold != 5)
    {
        logInfo("changing link check threshold from %u to %u", current_link_check_threshold, 5);
        if (dot->setLinkCheckThreshold(5) != mDot::MDOT_OK)
        {
            logError("failed to set link check threshold to %u", 5);
        }
    }
    // enable or disable Adaptive Data Rate.  CURRENTLY DISABLED
    dot->setAdr(adr);
    // Configure the join delay
    dot->setJoinDelay(jdelay);
    // save changes to configuration
    logInfo("SAVING CURR NW CONFIG");
    if (!dot->saveConfig())
    {
        logError("FAILED TO SAVE NW CONFIG");
    }

    // setting the tx power to 23 dbm
    uint32_t curr_tx_pwr = dot->getTxPower();
    logInfo("DEFAULT TX PWR ------------------ %u dbm",curr_tx_pwr);
    if(curr_tx_pwr != tx_pwr)
    {
        ret_val = dot->setTxPower(tx_pwr);
        if(ret_val != mDot::MDOT_OK)
        {
            logError("FAILED TO SET THE TX PWR TO:%u dbm",tx_pwr);
        }
        else
        {
            logInfo("SUCCESSFULLY SET THE TX to %u dbm",tx_pwr);
        }
    }
    else
    {
        logInfo("TX PWR is ALREADY SET TO :%u dbm",tx_pwr);
    }

    //setting the DR to max
    uint8_t nwTxDr = 3; //SF7BW125 --> PL size 242B

    if(dot->setTxDataRate(nwTxDr) != mDot::MDOT_OK)
    {
        logError("Failed to set Tx Dr to DR3");
    }

    // display configuration AFTER ALL THE CONFIGURATIONS ARE MADE
    display_config();
  }
  else
  {
    // restore the saved session if the dot woke from deepsleep mode
    // useful to use with deepsleep because session info is otherwise lost when the dot enters deepsleep
    logInfo("RESTRING FROM DEEP SLEEP");
    dot->restoreNetworkSession();
  }

  logInfo("HR:Successfully joined the gateway...\n");



  // formating str to be sent accross to the gateway using lora
//  for(std::string::iterator itr = msg.begin(); itr != msg.end(); ++itr)
//  {
//    data.push_back((uint8_t) *itr); //formating the str to be sent accross gateway to unit_8 format
//  }

  //HR: sending data to gateway
  //uint32_t f_flag = 0;

  uint16_t toa = (uint16_t)(dot->getTimeOnAir(242));

  while(true)
  {
    //uint32_t random_data = rand();
    std::string random_data;
    std::string dev_eui = mts::Text::bin2hexString(dot->getDeviceId()).c_str();

    for (size_t i = 0; i < 242; i++)
    {
        random_data.append("*");
    }

    std::vector<uint8_t> data(random_data.begin(),random_data.end());

    /********* commenting for time being *************
    // dev Toa and EUI push back

    random_data.append("|");
    for(size_t itr = 0; itr < dev_eui.size(); ++itr)
    {
        data.push_back(dev_eui[itr]);
    }

    data.push_back('|');
    uint16_t toa = (uint16_t)(dot->getTimeOnAir(242));

    char toa_str[5];
    sprintf(toa_str,"%u",toa);

    for(size_t itr = 0; itr < strlen(toa_str); ++itr)
    {
        data.push_back(toa_str[itr]);
    }
    ***************************************************/


    // join network if not joined
    if (!dot->getNetworkJoinStatus())
    {
        join_network();
    }

    ret_val = dot->send(data); // ack is enabled by default

    // error checking~~
    if(ret_val != mDot::MDOT_OK)
    {
      err_logging(dot,"Failed to send data to gateway",ret_val);
    }
    else
    {
      //f_flag = 0;
      logInfo("HR:Successfully sent data to gateway....\n");
    }

    //HR: realloc dyanmically for sending tx data
    //data.push_back((random_data >> 8) & 0xFF); // REALLOCATING THE 8 BIT VECTOR TO SUPPORT 32 DATA
    //data.push_back(random_data);

    //logInfo("DUMMY TX DATA:=========================== %lu [0x%04X]", random_data, random_data);
    logInfo("Device ID =============================== %s",dev_eui.c_str());
    //logInfo("DUMMY TX DATA: %lu [0x%04X]", temp_data, temp_data);
    logInfo("CURRENT FEC ============================= %u", dot->getFec());
    logInfo("CURRENT DUTY CYCLE STATUS =============== %u", dot->getDisableDutyCycle());
    logInfo("ToA based on current config ============= %u", toa);


    //wait(3);
    // if going into deepsleep mode, save the session so we don't need to join again after waking up
    // not necessary if going into sleep mode since RAM is retained
    if (deep_sleep)
    {
        logInfo("saving network session to NVM");
        dot->saveNetworkSession();
    }

    // ONLY ONE of the three functions below should be uncommented depending on the desired wakeup method
    //sleep_wake_rtc_only(deep_sleep);
    //sleep_wake_interrupt_only(deep_sleep);
    //sleep_wake_rtc_or_interrupt(deep_sleep);
  }

  //return EXIT_SUCCESS;
  return 0;
}
