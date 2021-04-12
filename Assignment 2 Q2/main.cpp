/***
 * Written by Lingke Ding
 * 
 * The Image.h header library was referenced to Code-Break0: https://github.com/Code-Break0/Image-Processing
 * 
 * Instruction for macOS and Linux:
 * - To check the the pre-compiled result, in your terminal, type the following: ./please_run_this
 * - To compile the program, type this in your terminal: g++ -std=c++11 -o please_run_this main.cpp Image.cpp GCoptimization.cpp maxflow.cpp LinkedBlockList.cpp
 * 
 * Instruction for windows:
 * - Take reference from the macOS and Linux instruction
 * - I never use windows for coding before, so this code has only been tested using macOS and if you cannot compile, you can email me to use my machine to test
 * 
 * Runing the code:
 * - The program will prompt you for the image file name with extensions, please make sure that the image is inside the same folder as this program
 * - The program will then prompt you for the number of segmented colours, please key in an integer
 * - The program will lastly prompt you for the minimum number of segmented colours that you can accept, please key in an integer for that
 * - (It should be noted that you should usually put 0 so that it won't take forever for the code to run, that is because when you input a minimum number of colours
 * - The code will keep re-initialising till it gets that many colours, sometimes it may just be impossible to get that color due to extremely low probability)
 * 
 * Coding logic:
 * - The code is sepearted into two parts:
 * - The first part is k means clustering using functions k_means_clustering_single and k_result
 * - The second part is Markov Random Field Graph Cut with multi-label using funciton MRF_gc
 * 
 * - In the k means clustering, no matter what k value you give, the program will always use the k value that can converge
 * - What this means is that if you give a k value that is gigantic, such as 100, the program lower the k value in each 200 loop till the program can converge
 * - Moreover, the program will loop the whole k means clustering 10 times, using the best value from these 10 times
 * - The best value is essentially the highest number of k and within those highest numbers of k, the lowest quality number (which is the sum of the difference between pixels to their cluster centre)
 * 
 * - In the Markov Random Field Graph Cut with multi-label, the labels are the cluster centres and data cost if the distance between each pixel to their respective cluster centre
 * - The smoothness is essentially 1 between each labels and 0 with itself. 
*/

#include "Image.h"
#include <stdio.h>
#include <stdlib.h> // for rand
#include "graph.h"
#include <iostream>
#include <math.h>
#include <ctime>
#include "k_means_clustering.h" // For K means clustering
#include "GCoptimization.h"

using namespace std;

void k_means_clustering_single(k_graph& knn, int num_of_cluster, Image& work_image);
void k_result(k_graph& knn, int num_of_cluster, Image& work_image);
int* MRF_gc(int num_labels, Image& work_image, k_graph& knn);

int main(){
    std::clock_t start;
    start = std::clock(); // timer start

    // Input variables decalring
    char filename[50]; int K_num; int minimum_k_num;

    // Filename
    cout << "Please Enter the Image File Name (with extensions): ";
    cin >> filename;
    Image original(filename);
    
    // Value of k
    cout << "Please Enter the number of segmented colour: ";
    cin >> K_num;

    cout << "Please Enter the number of minimum K number u want (it is best to put as 0): ";
    cin >> minimum_k_num;

    // Copy the image into a new object so that we can work on it
    Image work_image = original;
    cout << "Image width: " << work_image.w << " Image height: " << work_image.h << endl;

    // Generate K graph
    k_graph knn(work_image.h, work_image.w, K_num, minimum_k_num);

    // process for k means clustering
    knn.k_result(work_image, K_num);

    // Print out the result
    int** cluster_center = knn.retrieve_cluster_center();
    for (int i = 0; i < K_num; i++){
        cout << "Cluster Center: " << i + 1;
        cout << " R: " << cluster_center[i][0];
        cout << " G: " << cluster_center[i][1];
        cout << " B: " << cluster_center[i][2];
        cout << endl;
    }
    cout << endl;

    // MRF with multilabel process
    int* result = MRF_gc(K_num, work_image, knn);

    // Based on the result change data color
    int result_index = 0;
    for (int y = 0; y < work_image.h; y++){
        for (int x = 0; x < work_image.w*work_image.channels; x += work_image.channels){
            int pixel_index = x + y*work_image.w*work_image.channels;
            
            // Setting the color of the image
            for (int i = 0; i < work_image.channels; i++){
                int ID = result[result_index];
                work_image.data[pixel_index + i] = cluster_center[ID][i];
            }

            result_index ++;
        }
    }

    work_image.write("Result.jpg");

    double duration = (clock() - start) / (double) CLOCKS_PER_SEC; // Timer end
    cout << "Run Time: " << duration << "s" << endl;
    return 0;

}

/*********** Helper Functions ************************/

/**
 * 
 * @param num_labels Number of k
 * @param work_image The image data struct pointer
 * @param knn KNN struct pointer
 * 
 * @result the result of what label does each segment belong to
 * 
 * @brief Makov Random Field with Multi-label graph-cut
*/
int* MRF_gc(int num_labels, Image& work_image, k_graph& knn){

    // Establish the result array
    int num_pixels = work_image.w*work_image.h;
    int* result = new int[num_pixels];

    int** cluster_center = knn.retrieve_cluster_center();

    try{
        GCoptimizationGridGraph *gc = new GCoptimizationGridGraph(work_image.w, work_image.h, num_labels);

        // Set up data cost for each label
        // Essentially cost = |color difference|
        int gc_index = 0;
        for (int y = 0; y < work_image.h; y++){
            int graph_x = 0;
            for (int x = 0; x < work_image.w*work_image.channels; x += work_image.channels){
                int pixel_index = x + y*work_image.w*work_image.channels;
                // Get the color of that pixel
                int curr_color[3] = {work_image.data[pixel_index], work_image.data[pixel_index+1], work_image.data[pixel_index+2]};

                for (int l = 0; l < num_labels; l ++){
                    int R_dist = cluster_center[l][0] - curr_color[0]; R_dist = (int) sqrt(R_dist * R_dist);
                    int G_dist = cluster_center[l][1] - curr_color[1]; G_dist = (int) sqrt(G_dist * G_dist);
                    int B_dist = cluster_center[l][2] - curr_color[2]; B_dist = (int) sqrt(B_dist * B_dist);
                    int total_cost = (int) (R_dist + G_dist + B_dist) / 3;
                    gc->setDataCost(gc_index, l, total_cost);
                }

                gc_index ++; // Iteratively assess the different pixel inside the gc
            }
        }

        // next set up smoothness costs individually
        for (int l1 = 0; l1 < num_labels; l1++){
            for (int l2 = 0; l2 < num_labels; l2++){
                int cost = (l1 == l2) ? 0 : 1;
                gc->setSmoothCost(l1,l2,cost); 
            }
        }

        cout << "Before optimisation energy is " << gc->compute_energy() << endl;
        gc->expansion(10); // run expansion for 2 iterations
        cout << "After optimisation energy is " << gc->compute_energy() << endl;

        for (int i = 0; i < num_pixels; i++){
            result[i] = gc->whatLabel(i);
        }

        delete gc;
    }catch(GCException e){
        e.Report();
    }
    
    return result;
}