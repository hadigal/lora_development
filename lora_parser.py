import re
import os

def checkFile(path):
    print path
    if os.path.exists(path):
        print "Found file:%s"%path
        return True
    else:
        print "File not found\n"
        return False

def parserFunct(file):
    regex = r'(.*) TOA:(.*)'
    values = []
    if checkFile(file):
        with open(file,'r') as file:
            for line in file:
                if "TOA" in line:
                    # print re.search(regex,line,re.M|re.I)
                    obj = re.search(regex,line,re.M|re.I)
                    res = obj.group(2)
                    values.append(int(res.split()[0]))

                # if "TOA" == line:
                #     print "line"

                # print line
        print values
        return values
    else:
        return False

if __name__ == '__main__':
    path = r"/home/hadigal/Desktop/test_lora_logs.log"
    checkFile(path)
    val = parserFunct(path)
    tSil = []
    dc = 0.01
    constant = (1/dc) -1
    for toa in val:
        toa_sec = float(float(toa)/1000)
        print toa_sec
        temp = toa_sec*constant
        tSil.append(temp)
    print val
    print tSil
