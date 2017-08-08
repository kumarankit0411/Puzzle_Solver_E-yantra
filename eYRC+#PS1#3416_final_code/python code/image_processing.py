'''
*
* Team Id: 		eYRC+#PS1#3416
* Author List: 		HIMANSHU SHARMA, PIYUSH SINGH, ABHINAV SINGH, ANKIT KUMAR SINGH(TEAM LEADER)
* Filename: 		image_processing.py
* Theme: 		PUZZLE SOLVER - 1 
* Functions: 		get_template(n),play(img),puzzle(A,B),Set_in_order(A,B),transfer_2_bot(Grid1,Grid2,Sol),
* Global Variables:	temp0,temp1,temp2,temp3,temp4,temp5,temp6,temp7,temp8,temp9(all for storing template image)
*
'''
import cv2
import numpy
from itertools import combinations                       #built-in python function
from itertools import permutations                       #built-in python function
import serial

'''
*
* Function Name: 	get_template
* Input: 		number-->the digit whose template immage is required
* Output: 		image-->returns the template image of required digit
* Logic: 		takes an input and returns the template image of that digit stored in templates folder
* Example Call:		get_template(2)
*
'''

def get_template(number):    
        if number==0:
            return temp0
        if number==1:
            return temp1
        if number==2:
            return temp2
        if number==3:
            return temp3
        if number==4:
            return temp4
        if number==5:
            return temp5
        if number==6:
            return temp6
        if number==7:
            return temp7
        if number==8:
            return temp8
        if number==9:
            return temp9

#various templates being read here
temp0=cv2.imread('templates/0.jpg')
temp1=cv2.imread('templates/1.jpg')
temp2=cv2.imread('templates/2.jpg')
temp3=cv2.imread('templates/3.jpg')
temp4=cv2.imread('templates/4.jpg')
temp5=cv2.imread('templates/5.jpg')
temp6=cv2.imread('templates/6.jpg')
temp7=cv2.imread('templates/7.jpg')
temp8=cv2.imread('templates/8.jpg')
temp9=cv2.imread('templates/9.jpg')

#core function that traces out number from test image
'''
*
* Function Name: 	play
* Input: 		img-->image of the arena
* Output: 	        none
* Logic: 		detects the number from the arena image
* Example Call:		play(img)
*
'''

def play(img):
    
    #img-- a single test image as input argument
    #No_pos_D1 -- List containing detected numbers in Division D1
    #No_pos_D2 -- List of pair [grid number, value]

    
    No_pos_D1=[]
    No_pos_D2=[]
    img1=img.copy()   #copy of original image
    img2=img.copy()
   
    h,w,c=img.shape

    #cropping image and finding contours for D1

    D1_crop=img1[0:h,0:w/2.3,:]
    main1=D1_crop.copy()
    img1_gray=cv2.cvtColor(D1_crop,cv2.COLOR_BGR2GRAY)
    ret1,thresh1=cv2.threshold(img1_gray,127,255,0)
    _,contours1,heirarchy1=cv2.findContours(thresh1,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)

    #cropping image and finding contours for D2

    D2_crop=img1[0:h,w/2.3:w,:]
    main2=D2_crop.copy()
    img2_gray=cv2.cvtColor(D2_crop,cv2.COLOR_BGR2GRAY)
    ret2,thresh2=cv2.threshold(img2_gray,127,255,0)
    _,contours2,heirarchy2=cv2.findContours(thresh2,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)


    #processing for extracting out numbers from first grid D1

    roimage1=None
    for cnt1 in contours1:
        if cv2.contourArea(cnt1)>370 and cv2.contourArea(cnt1)<1320:     #only processing contours that have area in specific range.This helps us get number.
            x,y,w,h =cv2.boundingRect(cnt1)                             #getting coardinates of rectangle that would completely cover our number.
            roimage1=main1[y:y+h,x:x+w]                                #cropping out then part of image inside the rectangle.
            for j in range(0,10):                                      #loop to compare the cropped image with our templates.
                template1=get_template(j)
                res=cv2.matchTemplate(template1,roimage1,1)
                mins,_,_,_=cv2.minMaxLoc(res)                          #mins will store minimum value for the most approximate comparison.
                if mins<.06:
                    No_pos_D1.append(j)                                #appending the detected number to our list.
                    break
                  

    #processing for extracting out numbers and their position in the second grid D2

    roimage2=None
    cell_No=24        #keeps track of the cell no.
    c=0               #used in checking whether the digit extracted is at one's place or ten's place.
    xt=0              
    yt=0
    wt=0
    ht=0
    first=0
    for cnt2 in contours2:
        if cv2.contourArea(cnt2)>1330 and cv2.contourArea(cnt2)<7000:
            cell_No=cell_No-1
            c=0
            if(first>0):
                No_pos_D2.append([cell_No+1,first])             #if single digit number is present in a cell of grid D2 then this line executes
                first=0
        if cv2.contourArea(cnt2)>370 and cv2.contourArea(cnt2)<1320:
            x1,y1,w1,h1=cv2.boundingRect(cnt2)
            if ((x1+w1+15) in range(xt,xt+wt) or (x1-15) in range(xt,xt+wt)):           
                c=1
            roimage2=main2[y1:y1+h1,x1:x1+w1]
            for p in range(0,10):                                             #loop to detect numbers in test image by template matching
                template2=get_template(p)
                res=cv2.matchTemplate(template2,roimage2,1)
                mins,_,_,_=cv2.minMaxLoc(res)
                if mins<0.09:                       #This if loop does the work of detecting the place(one's or ten's) of digits in case of two digit number.
                    if c==0:
                        first=p
                        break
                    if((x1-15) in range(xt,xt+wt)):
                        No_pos_D2.append([cell_No,(first*10)+p])
                        first=0
                        break
                    else:    
                        No_pos_D2.append([cell_No,(p*10)+first])
                        first=0
                        break
            xt=x1
            yt=y1
            wt=w1
            ht=h1    

    #drawing contours on the digits 
            
    img_gray=cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    ret,thresh=cv2.threshold(img_gray,27,255,0)
    _,contours,hierarchy=cv2.findContours(thresh,cv2.RETR_TREE,cv2.CHAIN_APPROX_NONE)
    count=len(contours)
    for ct in range(0,count):
        a = cv2.contourArea(contours[ct])
        if (a<1300 and a>50):
            cv2.drawContours(img,contours,ct,(0,255,0),2)

    
    cv2.imshow('image',img)       #showing the output image

    return list(reversed(No_pos_D1)),list(reversed(No_pos_D2))     #returning required lists to where the function was called.


       
'''
*
* Function Name: 	puzzle
* Input: 		A-->list of numbers present in Grid 1
                        B-->list of numbers present in Grid 2 with their positions
