/**
 * Written by Lingke Ding
 * 
 * This header library contains the k means clustering structure that closely follows
 * the idea of adjecency graph. In a way, it stores all the cluster centres, graph node 
 * of which cluster centre does that pixel belong to. 
 * 
*/


#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <ctime>
#include "Image.h"
#include <math.h>

using namespace std;

class k_graph{
    private:
        int ** cluster_center;
        int total_number_of_clusters;
        int converged_number_of_clusters;
        int w;
        int h;
        int matrix_size;
        unsigned long int quality_number;
    public:
        k_graph(int h, int w, int number_of_clusters, int min_k){
            // Initialise cluster center
            this->total_number_of_clusters = number_of_clusters;
            //cout << "Initialising cluster centre\n";
            cluster_center = new int*[total_number_of_clusters];
            for (int i = 0; i < total_number_of_clusters; i++){
                cluster_center[i] = new int[3];
            }
            cluster_initialisation();

            //cout << "Initialising the graph matrix\n";
            // Initialise graph matrix
            this->w = w;
            this->h = h;
            this->matrix_size = w*h;
            this->converged_number_of_clusters = min_k;
            return;
        };

        void modify_cluster_center(int ID, int R, int G, int B);
        int** retrieve_cluster_center() {return cluster_center;};
        void modify_graph_node(int h, int w, int ID);
        void cluster_initialisation();
        void modify_total_number_of_cluster(int new_value){this->total_number_of_clusters = new_value;};
        int retrieve_total_number_of_cluster(){return total_number_of_clusters;};
        void modify_quality_number(unsigned long int number){this->quality_number = number;};
        unsigned long int retrieve_quality_number(){return quality_number;};
        void modify_converged_k(int number){this->converged_number_of_clusters = number;};
        int retrieve_converged_num_of_k(){return converged_number_of_clusters;};
        void k_result(Image& work_image, int numebr_of_clusters);
        void k_means_clustering_single(Image& work_image);
};

void k_graph::modify_cluster_center(int ID, int R, int G, int B){
    cluster_center[ID][0] = R;
    cluster_center[ID][1] = G;
    cluster_center[ID][2] = B;
}

void k_graph::cluster_initialisation(){
    srand(time(0));
    repeat: 
   
    for (int i = 0; i < total_number_of_clusters; i ++){
        cluster_center[i][0] = rand() % 256;
        cluster_center[i][1] = rand() % 256;
        cluster_center[i][2] = rand() % 256;
    }
    //cout << "Cluster centered initialised ... Checking if got repeated\n";
    // Use a stupid method to compare (anyway we won't be comparing a large dataset)
    for (int i = 0; i < total_number_of_clusters; i++){
        for (int j = 0; j < total_number_of_clusters; j++){
            if (i != j){
                // R
                if (cluster_center[i][0] == cluster_center[j][0]){
                    // G
                    if (cluster_center[i][1] == cluster_center[j][1]){
                        // B
                        if (cluster_center[i][2] == cluster_center[j][2]){
                            goto repeat;
                        }
                    }
                }
            }
        }
    }
    
}

void k_graph::k_result(Image& work_image, int num_of_cluster){
    cout << endl << "K Means Clustering..." << endl;

    int loop_num = 10;
    int cluster_result[loop_num][num_of_cluster][3]; // save all the results of the different runs
    unsigned long int cluster_quality[loop_num];
    int k_num_for_each_loop[loop_num];

    for (int i = 0; i < loop_num; i++){
        k_means_clustering_single(work_image);
        cout << "This is loop number " << i + 1 << " of total loop number " << loop_num;
        cout << " ... Number of converged K: " << total_number_of_clusters + 1 << endl;
        
        for (int j = 0; j < total_number_of_clusters; j++){
            cluster_result[i][j][0] = cluster_center[j][0]; cluster_result[i][j][1] = cluster_center[j][1]; cluster_result[i][j][2] = cluster_center[j][2];
        }
        k_num_for_each_loop[i] = total_number_of_clusters;
        cluster_quality[i] = quality_number;
        if (total_number_of_clusters > converged_number_of_clusters) {converged_number_of_clusters = total_number_of_clusters;}
        total_number_of_clusters = num_of_cluster; // reset the k value
    }

    // Get the best answer, we want the most number of k and the smallest of cost
    int best_index = 0;
    for (int i = 0; i < loop_num; i++){
        if (k_num_for_each_loop[i] == converged_number_of_clusters){
            if (best_index == 0){
                best_index = i;
            }else{
                if (cluster_quality[i] < cluster_quality[best_index]){
                    best_index = i;
                }
            }
        }
    }

    total_number_of_clusters = k_num_for_each_loop[best_index];
    for (int i = 0; i < total_number_of_clusters; i++){
        modify_cluster_center(i, cluster_result[best_index][i][0], cluster_result[best_index][i][1], cluster_result[best_index][i][2]);
    }
    return;
}

