#!/usr/bin/python
def calculate(time_min, time_s, count):
    total_s = time_min * 60 + time_s;
    result = float(count)/float(total_s)
    return result

###for stress test
#print("AONE: %f" % (calculate(178, 49, 50286)))
#print("E: %f" % (calculate(172, 17, 93261)))
#print("E: %f" % (calculate(159, 48, 86224)))
#print("F: %f" % (calculate(167, 46, 92458)))
