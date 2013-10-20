import os
import sys


class A(object):
    def __init__(self):
        print("A::1")
        #super(self.__class__, self).__init__()
        print("A::2")

class B(A):
    def __init__(self):
        print("B::1")
        super(self.__class__, self).__init__()
        print("B::2")
        
        
def main(argv=sys.argv):
    B()
    return 0

if __name__ == "__main__":
    sys.exit(main())