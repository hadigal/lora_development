import os
import sys
import matplotlib.pyplot as plt
import numpy as np
import re
import time, datetime

# def checkFile(path):
#     if os.path.exists(path):
#         print("Found file:%s"%path)
#         return True
#     else:
#         print("File not found\n")
#         return False

# this is for plotting the 
def plot_predicted(path,t_sub):
    #val,time_val = parserFunct(path)
    toa_dev1,toa_dev2 = parserFunct2(path)
    
    if not toa_dev1 or not toa_dev2:
        return
        
    tSil = []
    
    dc = 0.01
    constant = (1/dc) -1
    
    itr = 0
    
    for toa in val:
        toa_sec = float(float(toa)/1000)
        temp = toa_sec*constant
        tSil.append(temp)

    d_time = []
    
    for dup_time in time_val:
        a = time.strptime(dup_time, "%H:%M:%S:%f")
        d_time.append(datetime.timedelta(hours=a.tm_hour, minutes=a.tm_min, seconds=a.tm_sec).seconds)

    print("Time_dup:{}\n".format(d_time))

    prog_time = []
    itr = d_time[0];
    for itr1 in range(len(d_time)):
        prog_time.append(d_time[itr1])
        if itr1 == 0:
            val = d_time[0]
        prog_time[itr1] -= val
        # if itr1 == 0:
        #     prog_time.append(0)
        # else:
        #     prog_time.append(d_time[itr1] - d_time[itr1 -1])
    print("Time_Prog:{}\n".format(prog_time))

    time_list = []

    # print("LIST",len(t_sub))
    for itr2 in range(len(tSil)):
        if itr2 == 0:
            time_list.append(0)
        else:
            time_list.append(time_list[itr2-1] + tSil[itr2])

    tSil = np.array(tSil)
    time_list = np.array(time_list[:len(t_sub)])
    prog_time2 = np.array(prog_time[:len(t_sub)])
    print("The tSil in secs:\n{}".format(tSil))
    y_axis = np.array([1 for i in range(len(time_list))])

    # plt.figure()
    # plt.plot(tSil, y_axis, color='g',linestyle='None', markersize = 10.0)
    # plt.plot(time_list, y_axis, color='g',linestyle='None', markersize = 10.0)
    # plt.scatter(time_list,y_axis, label='Gateway Rx', color='g')
    plt.scatter(prog_time2,y_axis, label='Gateway Rx(UpLink)', color='g')
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
    regex2 = r'(.*) Duplicate:(.*)'
    regex3 = r'(.*)|INFO|(.*)'
    values = []
    val2 = []
    time_val = []
    itr = 0;
    if fileExists(file) != -1:
        with open(file,'r') as file:
            for line in file:
                if "TOA" in line:
                    # print re.search(regex,line,re.M|re.I)
                    obj = re.search(regex,line,re.M|re.I)
                    res = obj.group(2)
                    values.append(int(res.split()[0]))
                elif "Duplicate" in line:
                    obj = re.search(regex2,line,re.M|re.I)
                    res = obj.group(2)
                    val2.append(res.strip())
                    if val2[itr] == 'no':
                        obj = re.search(regex3,line,re.M|re.I)
                        res = obj.group(1)
                        # print("Time:{}\n".format(res.split('|INFO')))
                        time_val.append(res.split('|INFO')[0])
                        # print("Time:{}\n".format(time_val))
        return values,time_val
    else:
        return False
        
# this is for the log file
def parserFunct2(file,devList):
    regex_dev_toa = r'(.*)TOA:(.*)'
    #regex2 = r'(.*) Duplicate:(.*)'
    #regex3 = r'(.*)|INFO|(.*)'
    dev1_val = []
    dev2_val = []
    #time_val = []
    itr = 0;
    if fileExists(file) != -1:
        with open(file,'r') as file:
            for line in file:
                if devList[0] in line and "TOA" in line:
                    # print re.search(regex,line,re.M|re.I)
                    obj = re.search(regex_dev_toa,line,re.M|re.I)
                    res = obj.group(2)
                    dev1_value.append(int(res.split()[0]))
                elif devList[1] in line and "TOA" in line:
                    obj = re.search(regex_dev_toa,line,re.M|re.I)
                    res = obj.group(2)
                    dev2_val2.append(res.strip()[0])
                    
        return (dev1_val,dev2_val)
    else:
        return False


