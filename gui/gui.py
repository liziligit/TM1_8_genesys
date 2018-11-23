import os
import time

ISOTIMEFORMAT='%Y-%m-%d %X'

from Tkinter import *
root = Tk()
root.title("1X8 control panel")
root.geometry('1200x150')

var=StringVar()
e=Entry(root,textvariable=var,width=60)
var.set("")
e.pack()




def check1D():
    t=var.get()
    var.set("") 
    logFile=open("log.txt",'a')
    logFile.write(time.strftime(ISOTIMEFORMAT,time.gmtime(time.time()))+"  ./check1D.sh "+ t+"\n");
    print os.system("./check1D.sh "+ t)
    logFile.close()   

def check2D():
    
    t=var.get()
    var.set("") 
    logFile=open("log.txt",'a')
    logFile.write(time.strftime(ISOTIMEFORMAT,time.gmtime(time.time()))+"  ./check2D.sh "+ t+"\n");
    print os.system("./check2D.sh "+ t)
    logFile.close()
   

def init():
    t=var.get()
    var.set("") 
    logFile=open("log.txt",'a')
    logFile.write(time.strftime(ISOTIMEFORMAT,time.gmtime(time.time()))+"  ./init.sh "+ t+"\n")
    print os.system("./init.sh "+ t)
    logFile.close()  

def takeData():
    t=var.get()
    var.set("") 
    logFile=open("log.txt",'a')
    logFile.write(time.strftime(ISOTIMEFORMAT,time.gmtime(time.time()))+"  ./takeData.sh "+ t+"\n");
    print os.system("./takeData.sh "+ t)
    logFile.close()
   

def editScript():
    t=var.get()
    var.set("")
    print os.system("./editScript.sh "+ t)
   


def test():
    t=var.get()
    var.set("")
    print os.system("./test.sh "+ t)





def noScan():
    t=var.get()
    var.set("") 
    logFile=open("log.txt",'a')
    logFile.write(time.strftime(ISOTIMEFORMAT,time.gmtime(time.time()))+"  ./noScan.sh "+ t+"\n");
    print os.system("./noScan.sh "+ t)
    logFile.close()
    
def pulse():
    t=var.get()
    var.set("") 
    logFile=open("log.txt",'a')
    logFile.write(time.strftime(ISOTIMEFORMAT,time.gmtime(time.time()))+"  ./pulse.sh "+ t+"\n");
    print os.system("./pulse.sh "+ t)
    logFile.close()


def charge():
    t=var.get()
    var.set("") 
    logFile=open("log.txt",'a')
    logFile.write(time.strftime(ISOTIMEFORMAT,time.gmtime(time.time()))+"  ./charge.sh "+ t+"\n");
    print os.system("./charge.sh "+ t)
    logFile.close()
    

def pede():
    t=var.get()
    var.set("") 
    logFile=open("log.txt",'a')
    logFile.write(time.strftime(ISOTIMEFORMAT,time.gmtime(time.time()))+"  ./pede.sh "+ t+"\n");
    print os.system("./pede.sh "+ t)
    logFile.close()
    


w=8
l=3

Button(root, text="init", command = init, width=w, height=l).pack(side=LEFT)
Button(root, text="takeData", command = takeData, width=w, height=l).pack(side=LEFT)
Button(root, text="pede", command = pede, width=w, height=l).pack(side=LEFT)
Button(root, text="check 1D", command = check1D, width=w, height=l).pack(side=LEFT)
Button(root, text="check 2D", command = check2D, width=w, height=l).pack(side=LEFT)
Button(root, text="noScan", command = noScan, width=w, height=l).pack(side=LEFT)
Button(root, text="pulse", command = pulse, width=w, height=l).pack(side=LEFT)
Button(root, text="charge", command = charge, width=w, height=l).pack(side=LEFT)
Button(root, text="editScript", command = editScript, width=w, height=l).pack(side=LEFT)
Button(root, text="test", command = test, width=w, height=l).pack(side=LEFT)



root.mainloop()




