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
- 


|Library used         | Origin           |
| :----------------: | :----------------: |
| Image.h | Code-Break0: https://github.com/Code-Break0/Image-Processing|
| graph.h | University of Waterloo: https://vision.cs.uwaterloo.ca/code/|
| GCoptimization.h| University of Waterloo: https://vision.cs.uwaterloo.ca/code/|
| max_flow.cpp| University of Waterloo: https://vision.cs.uwaterloo.ca/code/|
| K_means_clustering.h| Myself|


