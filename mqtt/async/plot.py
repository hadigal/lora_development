import os
import sys
import matplotlib.pyplot as plt
import numpy as np

def fileExists(file):
    if os.path.exists(file):
        print("file:%s found\n"%file)
        return 1
    else:
        print("File:%s not found\n"%file)
        return -1

def creatTimeList(file,fileNum):
    timeList = []
    with open(file,'r') as fp:
        for line in fp:
            if fileNum == 1:
                timeList.append(int(line.strip('\n')) - 1542846916)
            else:
                timeList.append(int(line.strip('\n')) - 1542846921)
    return timeList

def plot(filePath):
    retFlag = 0
    retFlag += fileExists(filePath[0])
    retFlag += fileExists(filePath[1])

    if retFlag < 0:
        return -1

    timeList1 = creatTimeList(filePath[0],1)
    timeList2 = creatTimeList(filePath[1],2)

    timeList1 = np.array(timeList1)
    print(timeList1.shape)
    timeList2 = np.array(timeList2)

    y_axis1 = np.array([1 for i in range(len(timeList1))])
    y_axis2 = np.array([1 for i in range(len(timeList2))])

    plt.figure()
    plt.plot(timeList1,y_axis1)
    plt.scatter(timeList1,y_axis1)
    plt.xlabel('TimeList')
    plt.ylabel('constant')
    plt.title('pub msg from marconi')
    plt.show()
    plt.plot(timeList2,y_axis2)
    plt.scatter(timeList2,y_axis2)
    plt.xlabel('TimeList')
    plt.ylabel('constant')
    plt.title('timestamp for Received msg from mdot')
    plt.show()
    
    return 0

if __name__ == '__main__':
    filePath = ['/home/adigal/Desktop/log_mqttPub.txt','/home/adigal/Desktop/log_mqtt_marconi_sub.txt']
    ret = plot(filePath)
    if ret < 0:
        print("Error")