* Output: 		Sol-->list of numbers required to complete each number in Grid 2
* Logic: 		Performs calculation on the list of numbers on arena and builds a solution list named sol
* Example Call:		puzzle(No_pos_D1,newD2)
*
'''

def puzzle(A,B):                                         #function performing the given task
    C=list(B[0])                                         #creates a copy of a list
    x=len(C)
    for i in range(x-1,-1,-1):                           #this loop deleted the position number from second list D2
        if i%2==0:
            C.pop(i)

    #checking the possible combinations for given numbers
            
    f=0                                                  #used as a flag
    M=[]                                                 #stores list of tuples
    c=()                                                 #a tuple storing numbers which add to a particular number from D2
    F=[]                                                 #stores final list of tuples
    large=13                                             #limiting factor for total numbers that can be used from D1 to make all numbers in D2
    count=0                                              #stores sum of length of tuples
    Min_length=0                                         #limiting factor for number of tuples in the list
    #add your code here
    k=len(C)
    for a in permutations(C,k):                          #loop to generate all possible permutations of a list D2
        A1=list(A[0])
        for i in range(k):
            for t in range(1,13):                        #t is the number of digits to be used for making the sum
                if f==1:                                 #if sum found then break
                    break
                for v in combinations(A1,t):             #loop to generate all possible combinations of 't' numbers in D1
                    if sum(v)==a[i]:                     #if any combinations sums up to required number then append to M
                        M.append(v)
                        c=v
                        count=count+len(c)
                        f=1
                        break
                for u in c:                              #loop to remove used elements from first list to disallow repetition
                    A1.remove(u)
                c=()                                     #initialise tuple 'c' to null
            f=0                                          #initialise f to 0
           
        if count<large and Min_length<=len(M):           #condition to check if a new combination is possible for all the numbers of D2(if yes then executes)
            Min_length=len(M)
            large=count                                  #updating large
            F=list(M)
        M=[]   
        count=0
    Q=list(F)    
    Sol=Set_in_order(C,Q)
     
    return Sol

'''
*
* Function Name: 	Set_in_order
* Input: 		A-->list of numbers present in Grid 1
                        B-->list of numbers present in Grid 2 with their positions
