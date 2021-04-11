# EE4212 Assignment 2 Code
Author: Lingke Ding

In this code base, there are two folders containing the code for Part 1 and Part 2 of the assignment 2. Part 1 is named as Assignment 2 Q1. Part 2 is named as Assignment 2 Q2.

## **In Assignment 2 Q1:**

- Contains all the file needed to run it. 
- The README.txt is written by university of waterloo so please ignore it
- The code has only been tested on macOS and it will require **c++11** standard or above to compile, so please take note if you are testing on windows
- In order to compile on your computer, use the following command (Unix system):

    ``` g++ -std=c++11 -o please_run_this main.cpp Image.cpp graph.cpp maxflow.cpp ```

- In order to test the code on your computer, use the following command (Unix system): 

    ``` ./please_run_this ```

- Things that you need to make sure of

    - The images that you are testing with are in the same directory as this program

- When you are testing the code, you should see the following:

    > Please Enter the Image File Name (with extensions):

    - please give input filesnames such as denoise_input.jpg
    
        - Please do not forget the extension
        - If you input the wrong file name or the file cannot be found, the terminal will tell you failed to read the image name you just typed
    
    > Please Enter the m_lambda value in integer (the best is around 500):

    - Please input an integer number

- The output is saved inside the same directory named Result.jpg

## **In Assignment 2 Q2:**

- This folder contains all the file needed to run it. 
- The GCO_README.txt is written by university of waterloo so please ignore it
- The code has only been tested on macOS and it will require **c++11** standard or above to compile, so please take note if you are testing on windows
- In order to compile on your computer, use the following command (Unix system):

    ``` g++ -std=c++11 -o please_run_this main.cpp Image.cpp GCoptimization.cpp maxflow.cpp LinkedBlockList.cpp ```

- In order to test the code on your computer, use the following command (Unix system): 

    ``` ./please_run_this ```

- Things that you need to make sure of

    - The images that you are testing with are in the same directory as this program

- When you are testing the code, you should see the following:

    > Please Enter the Image File Name (with extensions):

    - please give input filesnames such as denoise_input.jpg
    
        - Please do not forget the extension
        - If you input the wrong file name or the file cannot be found, the terminal will tell you failed to read the image name you just typed
    
    > Please Enter the number of segmented colour:

    - Please input an integer number

    > Please Enter the number of minimum K number u want (it is best to put as 0):

    - Please input an inter number
    - Normally, you should put 0 here. This is an input that you should only use if you want to force the program to give you a particular number of k. 
    
        - The k means clustering logic is that if it takes too long to find K number of cluster centres, meaning it loops for more than 200 times and still cannot find it, it will reduce the number of K by 1. 
        - However, lowest numebr of k that it can go to is defined by the number of minimum k number. 
        - If you set it as 0, it will not actually go down to 0, usually the program will be able to identify at least 2 ~ 3 K numbers with relative ease and speed. Then, the program is done in this way that it loops 10 times to find the best output and each output will have the lowest K number to be at least as small as the previous highest k numeber. 
        - However, if you have set a number here, the lowest k number will be at least as small as the number you set here. 
        - It is worth noting that if put a number that is too high here, the code may just run forever. 

- The output is saved inside the same directory named Result.jpg


|All library used         | Origin           |
| :----------------: | :----------------: |
| Image.h | Code-Break0: https://github.com/Code-Break0/Image-Processing|
| graph.h | University of Waterloo: https://vision.cs.uwaterloo.ca/code/|
| GCoptimization.h| University of Waterloo: https://vision.cs.uwaterloo.ca/code/|
| max_flow.cpp| University of Waterloo: https://vision.cs.uwaterloo.ca/code/|
| K_means_clustering.h| Myself|
