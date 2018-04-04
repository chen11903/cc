'''
Created on 2018年2月6日

@author: c
'''
import face_recognition
import cv2
import time

cap = cv2.VideoCapture(1)     
cap.set(3,640) #设置分辨率  
cap.set(4,480)  

process_this_frame = True

while True:
    ret, small_frame = cap.read()
    t_start = time.clock() 
    
#     small_frame = cv2.resize(frame, (0, 0), fx=0.25, fy=0.25)
#     small_frame = cv2.resize(frame, (0, 0), fx=1, fy=1)

#     rgb_small_frame = small_frame[:, :, ::-1]

    if process_this_frame:
        face_locations = face_recognition.face_locations(small_frame)
        
        faceNum = len(face_locations)
        for i in range(0, faceNum):
            top =  face_locations[i][0]
            right =  face_locations[i][1]
            bottom = face_locations[i][2]
            left = face_locations[i][3]
          
            start = (left, top)
            end = (right, bottom)
          
            color = (55,255,155)
            thickness = 3
            cv2.rectangle(small_frame, start, end, color, thickness)
               
    cv2.imshow('Video', small_frame)
    
#     if cv2.waitKey(1) & 0xFF == ord('q'):
    if cv2.waitKey(1) & 0xFF == 27:
        break
    
    t_end = time.clock() 
    print( "Estimated frames per second  : {0}".format(round(1 / (t_end - t_start),2)))
    
cap.release()
cv2.destroyAllWindows()