* Output: 		Sol-->list of numbers required to complete each number in Grid 2
* Logic: 		corrects the order in which solution set is arranged.
                        For ex. if three nos. in Grid 2 are (11,12,14) then 'sol' should contain the numbers required to complete 11 first then the numbers to
                        complete 12 and then the number to complete 14.
* Example Call:		Set_in_order(C,Q)
*
'''
def Set_in_order(A,B):                         ##used to set the solution set in order
    Sol=[]    
    for i in A:
        for j in B:
            if sum(j)==i:
                Sol.append(j)
                B.remove(j)
                break
    #print Sol            
    return Sol           

'''
*
* Function Name: 	transfer_2_bot
* Input: 		Grid1-->list containing numbers in Grid 1
                        Grid2-->list containing numbers in Grid 2
                        Sol-->list containing the numbers required to sum up and complete numbers in Grid 2
* Output: 		transfers the list to robot via serial communication
* Logic: 		lists are communicated to the Robot serially using pyserial module
                        Each number is sent seperately in its ASCII form and recieved by robot via serial cable during the running of this code
                        When the code is run, the numbers are sent to the bot via 'COM2' port on our PC as soon as this function executes.
* Example Call:		transfer_2_bot(No_pos_D1[0],newD2[0],Sol)
*
'''
def transfer_2_bot(Grid1,Grid2,Sol):
       ser=serial.Serial('com1')   #connected to COM2
       #communicating the Grid1 list
       for i in Grid1:
           i=i + 48    #converting to ASCII  
           ser.write(chr(i))   #writing on port for robot to read
       ser.write('.')           #indicates completion of transfer of Grid1

       #communicating the Grid2 list
       for i in Grid2:
           if i>9:              #if number being sent is double digit then executes
              temp1=int(i/10) + 48
              temp2=(i % 10) + 48
              ser.write(chr(temp1)+chr(temp2)+',')
           else:                #if number being sent is single digit then executes
              i=i+48
              ser.write(chr(i)+',')
       ser.write('.')           #indicates completion of transfer of Grid2

       #communicating the Solution set
       for i in Sol:    
           for j in i:
               j = j + 48 
               ser.write(chr(j))
           ser.write(',')       #indicates succesful communication of one solution out of solution set
       ser.write('.')    #indicates completion of transfer of Sol(solution set)

#########################################################################################################################################
#########################################################################################################################################
if __name__ == "__main__":
    
    
    ########code for getting the array D1 and D2
    img = cv2.imread('test_image3.jpg')
    No_pos_D1,No_pos_D2 = play(img)
    print 'D1=',No_pos_D1
    print 'D2=',No_pos_D2
    ########
    #converting No_pos_D1 list in required form before being used by other functions
    No_pos_D1=[No_pos_D1]
    newD2=[]
    #converting No_pos_D2 in required form.For ex. [[2,12],[6,10],[10,18]]   ---->    [2,12,6,10,10,18]
    for i in No_pos_D2:
        for j in i:
            newD2.append(j)
    newD2=[newD2]
    Sol=puzzle(No_pos_D1,newD2)  #gets the solution list from puzzle function
    transfer_2_bot(No_pos_D1[0],newD2[0],Sol)  #used to transfer the output of this code to Robot via serial cable
    
   
    cv2.waitKey(0)
    cv2.destroyAllWindows()
