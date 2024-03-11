#!/usr/bin/env python3
import ctypes
import sys, os, csv
import time
import platform
from utils import *

class CanPkt(ctypes.Structure):
    _fields_ = [
        ("time", ctypes.c_longlong),
        ("canid", ctypes.c_int),
        ("datalen", ctypes.c_int),
        ("data", ctypes.c_char * 8)
    ]

def parse_time_relative(time_relative:str)->int:
    index = time_relative.index(".")
    fraclen = len(time_relative[index+1:])
    if fraclen < 9:
        time_relative += "0" * (9 - fraclen)
    elif fraclen > 9:
        raise NotImplementedError(f"TIME('{time_relative}')의 소수부 자리가 9자를 초과하는 경우에 대해 처리할 수 없음")
    return  int(f"{time_relative[:index]}{time_relative[index+1:]}")

def parse_data(data):
    c_data = []
    hexstr = data.split(':')
    for h in hexstr:
        c_data.append(int(h, 16))
    c_datalen = len(c_data)
    while len(c_data) < 8:
        c_data.append(0)
    return c_data, c_datalen

def parserow(row):
    canid = row['CANID']
    time = row['TIME']
    data = row['DATA']
    
    c_time = parse_time_relative(time)
    c_canid = int(canid)
    c_data, c_datalen = parse_data(data)
    
    return c_time, c_canid, c_data, c_datalen

def export(src, dst):
    with open(dst, 'wb') as o:
        sys.stdout.write(f'Read {src}\n')
        with open(src, 'r', encoding='utf-8') as f:
            sys.stdout.write(f'Parse {src}\n')
            reader = csv.DictReader(f)
            sys.stdout.write(f'Extract to {dst}\n')
            count = 0
            for row in reader:
                c_time, c_canid, c_data, c_datalen = parserow(row)
                pkt = CanPkt()
                pkt.time = c_time
                pkt.canid = c_canid
                pkt.data = b''.join(x.to_bytes(1, byteorder='little') for x in c_data)
                pkt.datalen = c_datalen
                o.write(bytes(pkt))
                count += 1
            sys.stdout.write(f'Extraction completed. Saved {count} packets\n\n')

if __name__ == "__main__":
    src_dir = script_path()
    export_dir = f'{src_dir}/export'
    mkdir_p(export_dir)
    
    if len(sys.argv) <= 1:
        sys.stderr.write(f'Using : {sys.argv[0]} <target> ...\n')
    else:
        for filename in sys.argv[1:]:
            filename = filename.replace('\\', '/')
            signiture = extract_signiture(filename)
            export(filename, f'{export_dir}/{signiture}.replay')

        
    