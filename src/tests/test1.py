def createGenerator():
    mylist = range(5)
    
    for i in mylist :
        yield i*2

mygenerator = createGenerator()

for i in mygenerator:
    print(i+3)
 