void k_graph::k_means_clustering_single(Image& work_image){
    int loop_num = 0;
    cluster_initialisation();

    k_means_clustering: // loop starts here
    loop_num++;

    quality_number = 0; 
    // Start doing the k means clustering
    int cluster_number[total_number_of_clusters]; for (int i = 0; i < total_number_of_clusters; i++){cluster_number[i] = 0;}
    unsigned long int cluster_sum[total_number_of_clusters][3]; for (int i = 0; i < total_number_of_clusters; i++){cluster_sum[i][0] = 0; cluster_sum[i][1] = 0; cluster_sum[i][2] = 0;}
    for (int y = 0; y < work_image.h; y++){
        for (int x = 0; x < work_image.w*work_image.channels; x += work_image.channels){
            int pixel_index = x + y*work_image.w*work_image.channels;
            // Get the color of that pixel
            int curr_color[3] = {work_image.data[pixel_index], work_image.data[pixel_index+1], work_image.data[pixel_index+2]};

            // Get the distance from this color to the cluster centers
            int color_dist[total_number_of_clusters]; int best_cluster = 0;
            for (int i = 0; i < total_number_of_clusters; i++){
                int R_dist = cluster_center[i][0] - curr_color[0]; R_dist = (int) R_dist * R_dist;
                int G_dist = cluster_center[i][1] - curr_color[1]; G_dist = (int) G_dist * G_dist;
                int B_dist = cluster_center[i][2] - curr_color[2]; B_dist = (int) B_dist * B_dist;
                color_dist[i] = (int) sqrt(R_dist + G_dist + B_dist);
                if (i != 0){
                    if (color_dist[i] < color_dist[best_cluster]) best_cluster = i;
                }
            }
            cluster_sum[best_cluster][0] += curr_color[0]; cluster_sum[best_cluster][1] += curr_color[1]; cluster_sum[best_cluster][2] += curr_color[2];
            cluster_number[best_cluster] ++;
            quality_number += color_dist[best_cluster];
        }
    }

    // Get the new mean of the color
    for (int i = 0; i < total_number_of_clusters; i++){
        if (cluster_number[i] != 0){
            cluster_sum[i][0] = (int) cluster_sum[i][0] / cluster_number[i]; // R
            cluster_sum[i][1] = (int) cluster_sum[i][1] / cluster_number[i]; // G
            cluster_sum[i][2] = (int) cluster_sum[i][2] / cluster_number[i]; // B
        }else{
            cluster_sum[i][0] = 0; cluster_sum[i][1] = 0; cluster_sum[i][2] = 0; 
        }
    }

    // Compare the new with the old one --> We relax it to be within the vacinity can already
    int same = 0; int thres_hold = 30;
    for (int i = 0; i < total_number_of_clusters; i++){
        for (int j = 0; j < total_number_of_clusters; j++){
            int R_dist = cluster_sum[i][0] - cluster_center[j][0]; 
            int G_dist = cluster_sum[i][1] - cluster_center[j][1]; 
            int B_dist = cluster_sum[i][2] - cluster_center[j][2]; 
            int dist = (int) sqrt(R_dist * R_dist + G_dist * G_dist + B_dist * B_dist);
            if (dist <= thres_hold){same ++;}
        }
    }

    if (same < total_number_of_clusters){
        for (int i = 0; i < total_number_of_clusters; i++){
            modify_cluster_center(i, cluster_sum[i][0], cluster_sum[i][1], cluster_sum[i][2]);
        }

        if (loop_num == 50){
            cluster_initialisation();
        }

        if (loop_num >= 100){
            if (total_number_of_clusters > converged_number_of_clusters){total_number_of_clusters = total_number_of_clusters - 1;}
            cluster_initialisation(); loop_num = 0;
        }
        goto k_means_clustering;
    }

    // check if got repeated centers, we do not want that
    // Check if got zero centers, we also do not want that (that is the initialisation value)
    for (int i = 0; i < total_number_of_clusters; i ++){
        for (int j = 0; j < total_number_of_clusters; j++){
            if (i != j){
                if (cluster_center[i][0] == cluster_center[j][0] && cluster_center[i][1] == cluster_center[j][1] && cluster_center[i][2] == cluster_center[j][2]){
                    cluster_initialisation(); goto k_means_clustering;
                }
            }
        }
        if (cluster_center[i][0] == 0 && cluster_center[i][1] == 0 && cluster_center[i][1] == 0){
            cluster_initialisation(); goto k_means_clustering;
        }
    }
}