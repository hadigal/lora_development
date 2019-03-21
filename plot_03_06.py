import os
import sys
import matplotlib.pyplot as plt
import numpy as np
import re
import time, datetime

"""
File check
"""
# def checkFile(path):
#     if os.path.exists(path):
#         print("Found file:%s"%path)
#         return True
#     else:
#         print("File not found\n")
#         return False

"""
Plot the toa based on mqtt logs
"""
def plot_predicted(path,t_sub1,t_sub2):
	print("t_sub1:\n{}".format(t_sub1))
	print("t_sub2:\n{}".format(t_sub2))
	devEUIs = ['008000000000fe37','008000000400982b']
	dev1,dev2 = parserFunct(path[0])
	toa_dev1,toa_dev2 = parserFunct2(path[1],devEUIs)

	if not toa_dev1 or not toa_dev2 or not dev1 or not dev2:
		return

	tSil1 = []
	tSil2 = []

	dc = 0.01
	constant = (1/dc) -1

	itr = 0

	for toa in toa_dev1:
		toa_sec = float(float(toa)/1000)
		temp = toa_sec*constant
		tSil1.append(temp)

	for toa in toa_dev2:
		toa_sec = float(float(toa)/1000)
		temp = toa_sec*constant
		tSil2.append(temp)

	d_time1 = []
	d_time2 = []

	for dup_time in dev1:
		a = time.strptime(dup_time, "%H:%M:%S:%f")
		d_time1.append(datetime.timedelta(hours=a.tm_hour, minutes=a.tm_min, seconds=a.tm_sec).seconds)

	for dup_time in dev2:
		a = time.strptime(dup_time, "%H:%M:%S:%f")
		d_time2.append(datetime.timedelta(hours=a.tm_hour, minutes=a.tm_min, seconds=a.tm_sec).seconds)

	print("Time_dup1:{}\n".format(d_time1))
	print("Time_dup2:{}\n".format(d_time2))

	prog_time1 = []
	itr1 = d_time1[0]

	prog_time2 = []
	itr2 = d_time2[0]

	# this to convert the duplicate flag: NO packet timestamps to plot grap in progressive manner
	for itr1 in range(len(d_time1)):
		prog_time1.append(d_time1[itr1])
		if itr1 == 0:
			val = d_time1[0]
		prog_time1[itr1] -= val
        # if itr1 == 0:
        #     prog_time.append(0)
        # else:
        #     prog_time.append(d_time[itr1] - d_time[itr1 -1])

	print("Time_Prog1:{}\n".format(prog_time1))

	for itr2 in range(len(d_time2)):
		prog_time2.append(d_time2[itr2])
		if itr2 == 0:
			val = d_time2[0]
		prog_time2[itr2] -= val
        # if itr1 == 0:
        #     prog_time.append(0)
        # else:
        #     prog_time.append(d_time[itr1] - d_time[itr1 -1])
	print("Time_Prog2:{}\n".format(prog_time2))

	time_list1 = []

	for itr2 in range(len(tSil1)):
		if itr2 == 0:
			time_list1.append(0)
		else:
			time_list1.append(time_list1[itr2-1] + tSil1[itr2])

	time_list2 = []

	for itr2 in range(len(tSil2)):
		if itr2 == 0:
			time_list2.append(0)
		else:
			time_list2.append(time_list2[itr2-1] + tSil2[itr2])

	tSil1 = np.array(tSil1)
	tSil2 = np.array(tSil2)

	time_list1 = np.array(time_list1[:100])
	time_list2 = np.array(time_list2[:100])

	prog_time1 = np.array(prog_time1[:100])
	prog_time2 = np.array(prog_time2[:100])

	print("The tSil1 in secs:\n{}".format(tSil1))
	print("The tSil2 in secs:\n{}".format(tSil2))

	y_axis1 = np.array([1 for i in range(len(time_list1))])
	y_axis2 = np.array([2 for i in range(len(time_list2))])

	#print("len(prog_time1),len(prog_time1),len(y_axis1):",len(prog_time1),len(prog_time2),len(y_axis1),len(y_axis2))

    # plt.figure()
    # plt.plot(tSil, y_axis, color='g',linestyle='None', markersize = 10.0)
    # plt.plot(time_list, y_axis, color='g',linestyle='None', markersize = 10.0)
    # plt.scatter(time_list,y_axis, label='Gateway Rx', color='g')
	plt.scatter(prog_time1[:100],y_axis1, label='Gateway Rx(UpLink) Dev:008000000000fe37', color='g')
	plt.scatter(prog_time2[:100],y_axis2, label='Gateway Rx(UpLink) Dev:008000000400982b', color='b')
	plt.xlabel('Time in secs')

