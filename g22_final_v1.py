import argparse

parser = argparse.ArgumentParser(description='Optional app description')

parser.add_argument('--silent', action='store_true',
                    help='A boolean switch')

args = parser.parse_args()

f = open("trace.log", "r")
#print(f.read())
Lines = f.readlines()

#count = 0

#for line in Lines:
#    count += 1
#    print("Line{}: {}".format(count, line.strip()))
print(args.silent)
if args.silent:
    print("Running in silent mode")
else:
    for line in Lines:
        x = line.split(" ")
        print(x[0],x[1])
        if x[0] == '0':
            print("Read request from L1 data cache for address {}".format(x[1]))
        elif x[0] == '1':
            print("Write request from L1 data cache for address {}".format(x[1]))
        elif x[0] == '2':
            print("Read request from L1 instruction cache for address {}".format(x[1]))
        elif x[0] == '3':
            print("snooped invalidate command for address {}".format(x[1]))
        elif x[0] == '4':
            print("snooped read request for address {}".format(x[1]))
        elif x[0] == '5':
            print("snooped write request for address {}".format(x[1]))
        elif x[0] == '6':
            print("snooped read with intent to modify request for address {}".format(x[1]))
        elif x[0] == '7':
            print("------- for address {}".format(x[1]))
        elif x[0] == '8':
            print("clear the cache and reset all state for address {}".format(x[1]))
        elif x[0] == '9':
            print("print contents and state of each valid cache line for address {}".format(x[1]))
f.close()