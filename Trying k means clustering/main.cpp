#include "k_means_clustering.h"

#include "Image.h"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <ctime>

using namespace std;

/**
 * To run: g++ -std=c++11 -o testing main.cpp Image.cpp
*/

unsigned long int k_means_clustering_single(k_graph& knn, int num_of_cluster, Image& work_image);
void k_result(k_graph& knn, int num_of_cluster, Image& work_image);

int main(){
    std::clock_t start;
    start = std::clock(); // timer start

    int num_of_cluster; char filename[50];

    cout << "Please enter the image file name (with extension): ";
    cin >> filename;
    Image original(filename);

    cout << "Please enter the number of clusters you want to have: ";
    cin >> num_of_cluster;

    // Copy the image into a new object so that we can work on it
    Image work_image = original;
    cout << "Image width: " << work_image.w << " Image height: " << work_image.h << endl;

    // Generate k graph
    k_graph knn(work_image.h, work_image.w, num_of_cluster);
    cout << "KNN successfully Created \n";

    // Get the k_result
    k_result(knn,num_of_cluster,work_image);
    num_of_cluster = knn.retrieve_total_number_of_cluster();

    // Retrieve the result to have a look
    int** cluster_center = knn.retrieve_cluster_center();
    cout << "Final KNN Result: \n";
    for (int i = 0; i < num_of_cluster; i++){
        cout << "ID: " << i;
        cout << " R: " << cluster_center[i][0];
        cout << " G: " << cluster_center[i][1];
        cout << " B: " << cluster_center[i][2];
        cout << "\n";
    }

    double duration = (clock() - start) / (double) CLOCKS_PER_SEC; // Timer end
    cout << "Run Time: " << duration << "s" << endl;

    return 0;
}

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
    for (int i = 0; i < loop_num; i ++){
        cout << "K means clustering loop number: " << i + 1 << endl;
        knn.cluster_initialisation(); // Reinitialise
        cluster_quality[i] = k_means_clustering_single(knn,num_of_cluster,work_image);
        cout << "K means loop number " << i + 1 << " finished" << endl;
        // Get the result into the cluster result
        int** cluster_center = knn.retrieve_cluster_center();
        for (int j = 0; j < num_of_cluster; j++){
            cluster_result[i][j][0] = cluster_center[j][0]; // R
            cluster_result[i][j][1] = cluster_center[j][1]; // G
            cluster_result[i][j][2] = cluster_center[j][2]; // B
        }
    }

    // Get the best answer, the smaller the better
    int best_index = 0;
    for (int i = 0; i < loop_num; i ++){
        int curr_index_result = cluster_quality[i];
        if (curr_index_result < cluster_quality[best_index]){
            best_index = i;
        }
    }

    // Give this result to the knn
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
 * @result A quality number of how good the k means clustering result is, the lower the better
 * 
 * @brief Does a single round of k means clustering calculation which the result is saved inside the knn structure
*/
unsigned long int k_means_clustering_single(k_graph& knn, int num_of_cluster, Image& work_image){
    int loop_number = 0;
    k_means_clustering: // Where the repeat will goto 

    loop_number ++;
    num_of_cluster = knn.retrieve_total_number_of_cluster();
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
                int R_dist = cluster_center[i][0] - curr_color[0]; R_dist = (int) sqrt(R_dist * R_dist);
                int G_dist = cluster_center[i][1] - curr_color[1]; G_dist = (int) sqrt(G_dist * G_dist);
                int B_dist = cluster_center[i][2] - curr_color[2]; B_dist = (int) sqrt(B_dist * B_dist);
                color_dist[i] = R_dist + G_dist + B_dist;
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
        if (cluster_number[i] == 0){
            cluster_sum[i][0] = 0; cluster_sum[i][1] = 0; cluster_sum[i][2] = 0;
        }else{
            cluster_sum[i][0] = (int) cluster_sum[i][0] / cluster_number[i]; // R
            cluster_sum[i][1] = (int) cluster_sum[i][1] / cluster_number[i]; // G
            cluster_sum[i][2] = (int) cluster_sum[i][2] / cluster_number[i]; // B
        }
    }

    // Compare the new with the old one --> We relax it to be within the vacinity can already
    int same = 0;
    for (int i = 0; i < num_of_cluster; i++){
        for (int j = 0; j < num_of_cluster; j++){
            int R_dist = cluster_sum[i][0] - cluster_center[j][0]; R_dist = (int) sqrt(R_dist * R_dist);
            int G_dist = cluster_sum[i][1] - cluster_center[j][1]; G_dist = (int) sqrt(G_dist * G_dist);
            int B_dist = cluster_sum[i][2] - cluster_center[j][2]; B_dist = (int) sqrt(B_dist * B_dist);

            int thres_hold = 10;
            if (R_dist < thres_hold && G_dist < thres_hold && B_dist < thres_hold) same++;
        }
    }

    if (same < num_of_cluster){
        //cout << "K means clustering current is: \n";
        for (int i = 0; i < num_of_cluster; i ++){
            knn.modify_cluster_center(i, cluster_sum[i][0], cluster_sum[i][1], cluster_sum[i][2]);
        }

        if (loop_number > 200){ // sometimes it is a bit difficult to find
            cout << "Reducing total number of clusters" << endl;
            knn.modify_total_number_of_cluster(num_of_cluster - 1);
            loop_number = 0;
        }

        goto k_means_clustering;
    }

    // Check if got repeated centers, we cannot allow this to exist, if have reinitialise and run again
    int repeated_center = 0;
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
    }
    if (repeated_center > 0){
        knn.cluster_initialisation();
        loop_number ++;
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

            int R_dist = cluster_center[ID][0] - curr_color[0]; R_dist = (int) sqrt(R_dist * R_dist); 
            int G_dist = cluster_center[ID][1] - curr_color[1]; G_dist = (int) sqrt(G_dist * G_dist);
            int B_dist = cluster_center[ID][2] - curr_color[2]; B_dist = (int) sqrt(B_dist * B_dist);
            
            int sum = R_dist + G_dist + B_dist; sum_of_difference += sum;
            
            graph_x++;
        }
    }
    return sum_of_difference;
}
