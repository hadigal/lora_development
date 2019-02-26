/*******************************************************************************
* File: loraEvent.h
* Description: This header file supports radio events of LoRa Communication
* Author: Hrishikesh Adigal
* Date: 11/02/2018
*******************************************************************************/
#ifndef __LORA_EVENT_H_
#define __LORA_EVENT_H_

#include "mbed.h"
#include "mDot.h"
#include "ChannelPlans.h"
#include "MTSLog.h"
#include "MTSText.h"
#include "mDotEvent.h"

extern mDot *dot;

class LoraEvent: public mDotEvent
{
    public:LoraEvent(){}
    virtual ~LoraEvent(){}
    
    // creating loopback for MacEvents
    virtual void MacEvent(LoRaMacEventFlags* flags, LoRaMacEventInfo* info)
    {
        
        if(mts::MTSLog::getLogLevel() == mts::MTSLog::TRACE_LEVEL)
        {
            // handling erros
            std::string msg = "OK";
            switch (info->Status)
            {
                case LORAMAC_EVENT_INFO_STATUS_ERROR:
                    msg = "EVENT ERROR";
                    break;
                case LORAMAC_EVENT_INFO_STATUS_TX_TIMEOUT:
                    msg = "TX TIMEOUT ERROR";
                    break;
                case LORAMAC_EVENT_INFO_STATUS_RX_TIMEOUT:
                    msg = "RX TIMEOUT ERROR";
                    break;
                case LORAMAC_EVENT_INFO_STATUS_JOIN_FAIL:
                    msg = "JOIN FAIL";
                    break;
                case LORAMAC_EVENT_INFO_STATUS_DOWNLINK_FAIL:
                    msg = "DOWNLINK FAIL";
                    break;
                case LORAMAC_EVENT_INFO_STATUS_ADDRESS_FAIL:
                    msg = "ADDR FAIL";
                    break;
                case LORAMAC_EVENT_INFO_STATUS_MIC_FAIL:
                    msg = "MIC FAIL";
                    break;
                default:
                    break;
            }
            // printing message based on lora event
            logTrace("Event: %s", msg.c_str());
            logTrace(":::::::::: FLAGS :::::::::: TX: %d RX: %d RX_DATA: %d RX_SLOT: %d LINK_CHECK: %d JOIN_ACCEPT: %d", flags->Bits.Tx, flags->Bits.Rx, flags->Bits.RxData, flags->Bits.RxSlot, flags->Bits.LinkCheck, flags->Bits.JoinAccept);
            logTrace(":::::::::: INFO :::::::::: STATUS: %d ACK: %d # of TX Retries: %d Tx_@_DR: %d Rx_Port: %d RxBUFFSize: %d Rx_RSSI: %d Rx_SNR: %d Energy: %d Margin: %d Gateways: %d",
                     info->Status, info->TxAckReceived, info->TxNbRetries, info->TxDatarate, info->RxPort, info->RxBufferSize,
                     info->RxRssi, info->RxSnr, info->Energy, info->DemodMargin, info->NbGateways);
        }
        time_t secs = time(NULL);
        uint8_t curr_tx_dr = dot->getTxDataRate();
        logInfo("Current Tx DR:%u",curr_tx_dr);
        if(flags->Bits.Rx)
        {
            logDebug("RX DATA SIZE:%d Bytes",info->RxBufferSize);
            // printing the data received in buffer if size is > 0 bytes
            if (info->RxBufferSize > 0)
            {
                std::string rx((const char*)info->RxBuffer, info->RxBufferSize);
                logInfo("RX DATA ---------------- : %s [%s]\r", rx.c_str(), mts::Text::bin2hexString(info->RxBuffer, info->RxBufferSize).c_str());
                logInfo("Time data was rxd. = %s", ctime(&secs));
                if(curr_tx_dr == atoi(rx.c_str()))
                {
                    logInfo("CURRENT TX DR:%u New TX DR:%u are equal... No changes required\n",curr_tx_dr,atoi(rx.c_str()));
                }
                else
                {
                    //uint8_t nwTxDr = atoi(rx.c_str()); -----------------> Modify this later!!!
                    uint8_t nwTxDr = 3;
                    logInfo("Chaning the Current DR:%u to DR:%u\n",curr_tx_dr,atoi(rx.c_str()));
                    dot->setTxDataRate(nwTxDr);
                }
            }
        }
    }
};
#endif
    
    

