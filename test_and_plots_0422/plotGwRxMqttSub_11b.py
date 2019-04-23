import os
import sys
import matplotlib.pyplot as plt
import numpy as np
import re
import time, datetime
import dateutil.parser as dp

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
Plot the gw rx timestamps and toa timestamps
"""
def plot_predicted(path,t_sub1,t_sub2,yax,sf37,sf36):
	devEUIs = ['00-80-00-00-00-00-fe-37','00-80-00-00-00-00-fe-36']

	# Gateway RX
	dev1,dev2 = parserFunct(path[0])

	d_time1 = []
	d_time2 = []

	for dup_time1 in dev1:
		d_time1.append(int(dup_time1))

	for dup_time2 in dev2:
		d_time2.append(int(dup_time2))


	print("Time_dup1:{}\n".format(d_time1))
	print("Time_dup2:{}\n".format(d_time2))

	# save mqtt gateway rx time in epoch format
	prog_time1 = []
	#itr1 = d_time1[0]
	prog_time2 = []
	#itr2 = d_time2[0]

	# this to convert the duplicate flag: NO packet timestamps to plot grap in progressive manner
	for itr1 in range(len(d_time1)):
		prog_time1.append(d_time1[itr1])
		if itr1 == 0:
			val1 = d_time1[0]
		prog_time1[itr1] -= val1


	print("Time_Prog1:{}\n".format(prog_time1))

	for itr2 in range(len(d_time2)):
		prog_time2.append(d_time2[itr2])
		if itr2 == 0:
			val = d_time2[0]
		prog_time2[itr2] -= val

	print("Time_Prog2:{}\n".format(prog_time2))

	time_list1 = []

	prog_time1 = np.array(prog_time1[:20])
	prog_time2 = np.array(prog_time2[:20])

	y_axis1 = np.array([yax[0] for i in range(len(prog_time1))])
	y_axis2 = np.array([yax[1] for i in range(len(prog_time2))])

	lb1 = 'Gateway Rx(UpLink) Dev:008000000000fe37' + ' ' + sf37
	lb2 = 'Gateway Rx(UpLink) Dev:008000000000fe36' + ' ' + sf36

	#print("len(prog_time1),len(prog_time1),len(y_axis1):",len(prog_time1),len(prog_time2),len(y_axis1),len(y_axis2))

    # plt.figure()
    # plt.plot(tSil, y_axis, color='g',linestyle='None', markersize = 10.0)
    # plt.plot(time_list, y_axis, color='g',linestyle='None', markersize = 10.0)
    # plt.scatter(time_list,y_axis, label='Gateway Rx', color='g')
	plt.rcParams.update({'font.size': 16})
	plt.scatter(prog_time1[:20],y_axis1, label=lb1, color='g')
	#plt.scatter(prog_time2[:20],y_axis2, label='Gateway Rx(UpLink) Dev:0080000004009802', color='b')
	if 'NULL' not in sf36:
		plt.scatter(prog_time2[:20],y_axis2, label=lb2, color='b')

	plt.rcParams.update({'font.size': 14})
	plt.xlabel('Time in secs')
	plt.ylabel('DR0 --> DR4')

def fileExists(file):
    if os.path.exists(file):
        print("file:%s found\n"%file)
        return 1
    else:
        print("File:%s not found\n"%file)
        return -1


def parserFunct(file):
	regex2 = r'(.*)Device ID:(.*)'
	regex3 = r'(.*)Msg Time: (.*)'
	dev2 = []
	dev1 = []

	val1 = []
	val2 = []
	itr1 = 0
	itr2 = 0

	if fileExists(file) != -1:
		with open(file,'r') as file:
			for line in file:
				if "00-80-00-00-00-00-fe-37" in line:
					obj = re.search(regex3,line,re.M|re.I)
					#print(obj);
					res = obj.group(2);
					# print("res.split()[0]:{}".format(res.split()[0]))
					tp = dp.parse(res.split()[0])
					t_sec = tp.strftime('%s')
					dev1.append(t_sec)
					itr1 += 1
				elif "00-80-00-00-00-00-fe-36" in line:
					obj = re.search(regex3,line,re.M|re.I)
					#print(obj);
					res = obj.group(2);
					# print("res.split()[0]:{}".format(res))
					tp = dp.parse(res.split()[0])
					t_sec = tp.strftime('%s')
					dev2.append(t_sec)
					itr2 += 1
		return dev1,dev2
	else:
		return False

'''
# this is for the toa item in the marconi log file
'''
def parserFunctToa(file,devList):

	regex_dev_toa = r'(.*)TOA:(.*)'

	dev1_val = []
	dev2_val = []
	#time_val = []
	itr = 0;
	if fileExists(file) != -1:
		with open(file,'r') as file:
			for line in file:
				if devList[0] in line and "TOA" in line:
					obj = re.search(regex_dev_msg_rx,line,re.M|re.I)
					res = obj.group(2)
					dev1_val.append(int(res.split()[0]))
				elif devList[1] in line and "TOA" in line:
					#elif devList[1] in line and "TOA" in line:
					#obj = re.search(regex_dev_toa,line,re.M|re.I)
					obj = re.search(regex_dev_msg_rx,line,re.M|re.I)
					res = obj.group(2)
					dev2_val.append(int(res.split()[0]))
		return (dev1_val,dev2_val)
	else:
		return False

'''
To plot the mqtt_sub timestamps
'''
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

    return timeList_dev1,timeList_dev2

def plot(filePath,devEUIs,yax,sf37,sf36):
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

	y_axis1 = np.array([yax[0] for i in range(len(timeList_dev1))])
	y_axis2 = np.array([yax[1] for i in range(len(timeList_dev2))])

	lb1 = 'mqtt sub(marconi) Dev:008000000000fe37' + ' ' + sf37
	lb2 = 'mqtt sub(marconi) Dev:008000000000fe36' + ' ' + sf36
	#fig = plt.figure(figsize=(18, 16), dpi= 100, facecolor='w', edgecolor='k')
	# fig = plt.figure()

	#fig.canvas.draw()
	plt.plot(timeList_dev1[:20], y_axis1[:20], color='r', linestyle='None', markersize = 10.0)
	plt.scatter(timeList_dev1[:20],y_axis1[:20],label = lb1 ,color='c')
	plt.plot(timeList_dev2[:20], y_axis2[:20], color='b', linestyle='None', markersize = 10.0)
	#plt.scatter(timeList_dev2[:20],y_axis2[:20], label = 'mqtt sub(marconi) Dev:0080000004009802',color='r')
	if 'NULL' not in sf36:
		plt.scatter(timeList_dev2[:20],y_axis2[:20], label = lb2,color='r')

	return timeList_dev1,timeList_dev2


if __name__ == '__main__':

	filePath1 = ['/home/iot/Desktop/testJuypter/test0422/gw_tmst_0422_dr0dr1_11b.txt','/home/iot/Desktop/testJuypter/test0422/log_mqtt_marconi_sub_0422_dr0dr1_11b.txt']
	filePath2 = ['/home/iot/Desktop/testJuypter/test0422/gw_tmst_0422_dr2dr3_11b.txt','/home/iot/Desktop/testJuypter/test0422/log_mqtt_marconi_sub_0422_dr2dr3_11b.txt']
	filePath3 = ['/home/iot/Desktop/testJuypter/test0422/gw_tmst_0422_dr4_11b.txt','/home/iot/Desktop/testJuypter/test0422/log_mqtt_marconi_sub_0422_dr4_11b.txt']

	devEUIs = ['00-80-00-00-00-00-fe-37','00-80-00-00-00-00-fe-36']
	axis_sub = [1,3]
	axis_gw = [2,4]

	fig = plt.figure()
	ret1,ret2 = plot(filePath1,devEUIs,[1,3],'SF10','SF9') # mqtt sub/pub plot
	plot_predicted(filePath1,ret1,ret2,[2,4],'SF10','SF9')
	ret1,ret2 = plot(filePath2,devEUIs,[7,5],'SF7','SF8') # mqtt sub/pub plot
	plot_predicted(filePath2,ret1,ret2,[8,6],'SF7','SF8')
	ret1,ret2 = plot(filePath3,devEUIs,[9,0],'SF8BW500','NULL') # mqtt sub/pub plot
	plot_predicted(filePath3,ret1,ret2,[10,0],'SF8BW500','NULL')

	plt.rcParams.update({'font.size': 12})
	plt.title('Gateway Rx Time vs mqtt sub(marconi) time for dev:008000000000fe36 and dev:008000000000fe37 @11B PL operating at different Data Rates [DR0-DR4]')
	plt.rcParams.update({'font.size': 16})
	text_str = 'Data Transfer Path: mdot Tx -> Gateway Rx -> Gateway MQTT pub -> marconi MQTT sub\n'
	fig.text(.5,0.01,text_str,wrap=True,ha='center')

	plt.gca().legend(loc='best')
	plt.gca().yaxis.set_major_locator (plt.NullLocator())
	plt.grid(True)
	plt.show()
