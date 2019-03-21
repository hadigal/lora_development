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
	toaArr = [89,189,368]
	toaArr = np.array(toaArr)
	
	dataRate = ['DR1','DR2','DR3']
	dataRate = np.array(dataRate)
	
	fig = plt.figure()
	plt.scatter(dataRate,toaArr,color='c')
	plt.plot(dataRate,toaArr)
    
if __name__ == '__main__':
    #filePath = ['/home/hadigal/Desktop/log_mqttPub.txt','/home/hadigal/Desktop/log_mqtt_marconi_sub.txt']
    ret = plot()
    # if ret == -1:
    #	print("Error")
    #	return
    
    plt.title('Time on Air for DR1-DR3 at Max Payload size')
    plt.xlabel('Data Rate between DR1-DR3')
    plt.ylabel('Time in msecs')
    plt.grid(True)
    plt.show()
    
    
   
    

	
