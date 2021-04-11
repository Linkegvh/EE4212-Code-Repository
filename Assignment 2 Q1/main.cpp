#include "Image.h"
#include <stdio.h>
#include "graph.h"
#include <iostream>
#include <math.h>
#include <ctime>

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
 * 
 * Running the program:
 * - The program will prompt you to input the image file name with extensions, please do so and ensure that the image file is in the same folder as this program
 * - Then the program will prompt you to input the m_lambda value, please input an integer value
*/



int color_dist(int [], int []);

int main(){
    std::clock_t start;
    start = std::clock(); // timer start
    
    char filename[50]; int m_lambda;

    cout << "Please Enter the Image File Name (with extensions): ";
    cin >> filename;
    Image original(filename);

    cout << "Please Enter the m_lambda value in integer (the best is around 500): ";
    cin >> m_lambda;

    //Image original("denoise_input.jpg");

    // Copy the image into a new object so that we can work on it
    Image work_image = original;

    // Initiate the graph structure
    typedef Graph<int,int,int> GraphType;
	GraphType *g = new GraphType(/*estimated # of nodes*/ 2, /*estimated # of edges*/ 1); 
    int i = 0; // i here corresponds to the pixel index 

    int background_color[3] = {244, 209, 112}; // Yellow
    int foreground_color[3] = {0, 8, 248}; // Blue

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

                    g -> add_edge(left_neighbour, i, m_lambda, m_lambda);
                }
            }else{
                if (x != 0){
                    int neigh_dist;

                    // Left neighbour
                    int left_neighbour = i - 1;

                    g -> add_edge(left_neighbour, i, m_lambda, m_lambda);

                    // Upper Neighbour
                    int upper_neighbour = i - work_image.w;

                    g -> add_edge(upper_neighbour, i, m_lambda, m_lambda);


                }else{
                    // This case is the left most column
                    int upper_neighbour = i - work_image.w;

                    g -> add_edge(upper_neighbour, i, m_lambda, m_lambda);
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

    double duration = (clock() - start) / (double) CLOCKS_PER_SEC; // Timer end
    cout << "Run Time: " << duration << "s" << endl;

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