class HeaderRow:
    def __init__(self, header) -> None:
        self.__header = header
        self.__row = [ "" ] * len(header)

    def __getitem__(self, index):
        key = self.__header[index]
        return self.__row[key]
    
    def __setitem__(self, index, value):
        key = self.__header[index]
        self.__row[key] = value

    @property
    def row(self):
        return self.__row[:]