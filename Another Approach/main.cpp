#include "Image.h"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <ctime>
#include "clustering.h"
#include "GCoptimization.h"
#include "graph.h"

using namespace std;

/**
 * To compile: g++ -std=c++11 -o testing main.cpp Image.cpp GCoptimization.cpp maxflow.cpp LinkedBlockList.cpp
*/
int* MRF_gc(int num_labels, Image& work_image, k_means_clustering& knn);

int main(){

    char filename[100]; int k;

    cout << "Filename with extension: ";
    cin >> filename;
    Image input(filename); Image image_data = input;
    int w = image_data.w; int h = image_data.h; int total_pixel = w*h;

    cout << "k: ";
    cin >> k;

    k_means_clustering my_cluster;
    my_cluster.init(w,h,k);
    my_cluster.do_k_clustering(image_data, k);

    // Get the outcome
    int final_k = my_cluster.get_k();
    if (final_k != k){
        cout << endl << "K is updated to: " << final_k << endl;
        k = final_k;
    }
    int ** k_centers = my_cluster.k_center();
    for (int i = 0; i < k; i++){
        cout << "ID: " << i + 1;
        cout << " R: " << k_centers[i][0];
        cout << " G: " << k_centers[i][1];
        cout << " B: " << k_centers[i][2] << endl;
    }
    
    // MRF with multilabel process
    int* result = MRF_gc(k, image_data, my_cluster);

    // Based on the result change data color
    int result_index = 0;
    for (int y = 0; y < image_data.h; y++){
        for (int x = 0; x < image_data.w*image_data.channels; x += image_data.channels){
            int pixel_index = x + y*image_data.w*image_data.channels;
            
            // Setting the color of the image
            for (int i = 0; i < image_data.channels; i++){
                int ID = result[result_index];
                image_data.data[pixel_index + i] = k_centers[ID][i];
            }

            result_index ++;
        }
    }

    image_data.write("Result.jpg");

    return 0;
}

int* MRF_gc(int num_labels, Image& work_image, k_means_clustering& knn){

    // Establish the result array
    int num_pixels = work_image.w*work_image.h;
    int* result = new int[num_pixels];

    int** cluster_center = knn.k_center();

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