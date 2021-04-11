/***
 * Written by Lingke Ding
 * 
 * To compile: g++ -std=c++11 -o testing main.cpp Image.cpp GCoptimization.cpp maxflow.cpp LinkedBlockList.cpp
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
    char filename[50]; int K_num;

    // Filename
    cout << "Please Enter the Image File Name (with extensions): ";
    cin >> filename;
    Image original(filename);
    
    // Value of k
    cout << "Please Enter the number of segmented colour: ";
    cin >> K_num;

    // Copy the image into a new object so that we can work on it
    Image work_image = original;
    cout << "Image width: " << work_image.w << " Image height: " << work_image.h << endl;

    // Generate K graph
    k_graph knn(work_image.h, work_image.w, K_num);

    // Process for k means clustering
    k_result(knn, K_num, work_image);
    if (K_num != knn.retrieve_total_number_of_cluster()){
        cout << endl;
        cout << "Changing K numebr to " << knn.retrieve_total_number_of_cluster() << endl;
        cout << "This is because you have used a k number that is too big and thus the cluster centre cannot converge to a fixed result" << endl << endl;
        K_num = knn.retrieve_total_number_of_cluster();
    } 

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
 * @param knn KNN Struct Pointer
 * @param num_of_cluster Number of k that you want to have for k means cluster
 * @param work_image The image data struct pointer
 * 
 * @result NULL
 * 
 * @brief does 100 loops of K means clustering calculation and save the best result into the knn struct
*/
void k_result(k_graph& knn, int num_of_cluster, Image& work_image){
    // Loop this 100 times to get the best centre
    int loop_num = 10;
    int cluster_result[loop_num][num_of_cluster][3]; // save all the results of the different runs
    unsigned long int cluster_quality[loop_num];
    int k_num_for_each_loop[loop_num];
    for (int i = 0; i < loop_num; i ++){
        knn.modify_total_number_of_cluster(num_of_cluster); // reinitialise the number of clusters
        knn.cluster_initialisation(); // Reinitialise
        k_means_clustering_single(knn,num_of_cluster,work_image);
        cluster_quality[i] = knn.retrieve_quality_number();
        
        int curr_num_of_cluster = knn.retrieve_total_number_of_cluster();
        k_num_for_each_loop[i] = curr_num_of_cluster;
        cout << "This is loop number " << i + 1 << " of total loop number " << loop_num;
        cout << " ... Number of converged K: " << curr_num_of_cluster << endl;

        int last_best_converged_k = knn.retrieve_converged_num_of_k();
        if (curr_num_of_cluster > last_best_converged_k){
            knn.modify_converged_k(curr_num_of_cluster);
        }

        // Get the result into the cluster result
        int** cluster_center = knn.retrieve_cluster_center();
        for (int j = 0; j < curr_num_of_cluster; j++){
            cluster_result[i][j][0] = cluster_center[j][0]; // R
            cluster_result[i][j][1] = cluster_center[j][1]; // G
            cluster_result[i][j][2] = cluster_center[j][2]; // B
        }
    }

    // Get the best answer, we want the most number of k and the smallest of cost
    int best_index = 0;
    for (int i = 0; i < loop_num; i ++){
        unsigned long int curr_index_result = cluster_quality[i];
        int curr_k_number = k_num_for_each_loop[i];

        // We want the most number of k and smallest of cost
        if (curr_k_number > k_num_for_each_loop[best_index]){
            best_index = i;
        }else if (curr_k_number == k_num_for_each_loop[best_index] && curr_index_result < cluster_quality[best_index]){
            best_index = i;
        }
    }

    // Give this result to the knn
    num_of_cluster = k_num_for_each_loop[best_index];
    knn.modify_total_number_of_cluster(num_of_cluster);
    for (int i = 0; i < num_of_cluster; i++){
        knn.modify_cluster_center(i, cluster_result[best_index][i][0], cluster_result[best_index][i][1], cluster_result[best_index][i][2]);
    }

}