def creatTimeList(file,devEUIs):
    regex_dev_sub_time = r'(.*)Msg Time:(.*)'
    
    timeList_dev1 = []
    timeList_dev2 = []
    
    itr = 0
    itr2 = 0
    val = 0
    
    with open(file,'r') as fp:
        for line in fp:
        	if devEUIs[0] in line:
        		obj = re.search(regex_dev_sub_time,line,re.M|re.I)
        		res = obj.group(2)
        		
        		timeList_dev1.append(int(res.split()[0]))
		        if itr == 0:
		            val = timeList_dev1[0]
		        #print("dev:{}".format(res.split()[0]))
		        timeList_dev1[itr] -= val
		        itr += 1
		        
	        elif devEUIs[1] in line:
	        	obj = re.search(regex_dev_sub_time,line,re.M|re.I)
	        	res = obj.group(2)
	        	timeList_dev2.append(int(res.split()[0]))
	        	if itr2 == 0:
	        		val = timeList_dev2[0]
	        	timeList_dev2[itr2] -= val
	        	itr2 += 1
            
    #print("Time List1:{}".format(timeList_dev1))
    #print("Time List2:{}".format(timeList_dev2))
    return timeList_dev1,timeList_dev2

def plot(filePath,devEUIs):
    retFlag = 0
    #retFlag += fileExists(filePath[0])
    retFlag += fileExists(filePath[1])

    #if retFlag < 2:
    #    return -1
    
    if retFlag < 1:
        return -1

    #timeList1 = creatTimeList(filePath[0])
    timeList_dev1,timeList_dev2 = creatTimeList(filePath[1],devEUIs)

    #timeList1 = np.array(timeList1)
    timeList_dev1 = np.array(timeList_dev1)
    timeList_dev2 = np.array(timeList_dev2)

    y_axis1 = np.array([3 for i in range(len(timeList_dev1))])
    y_axis2 = np.array([2 for i in range(len(timeList_dev2))])

    fig = plt.figure()
    plt.plot(timeList_dev1[:100], y_axis1[:100], color='r', linestyle='None', markersize = 10.0)
    plt.scatter(timeList_dev1[:100],y_axis1[:100],label = 'mqtt sub(marconi) dev:008000000400982b',color='c')
    # plt.xlabel('Time in secs')
    # plt.ylabel('constant')
    # plt.title('time mqtt pub msg from marconi')
    # plt.show()
    plt.plot(timeList_dev2[:100], y_axis2[:100], color='b', linestyle='None', markersize = 10.0)
    plt.scatter(timeList_dev2[:100],y_axis2[:100], label = 'mqtt sub(marconi) dev:008000000000fe37',color='r')
    #text_str = 'Data Transfer Path: marconi MQTT pub -> Gateway MQTT sub -> Gateway Tx -> mdot Rx\nData Transfer Path: mdot Tx -> Gateway Rx -> Gateway MQTT pub -> marconi MQTT sub\n'
    
    text_str = 'Data Transfer Path: mdot Tx -> Gateway Rx -> Gateway MQTT pub -> marconi MQTT sub\n'
    fig.text(.5,0.01,text_str,wrap=True,ha='center')
    
    
    # plt.xlabel('Time in secs')
    # plt.ylabel('constant')
    # plt.title('timestamp for mqtt sub msg from mdot')
    # plt.show()

    return timeList_dev1,timeList_dev2


if __name__ == '__main__':

	#dev EUI logged to afile on marconi with mqtt sub time stamps
	devEUIs = ['008000000400982b','008000000000fe37']
	
	filePath = ['/home/adigal/Downloads/log_mqttPub.txt','/home/adigal/Downloads/log_mqtt_marconi_sub.txt']
	
	ret = plot(filePath,devEUIs) # mqtt sub/pub plot
	
	plt.title('mqtt sub(marconi) dev:008000000400982b max PL: 113B @ SF8BW125 vs mqtt sub(marconi) dev:008000000000fe37 max PL: 234B @ SF7BW125')
	plt.gca().legend(loc='best')
	plt.gca().yaxis.set_major_locator (plt.NullLocator())
	plt.grid(True)
	plt.show()
