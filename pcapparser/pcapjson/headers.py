from .header_row import HeaderRow

class Headers:
    def __init__(self):
        self.__row = []
        self.__headersDict = {}

    def __add__(self, item):
        if isinstance(item, str):
            self.__append([item])
        elif isinstance(item, list):
            self.__append(item)
        return self

    def __append(self, items):
        self.__row.append(items[0])
        index:int = len(self.__row) - 1

        for item in items:
            if not isinstance(item, str):
                item = str(item)
            self.__headersDict[item] = index

    def __getitem__(self, index):
        return self.__headersDict[index]
    
    def __len__(self):
        return len(self.__row)

    @property
    def row(self):
        return self.__row[:]
    
    def specifier(self):
        return HeaderRow(self)