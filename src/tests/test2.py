def counter(n):  
    
    while n < 1000:
        yield n
        n += 1

#for i in counter(1):
#    print(i)   
    
squares = [i*i for i in counter(1)][0:10]

print(squares)