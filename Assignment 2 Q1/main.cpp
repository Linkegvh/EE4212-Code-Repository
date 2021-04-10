#include "Image.h"
#include <stdio.h>
#include "graph.h"
#include <iostream>
#include <math.h>

using namespace std;

/***
 * Written by Lingke Ding
 * 
 * The Image.h header library was referenced to Code-Break0 : https://github.com/Code-Break0/Image-Processing
 * 
 * Instruction for macOS and Linux:
 * - To check the pre-compiled result, in your terminal, type the following: ./please_run_this
 * - To compile the program, type this in your terminal: g++ -std=c++11 -o please_run_this main.cpp Image.cpp graph.cpp maxflow.cpp
 * 
 * Instruction for windows:
 * - Take reference from the macOS and Linux instruction. 
 * - I never use windows for coding before, so this code has only been tested using macOS and if it fails to run on windows, you are welcome to email me to test on a different machine
*/



int color_dist(int [], int []);

int main(){
    
    char filename[50];

    cout << "Please Enter the Image File Name (with extensions): ";
    cin >> filename;
    Image original(filename);

    //Image original("denoise_input.jpg");

    // Copy the image into a new object so that we can work on it
    Image work_image = original;

    // Initiate the graph structure
    typedef Graph<int,int,int> GraphType;
	GraphType *g = new GraphType(/*estimated # of nodes*/ 2, /*estimated # of edges*/ 1); 
    int i = 0; // i here corresponds to the pixel index 

    int background_color[3] = {244, 209, 112}; // Yellow
    int foreground_color[3] = {0, 8, 248}; // Blue
    int m_labmda = 480;

    // Iterate through every pixel
    for (int y = 0; y < work_image.h; y++){
        for (int x = 0; x < work_image.w*work_image.channels; x += work_image.channels){ // 3 color channels
            int pixel_index = x + y*work_image.w*work_image.channels;

            // Get the color of that pixel
            int colour[3] = {work_image.data[pixel_index], work_image.data[pixel_index+1], work_image.data[pixel_index+2]};
            
            // Add another node
            g -> add_node();
            g -> add_tweights(i, color_dist(foreground_color, colour), color_dist(background_color, colour));
            
            // add edge (we use the 4 neighbour method)
            if (y == 0){
                // This case is row 1 which has no upper neighbour
                if (x != 0){
                    // pixel 0 has no upper nor left neighbour
                    int left_neighbour = i-1;

                    g -> add_edge(left_neighbour, i, m_labmda, m_labmda);
                }
            }else{
                if (x != 0){
                    int neigh_dist;

                    // Left neighbour
                    int left_neighbour = i - 1;

                    g -> add_edge(left_neighbour, i, m_labmda, m_labmda);

                    // Upper Neighbour
                    int upper_neighbour = i - work_image.w;

                    g -> add_edge(upper_neighbour, i, m_labmda, m_labmda);


                }else{
                    // This case is the left most column
                    int upper_neighbour = i - work_image.w;

                    g -> add_edge(upper_neighbour, i, m_labmda, m_labmda);
                }   
            }

            i++;
        }
    }

    cout << "Total number of pixel: " << i << '\n';
    cout << "Image width: " << work_image.w << " Image height: " << work_image.h << '\n';

    int flow = g -> maxflow();
    cout << "Flow = " << flow << '\n';
    
    // Start mapping back 
    for (int iterate = 0; iterate < i; iterate++){
        int pixel_index = iterate * work_image.channels;
        if (g -> what_segment(iterate) == GraphType::SOURCE){
            // Set the pixel colour to source colour
            for (int j = 0; j < 3; j++){
                work_image.data[pixel_index + j] = background_color[j];
            }
        }else if (g -> what_segment(iterate) == GraphType::SINK){
            // Set the pixel colour to source colour
            for (int j = 0; j < 3; j++){
                work_image.data[pixel_index + j] = foreground_color[j];
            }
        }
    }

    delete g;

    work_image.write("Result.jpg");

    return 0;
}

// A very primal color distance function
int color_dist(int color_a[], int color_b[]){
    int dist = 0;
    for (int i = 0; i < 3; i++){
        dist = dist + color_a[i] - color_b[i];
    }
    dist = (int) sqrt(dist*dist);
    return dist;
}