def fileExists(file):
    if os.path.exists(file):
        print("file:%s found\n"%file)
        return 1
    else:
        print("File:%s not found\n"%file)
        return -1

def parserFunct(file):
	regex2 = r'(.*) Duplicate:(.*)'
	regex3 = r'(.*)|INFO|(.*)'
	values = []
	val2 = []
	time_val = []
	itr = 0

	if fileExists(file) != -1:
		with open(file,'r') as file:
			for line in file:
				if "00-80-00-00-00-00-fe-37" in line and "Duplicate" in line:
					obj = re.search(regex2,line,re.M|re.I)
					res = obj.group(2)
					val2.append(res.strip())
					if val2[itr] == 'no':
						obj = re.search(regex3,line,re.M|re.I)
						res = obj.group(1)
						# print("Time:{}\n".format(res.split('|INFO')))
						time_val.append(res.split('|INFO')[0])
				elif "00-80-00-00-04-00-98-2b" in line and "Duplicate" in line:
					obj = re.search(regex2,line,re.M|re.I)
					res = obj.group(2)
					val2.append(res.strip())
					if val2[itr] == 'no':
						obj = re.search(regex3,line,re.M|re.I)
						res = obj.group(1)
						# print("Time:{}\n".format(res.split('|INFO')))
						values.append(res.split('|INFO')[0])
		return time_val,values
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
                    dev1_val.append(int(res.split()[0]))
                elif devList[1] in line and "TOA" in line:
                    obj = re.search(regex_dev_toa,line,re.M|re.I)
                    res = obj.group(2)
                    dev2_val.append(res.strip()[0])

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

	if retFlag < 1:
		return -1

	#timeList1 = creatTimeList(filePath[0])
	timeList_dev1,timeList_dev2 = creatTimeList(filePath[1],devEUIs)

	#timeList1 = np.array(timeList1)
	timeList_dev1 = np.array(timeList_dev1)
	timeList_dev2 = np.array(timeList_dev2)

	y_axis1 = np.array([3 for i in range(len(timeList_dev1))])
	y_axis2 = np.array([4 for i in range(len(timeList_dev2))])

	fig = plt.figure()
	plt.plot(timeList_dev1[:100], y_axis1[:100], color='r', linestyle='None', markersize = 10.0)
	plt.scatter(timeList_dev1[:100],y_axis1[:100],label = 'mqtt sub(marconi) Dev:008000000000fe37',color='c')
	plt.plot(timeList_dev2[:100], y_axis2[:100], color='b', linestyle='None', markersize = 10.0)
	plt.scatter(timeList_dev2[:100],y_axis2[:100], label = 'mqtt sub(marconi) Dev:008000000400982b',color='r')
	#text_str = 'Data Transfer Path: marconi MQTT pub -> Gateway MQTT sub -> Gateway Tx -> mdot Rx\nData Transfer Path: mdot Tx -> Gateway Rx -> Gateway MQTT pub -> marconi MQTT sub\n'
	plt.title('Gateway Rx Time vs mqtt sub(marconi) time for dev:008000000400982b max PL: 113B @ SF8BW125 vs mqtt sub(marconi) dev:008000000000fe37 max PL: 234B @ SF7BW125')
	text_str = 'Data Transfer Path: mdot Tx -> Gateway Rx -> Gateway MQTT pub -> marconi MQTT sub\n'
	fig.text(.5,0.01,text_str,wrap=True,ha='center')
	return timeList_dev1,timeList_dev2


if __name__ == '__main__':

	filePath = ['/home/adigal/Documents/lora-network-server.log','/home/adigal/Documents/mqtt_sub_log.txt']

	#ret1,ret2 = plot(filePath)
    # if ret == -1:
    #     print("Error")

	#path = r"/home/hadigal/Desktop/test_lora_logs.log"
	#plt.title('Gateway Rx Time vs actual time to mqtt sub tx pkt of size 11B@SF7BW125 from mdot vs mqtt pub pkt of 10B size to mdot')
	#dev EUI logged to afile on marconi with mqtt sub time stamps
	devEUIs = ['008000000000fe37','008000000400982b']
	#filePath = ['/home/adigal/Downloads/log_mqttPub.txt','/home/adigal/Downloads/log_mqtt_marconi_sub.txt']
	#plt.title('Gateway Rx Time vs mqtt sub(marconi) for dev:008000000400982b max PL: 113B @ SF8BW125 vs mqtt sub(marconi) dev:008000000000fe37 max PL: 234B @ SF7BW125')
	ret1,ret2 = plot(filePath,devEUIs) # mqtt sub/pub plot
	plot_predicted(filePath,ret1,ret2)
	plt.gca().legend(loc='best')
	plt.gca().yaxis.set_major_locator (plt.NullLocator())
	plt.grid(True)
	plt.show()
