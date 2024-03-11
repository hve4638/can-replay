import json
import sys

def parse(target:str, on_parse:callable, on_error:callable = None)->bool:
    def def_on_error(data, exception, number):
        sys.stderr.write('[jsonparser] error occur while on_parse()\n')
        sys.stderr.write('- index : {number}\n')
        sys.stderr.write('- exception : {exception}\n')
        sys.stderr.write('stop\n')
        exit(-2)

    with open(target, 'r', encoding='utf-8') as f:
        try:
            json_object = json.load(f)
        except json.decoder.JSONDecodeError:
            sys.stderr.write(f'Could not decode json file \'{target}\'\n')
            return False

    for i, data in enumerate(json_object):
        try:
            on_parse(data=data)
        except KeyError as ex:
            val = data['_source']['layers']['frame']['frame.time_relative']

            sys.stderr.write(f'Exception Occurs : KeyError ')
            sys.stderr.write(f"{ex}")
            sys.stderr.write(f'\n')
            on_error(data=data, exception=ex, number = i+1)
        except Exception as ex:
            on_error(data=data, exception=ex, number = i+1)
    return True

        