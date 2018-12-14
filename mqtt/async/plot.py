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

    y_axis1 = np.array([1 for i in range(len(timeList1))])
    y_axis2 = np.array([1 for i in range(len(timeList2))])

    plt.figure()
    plt.plot(timeList1,y_axis1,color='r', linestyle='None', markersize = 10.0)
    plt.scatter(timeList1,y_axis1)
    plt.xlabel('Time in secs')
    plt.ylabel('constant')
    plt.title('time mqtt pub msg from marconi')
    plt.show()
    plt.plot(timeList2,y_axis2,color='r', linestyle='None', markersize = 10.0)
    plt.scatter(timeList2,y_axis2)
    plt.xlabel('Time in secs')
    plt.ylabel('constant')
    plt.title('timestamp for mqtt sub msg from mdot')
    plt.show()

    return 0

def plot_predicted(path):
    val = parserFunct(path)
    tSil = []
    dc = 0.01
    constant = (1/dc) -1
    for toa in val:
        toa_sec = float(float(toa)/1000)
        temp = toa_sec*constant
        tSil.append(temp)

    tSil = np.array(tSil)
    print("The tSil in secs:\n{}".format(tSil))
    y_axis = np.array([1 for i in range(len(tSil))])
    plt.figure()
    plt.plot(tSil,y_axis,color='r', linestyle='None', markersize = 10.0)
    plt.scatter(tSil,y_axis)
    plt.xlabel('Time in secs')
    plt.ylabel('constant')
    plt.title('time silence between each rx pkt at gateway')
    plt.show()


if __name__ == '__main__':
    filePath = ['/home/hadigal/Desktop/log_mqttPub.txt','/home/hadigal/Desktop/log_mqtt_marconi_sub.txt']
    ret = plot(filePath)
    if ret < 0:
        print("Error")
    path = r"/home/hadigal/Desktop/test_lora_logs.log"
    plot_predicted(path)
