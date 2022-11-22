import argparse
from os.path import exists

parser = argparse.ArgumentParser(description='Simulation of last level cache')

parser.add_argument('--silent', action='store_true',
                    help='Switch to enable silent mode')

parser.add_argument('--trace', type=str, default="trace.log",
                    help='Point the trace file')

args = parser.parse_args()

file_exists = exists(args.trace)

if file_exists:
    f = open(args.trace, "r")
    Lines = f.read().splitlines()
    count = 0

    print("\n%%%%%%%% ECE 585 - Simulation of last level Cache %%%%%%%%\n%%%%%%%% By Adithya Rajesh, Moksh Jadhav, Sanket Patil %%%%%%%%")
    print("%%%%%%%% Start of Simulation %%%%%%%%\n")

    if args.silent:
        print("Running in silent mode\n")
    else:
        print("Running in normal mode\n")
        for line in Lines:
            count += 1
            x = line.split(" ")
            print("Reading line:",count,"of trace file -",x[0],x[1])
            if x[0] == '0':
                print("Operation - Read request from L1 data cache for address",x[1],end='')
            elif x[0] == '1':
                print("Operation - Write request from L1 data cache for address",x[1],end='')
            elif x[0] == '2':
                print("Operation - Read request from L1 instruction cache for address",x[1],end='')
            elif x[0] == '3':
                print("Operation - snooped invalidate command for address",x[1],end='')
            elif x[0] == '4':
                print("Operation - snooped read request for address",x[1],end='')
            elif x[0] == '5':
                print("Operation - snooped write request for address",x[1],end='')
            elif x[0] == '6':
                print("Operation - snooped read with intent to modify request for address",x[1],end='')
            elif x[0] == '7':
                print("Operation - No operation")
            elif x[0] == '8':
                print("Operation - clear the cache and reset all state for address",x[1],end='')
            elif x[0] == '9':
                print("Operation - print contents and state of each valid cache line for address",x[1],end='')
            print("\n")
    f.close()
    print("%%%%%%%% End of Simulation %%%%%%%%")        
else:
    print("Trace file does not exist. Please check !!!")