import os
import sys
import matplotlib.pyplot as plt
import numpy as np
import re

# def checkFile(path):
#     if os.path.exists(path):
#         print("Found file:%s"%path)
#         return True
#     else:
#         print("File not found\n")
#         return False

def plot_predicted(path,t_sub):
    val = parserFunct(path)
    if not val:
        return
    tSil = []
    dc = 0.01
    constant = (1/dc) -1
    itr = 0
    for toa in val:
        toa_sec = float(float(toa)/1000)
        temp = toa_sec*constant
        tSil.append(temp)

    time_list = []

    # print("LIST",len(t_sub))
    for itr2 in range(len(tSil)):
        if itr2 == 0:
            time_list.append(0)
        else:
            time_list.append(time_list[itr2-1] + tSil[itr2])

    tSil = np.array(tSil)
    time_list = np.array(time_list[:len(t_sub)])
    print("The tSil in secs:\n{}".format(tSil))
    y_axis = np.array([1 for i in range(len(time_list))])

    # plt.figure()
    # plt.plot(tSil, y_axis, color='g',linestyle='None', markersize = 10.0)
    # plt.plot(time_list, y_axis, color='g',linestyle='None', markersize = 10.0)
    plt.scatter(time_list,y_axis, label='Gateway Rx', color='g')
    plt.xlabel('Time in secs')

def fileExists(file):
    if os.path.exists(file):
        print("file:%s found\n"%file)
        return 1
    else:
        print("File:%s not found\n"%file)
        return -1

def parserFunct(file):
    regex = r'(.*) TOA:(.*)'
    values = []
    if fileExists(file) != -1:
        with open(file,'r') as file:
            for line in file:
                if "TOA" in line:
                    # print re.search(regex,line,re.M|re.I)
                    obj = re.search(regex,line,re.M|re.I)
                    res = obj.group(2)
                    values.append(int(res.split()[0]))
        return values
    else:
        return False

def creatTimeList(file,fileNum):
    timeList = []
    itr = 0
    val = 0
    with open(file,'r') as fp:
        for line in fp:
            if fileNum == 1:
                # timeList.append(int(line.strip('\n')) - 1544758931)
                timeList.append(int(line.strip('\n')))
                if itr == 0:
                    val = timeList[0]
                timeList[itr] -= val
            else:
                # timeList.append(int(line.strip('\n')) - 1544758931)
                timeList.append(int(line.strip('\n')))
                if itr == 0:
                    val = timeList[0]
                timeList[itr] -= val
            itr += 1
    return timeList

def plot(filePath):
    retFlag = 0
    retFlag += fileExists(filePath[0])
    retFlag += fileExists(filePath[1])

    if retFlag < 2:
        return -1

    timeList1 = creatTimeList(filePath[0],1)
    timeList2 = creatTimeList(filePath[1],2)

    timeList1 = np.array(timeList1)
    timeList2 = np.array(timeList2)

    y_axis1 = np.array([3 for i in range(len(timeList1))])
    y_axis2 = np.array([2 for i in range(len(timeList2))])

    fig = plt.figure()
    # plt.plot(timeList1, y_axis1, color='r', linestyle='None', markersize = 10.0)
    plt.scatter(timeList1,y_axis1,label = 'mqtt pub',color='r')
    # plt.xlabel('Time in secs')
    # plt.ylabel('constant')
    # plt.title('time mqtt pub msg from marconi')
    # plt.show()
    # plt.plot(timeList2, y_axis2, color='b', linestyle='None', markersize = 10.0)
    plt.scatter(timeList2,y_axis2, label = 'mqtt sub',color='b')
    text_str = 'Data Transfer Path for mqtt pub: marconi MQTT pub -> Gateway MQTT sub -> Gateway Tx -> mdot Rx\nData Transfer Path for mqtt sub: mdot Tx -> Gateway Rx -> Gateway MQTT pub -> marconi MQTT sub\n'
    fig.text(.5,0.01,text_str,wrap=True,ha='center')
    # plt.xlabel('Time in secs')
    # plt.ylabel('constant')
    # plt.title('timestamp for mqtt sub msg from mdot')
    # plt.show()

    return timeList2


if __name__ == '__main__':
    filePath = ['/home/hadigal/Desktop/log_mqttPub.txt','/home/hadigal/Desktop/log_mqtt_marconi_sub.txt']
    ret = plot(filePath)
    # if ret == -1:
    #     print("Error")
    path = r"/home/hadigal/Desktop/test_lora_logs.log"
    plot_predicted(path,ret)
    plt.title('Gateway Rx Time vs actual time to mqtt sub tx pkt of size 11B@SF7BW125 from mdot vs mqtt pub pkt of 10B size to mdot')
    plt.gca().legend(loc='best')
    plt.gca().yaxis.set_major_locator (plt.NullLocator())
    plt.grid(True)
    plt.show()
