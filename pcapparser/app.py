import sys, os
import time
import csv, json
import pcapjson
from threading import Thread

# tshark_path = "C:\\Program Files\\Wireshark\\tshark.exe"
# tshark = "C:\\\"Program Files\"\\Wireshark\\tshark.exe"
tshark = "tshark"

waitcurse_index = 0
waitcurse_str = "|/-\\"

script_path = os.path.abspath(sys.argv[0]).replace('\\','/')
script_directory = os.path.dirname(script_path).replace('\\','/')

export_directory = os.environ.get('EXPORT_PCAPPARSER')
if not export_directory:
    export_directory = script_directory
    
result_directory = f'{export_directory}/json'
tmp_directory = f'{export_directory}/csv'

def mkdir_p(directory):
    if not os.path.exists(directory):
        os.mkdir(directory)

def waitcurse(th:Thread):
    global waitcurse_index
    while th.is_alive():
        sys.stdout.write(waitcurse_str[waitcurse_index % len(waitcurse_str)])
        sys.stdout.write("\r")
        waitcurse_index += 1
        time.sleep(0.3)

def export(target:str, signiture:str):
    target_json = f'{tmp_directory}/{signiture}.json'
    target_csv = f'{result_directory}/{signiture}.csv'
    
    sys.stdout.write(f'Target: \'{target}\'\n')
    sys.stdout.write('Convert to JSON\n')
    errorcode = os.system(f'{tshark} -r {target} -T json >"{target_json}"')
    if errorcode != 0:
        sys.stderr.write('tshark process fail\n')
        sys.stderr.write('stop\n')
        return 1

    header = pcapjson.Headers()
    header += ['CANID']
    header += ['DATALEN']
    header += ['TIME']
    header += ['INTERVAL']
    header += ['DATA']
    
    rows = []
    def on_parse(data:map):
        layers = data['_source']['layers']

        canid = layers['can']['can.id']
        canlen = layers['can']['can.len']
        time_relative = layers['frame']['frame.time_relative']
        time_delta = layers['frame']['frame.time_delta']
        candata = layers['data']['data.data']
        
        contents = {}
        contents['TIME'] = time_relative
        contents['CANID'] = canid
        contents['INTERVAL'] = time_delta
        contents['DATALEN'] = canlen
        contents['DATA'] = candata
        rows.append(contents)
    
    sys.stdout.write('Export to CSV\n')
    if not pcapjson.parse(target=target_json, on_parse=on_parse):
        sys.stdout.write('stop\n')
        return 1
    else:
        with open(target_csv, "w", encoding="utf-8", newline='') as f:
            writer = csv.DictWriter(f, fieldnames=header.row)
            writer.writeheader()
            for row in rows:
                writer.writerow(row)
    
    sys.stdout.write(f"Success to export file : '{target_csv}'\n\n")
    return 0

if __name__ == "__main__":
    mkdir_p(f'{export_directory}/result')
    mkdir_p(f'{export_directory}/tmp')

    for filename in sys.argv[1:]:
        filename = filename.replace('\\', '/')
        _, file = os.path.split(filename)
        name, ext = os.path.splitext(file)

        th = Thread(target=export, args=(filename, name))
        th.start()

        waitcurse(th)
