import os
import sys
import matplotlib.pyplot as plt
import numpy as np
import re
import time, datetime


def plot():
	"""
	if os.path.exists(file):
		print("file:%s found\n"%file)
	else:
		print("File:%s not found\n"%file)
		return -1
	"""
	toaArr = [32,93,113,379,166]
	toaArr = np.array(toaArr)

	dataRate = ['DR0','DR1','DR2','DR3','DR4']
	dataRate = np.array(dataRate)

	fig = plt.figure()
	plt.scatter(dataRate,toaArr,color='c')
	plt.plot(dataRate,toaArr)
	text_str = '1)SF8BW500 - DR4 MaxPL:242B 2)SF7BW125 - DR3 MaxPL:242B 3)SF8BW125 - DR2 MaxPL:125B 4)SF9BW125 - DR1 MaxPL:53B 5)SF10BW125 - DR0 MaxPL:11B\n'
	fig.text(.5,0.01,text_str,wrap=True,ha='center')

if __name__ == '__main__':
	#filePath = ['/home/hadigal/Desktop/log_mqttPub.txt','/home/hadigal/Desktop/log_mqtt_marconi_sub.txt']
	ret = plot()
	# if ret == -1:
	#	print("Error")
	#	return
	plt.title('Time on Air for DR0-DR4 at Max Payload size')
	plt.xlabel('Data Rate between DR0-DR4')
	plt.ylabel('Time in msecs')
	plt.grid(True)
	plt.show()