/**
 * 
 * @param knn KNN struct pointer
 * @param num_of_clusters Number of k that you want to have for k means clustering
 * @param work_image The image data struct pointer
 * 
 * @result NULL
 * 
 * @brief Does a single round of k means clustering calculation which the result is saved inside the knn structure
*/
void k_means_clustering_single(k_graph& knn, int num_of_cluster, Image& work_image){
    int loop_number = 0; // This is a loop number used to regulate the total amount of looping allowed

    k_means_clustering: // Where the repeat will goto 
    
    loop_number ++;
    num_of_cluster = knn.retrieve_total_number_of_cluster(); // Just in case that the number of clusters are reduced

    int** cluster_center = knn.retrieve_cluster_center();

    // Start doing the K means clustering
    for (int y = 0; y < work_image.h; y++){
        int graph_x = 0;
        for (int x = 0; x < work_image.w*work_image.channels; x += work_image.channels){
            int pixel_index = x + y*work_image.w*work_image.channels;
            
            // Get the color of that pixel
            int curr_color[3] = {work_image.data[pixel_index], work_image.data[pixel_index+1], work_image.data[pixel_index+2]};

            // Get the distance from this color to the cluster centers
            int color_dist[num_of_cluster]; int best_cluster = 0;
            for (int i = 0; i < num_of_cluster; i++){
                int R_dist = cluster_center[i][0] - curr_color[0]; R_dist = (int) R_dist * R_dist;
                int G_dist = cluster_center[i][1] - curr_color[1]; G_dist = (int) G_dist * G_dist;
                int B_dist = cluster_center[i][2] - curr_color[2]; B_dist = (int) B_dist * B_dist;
                color_dist[i] = (int) sqrt(R_dist + G_dist + B_dist);
                if (i != 0){
                    if (color_dist[i] < color_dist[i-1]) best_cluster = i;
                }
            }
            knn.modify_graph_node(y,graph_x,best_cluster);
            graph_x ++; // increase the graph_x
        }
    }

    // Get the new mean of the color
    unsigned long int cluster_sum[num_of_cluster][3]; // max is 4294967295 which is equilvalent to 16777216 pixels and that means 4k x 4k (not possible to hit with the given images)
    int cluster_number[num_of_cluster];
    for (int i = 0; i < num_of_cluster; i++){
        cluster_number[i] = 0; // flush it with 0 first
        cluster_sum[i][0] = 0;
        cluster_sum[i][1] = 0;
        cluster_sum[i][2] = 0;
    }
    for (int y = 0; y < work_image.h; y++){
        int graph_x = 0;
        for (int x = 0; x < work_image.w*work_image.channels; x += work_image.channels){
            int ID = knn.retrieve_graph_node(y,graph_x);
            cluster_number[ID]++;

            int pixel_index = x + y*work_image.w*work_image.channels;
            int curr_color[3] = {work_image.data[pixel_index], work_image.data[pixel_index+1], work_image.data[pixel_index+2]};

            cluster_sum[ID][0] += curr_color[0]; // R
            cluster_sum[ID][1] += curr_color[1]; // G
            cluster_sum[ID][2] += curr_color[2]; // B

            graph_x++;
        }
    }

    // get new cluster center
    for (int i = 0; i < num_of_cluster; i++){
        if (cluster_number[i] != 0){
            cluster_sum[i][0] = (int) cluster_sum[i][0] / cluster_number[i]; // R
            cluster_sum[i][1] = (int) cluster_sum[i][1] / cluster_number[i]; // G
            cluster_sum[i][2] = (int) cluster_sum[i][2] / cluster_number[i]; // B
        }else{
            cluster_sum[i][0] = 0; cluster_sum[i][1] = 0; cluster_sum[i][2] = 0; // Prevent floating point errors
        } 
    }

    // Compare the new with the old one --> We relax it to be within the vacinity can already
    int same = 0;
    for (int i = 0; i < num_of_cluster; i++){
        for (int j = 0; j < num_of_cluster; j++){
            int R_dist = cluster_sum[i][0] - cluster_center[j][0]; R_dist = (int) sqrt(R_dist * R_dist);
            int G_dist = cluster_sum[i][1] - cluster_center[j][1]; G_dist = (int) sqrt(G_dist * G_dist);
            int B_dist = cluster_sum[i][2] - cluster_center[j][2]; B_dist = (int) sqrt(B_dist * B_dist);

            int thres_hold = 20;
            if (R_dist <= thres_hold && G_dist <= thres_hold && B_dist <= thres_hold){ 
                same++;
            }
        }
    }

    if (same < num_of_cluster){
        //cout << "K means clustering current is: \n";
        for (int i = 0; i < num_of_cluster; i ++){
            knn.modify_cluster_center(i, cluster_sum[i][0], cluster_sum[i][1], cluster_sum[i][2]);
        }

        // if we just keep looping, it means the initialisation is not good, let's reinitialise
        if (loop_number == 100){
            knn.cluster_initialisation();
        }

        // If we keep looping non-stop, it means the user has keyed in a k number that is too large for this image
        // In other words, the image cannot have that many k numbers
        // Thus, we will reduce the k number by 1 each time till it passes
        if (loop_number >= 200){
            // cout << "Reducing total number of clusters" << endl;
            
            // we will only accept output that is at least as good as the last best output
            int last_best_k_number = knn.retrieve_converged_num_of_k();
            if (num_of_cluster > last_best_k_number){
                knn.modify_total_number_of_cluster(num_of_cluster - 1);
                knn.cluster_initialisation();
            }else{
                knn.cluster_initialisation(); // reinitialise
            }
            loop_number = 0;
        }
        goto k_means_clustering;
    }

    // Check if got repeated centers, we cannot allow this to exist, if have reinitialise and run again
    // Check if got any of the centres is 0, we cannot that also because the graph cut algo cannot take it
    int repeated_center = 0; int got_zero_entries = 0;
    for (int i = 0; i < num_of_cluster; i++){
        for (int j = 0; j < num_of_cluster; j++){
            if (i != j){
                if (cluster_center[i][0] == cluster_center[j][0]){ // R
                    if (cluster_center[i][1] == cluster_center[j][1]){ // G
                        if (cluster_center[i][2] == cluster_center[j][2]){ // B
                            repeated_center ++;
                        }
                    }
                }
            }
        }

        if (cluster_center[i][0] == 0 && cluster_center[i][1] == 0 && cluster_center[i][2] == 0) got_zero_entries ++;
    }
    if (repeated_center > 0 || got_zero_entries > 0){
        knn.cluster_initialisation();
        goto k_means_clustering;
    }

    // Get a quality number which is the mean of all the distance
    unsigned long int sum_of_difference = 0;
    for (int y = 0; y < work_image.h; y++){
        int graph_x = 0;
        for (int x = 0; x < work_image.w*work_image.channels; x += work_image.channels){
            int pixel_index = x + y*work_image.w*work_image.channels;
            int curr_color[3] = {work_image.data[pixel_index], work_image.data[pixel_index+1], work_image.data[pixel_index+2]};
            int ID = knn.retrieve_graph_node(y,graph_x);

            int R_dist = cluster_center[ID][0] - curr_color[0]; R_dist = (int) R_dist * R_dist; 
            int G_dist = cluster_center[ID][1] - curr_color[1]; G_dist = (int) G_dist * G_dist;
            int B_dist = cluster_center[ID][2] - curr_color[2]; B_dist = (int) B_dist * B_dist;
            
            int sum = (int) sqrt(R_dist + G_dist + B_dist); sum_of_difference += sum;
            
            graph_x++;
        }
    }
    knn.modify_quality_number(sum_of_difference);
    return;
}

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
                //int cost = (l1-l2)*(l1-l2) <= 4  ? (l1-l2)*(l1-l2):4; // This line has issue
                int cost = (l1 == l2) ? 0 : 1;
    
                /*
                int R_dist = cluster_center[l1][0] - cluster_center[l2][0]; R_dist = (int) sqrt(R_dist * R_dist);
                int G_dist = cluster_center[l1][1] - cluster_center[l2][1]; G_dist = (int) sqrt(G_dist * G_dist);
                int B_dist = cluster_center[l1][2] - cluster_center[l2][2]; B_dist = (int) sqrt(B_dist * B_dist);
                
                int cost;
                if (R_dist < 50 && G_dist < 50 && B_dist < 50){
                    cost = 0;
                }else {
                    cost = 1;
                }
                */
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