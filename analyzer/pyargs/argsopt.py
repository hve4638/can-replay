class ArgsOpt:
    def __init__(self, container):
        pass
    
    def int(self, *opt):
        def decorator(func):
            return func
        return decorator
    
    def bool(self, *opt):
        def decorator(func):
            return func
        return decorator
    
    def float(self, *opt):
        def decorator(func):
            return func
        return decorator
    
    def string(self, *opt):
        def decorator(func):
            return func
        return decorator