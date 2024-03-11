#!/usr/bin/env python
# coding: utf-8
import os, sys, csv
import statistics
from decimal import Decimal, getcontext

export_directory = os.environ.get('EXPORT_ANALYZER')
if not export_directory:
    script_path = os.path.abspath(sys.argv[0]).replace('\\','/')
    script_directory = os.path.dirname(script_path).replace('\\','/')
    export_directory = script_directory

result_directory = f'{export_directory}/analyze'

def mkdir_p(directory):
    if not os.path.exists(directory):
        os.mkdir(directory)

def read_csv(filename:str, limit = None):
    items = []
    with open(filename, 'r', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        for i, row in enumerate(reader):
            items.append(row)
            if limit is not None and i >= limit:
                break
    return items

def analyze_can(csvitems:list):
    containers = {}
    total = 0
    for i, item in enumerate(csvitems):
        canid = item['CANID']
        num = Decimal(item['TIME'])
        total += 1

        if canid not in containers:
            containers[canid] = {
                'lasttime' : num,
                'intervals' : [],
                'min-interval' : None,
                'max-interval' : None,
                'sum' : Decimal(0),
                'count' : 0
            }
        container = containers[canid]

        interval = num - container['lasttime']
        container['lasttime'] = num
        container['sum'] += interval
        container['count'] += 1
        container['intervals'].append(interval)
        if interval > 0:
            imin = container['min-interval']
            imax = container['max-interval']
            if imin is None or interval < imin:
                container['min-interval'] = interval
            if imax is None or interval > imax:
                container['max-interval'] = interval
        
    infomation = {
        'total' : total,
    }
    return containers, infomation

def write_statistic_interval_csv(filename:str, containers:list, infomation:dict):
    header = ['CANID', 'COUNT', 'MIN', 'MAX', 'AVE', 'MEDIAN']
    with open(filename, 'w', encoding='utf-8', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=header)
        writer.writeheader()
        for key, can_result in containers.items():
            data = {}
            data['CANID'] = key
            data['MIN'] = '{:.9f}'.format(can_result['min-interval'])
            data['MAX'] = '{:.9f}'.format(can_result['max-interval'])
            data['MEDIAN'] = '{:.9f}'.format(statistics.median(can_result['intervals']))
            # data['SUM'] = str(can_result['sum'])
            data['AVE'] = '{:.9f}'.format(can_result['sum'] / can_result['count'])
            data['COUNT'] = can_result['count']
            writer.writerow(data)
        writer.writerow({'CANID': 'total', 'COUNT' : infomation['total']})

if __name__ == "__main__":
    mkdir_p(f'{result_directory}')
    filenames = []

    for filename in sys.argv[1:]:
        filename = filename.replace('\\', '/')
        _, file = os.path.split(filename)
        name, ext = os.path.splitext(file)

        rcright = read_csv(filename)
        containers, infomation = analyze_can(rcright)
        write_statistic_interval_csv(f"{result_directory}/{name}.csv", containers, infomation)
