#!/usr/bin/python
#TODO: Write me in C please

prefix = '/sys/class/i2c-adapter/i2c-0/0-0073/s3c2440-ts/calibration'
a = open('/etc/pointercal').readline().strip().split(" ")
for i in range(0, len(a)):
    f = open('%s/%s' % (prefix, i), 'w')
    f.write(a[i])
    f.close()
