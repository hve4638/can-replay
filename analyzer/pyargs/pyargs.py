#!/usr/bin/env python3
import weakref

class PyArgs:
    def __init__(self):
        parser = 1;
        container = {}

    @property
    def option(self):
        return self._option
    


if __name__ == "__main__":
    args = PyArgs()
    @args.option('-v', '--verbose')
    def hello():
        print(123)

    
