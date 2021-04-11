
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <iostream>
#include "Image.h"

using namespace std;

class k_means_clustering{
    private:
        int width;
        int height;
        int num_pixel;
        int ** k_centers;
        int * k_matrix;
        int k;
        int best_k;
        int quality;

    public:
        void init(int w, int h, int k_input);
        void replace_k_center(int k_index, int R, int G, int B){k_centers[k_index][0] = R; k_centers[k_index][1] = G; k_centers[k_index][2] = B;};
        int** k_center(){return k_centers;};
        void node_change(int index, int k_index){k_matrix[index] = k_index;};
        int node_retrieve(int index){return k_matrix[index];};
        void k_centers_init();
        void k_clustering_single_run(Image& the_image);
        void do_k_clustering(Image& the_image, int k_input);
        int get_k(){return k;};
};


void k_means_clustering::init(int w, int h, int k_input){
    this->width = w; this->height = h; this->num_pixel = w*h; this->k = k_input; this->best_k = 0; this->quality = 0;
    
    // initialise the k centers
    k_centers = new int* [k_input];
    for (int i = 0; i < k; i ++) {k_centers[i] = new int [3];}
    k_centers_init();

    // initialise k matrix
    k_matrix = new int [num_pixel]; for (int i = 0; i < num_pixel; i++){k_matrix[i] = 0;} // flush with 0

    return;
}

void k_means_clustering::k_centers_init(){
    srand(time(0)); // random seed coming from time now
    loop: 
    // initialise with random
    for (int i = 0; i < k; i++){ k_centers[i][0] = rand()%256; k_centers[i][1] = rand()%256; k_centers[i][2] = rand()%256;}

    // Check if got repeat
    for (int i = 0; i < k; i++){
        for (int j = 0; j < k; j++){
            if (i != j) {
                if (k_centers[i][0] == k_centers[j][0] && k_centers[i][1] == k_centers[j][1] && k_centers[i][2] == k_centers[j][2]){ 
                    goto loop;
                }
            }
        }
    }
    return;
}

void k_means_clustering::k_clustering_single_run(Image& the_image){
    int loop = 0;
    k_centers_init();

    restart:

    loop ++;

    // k means clustering start
    int index = 0; quality = 0;
    unsigned long int cluster_sum[k][3]; for (int i = 0; i < k; i++){cluster_sum[i][0] = 0; cluster_sum[i][1] = 0; cluster_sum[i][2] = 0;}
    int cluster_count[k]; for (int i = 0; i < k; i++){cluster_count[i] = 0;}
    for (int h = 0; h < the_image.h; h++){
        for (int w = 0; w < the_image.w*the_image.channels; w += the_image.channels){
            int data_index = w + h*the_image.w*the_image.channels;
            int R = the_image.data[data_index]; int G = the_image.data[data_index + 1]; int B = the_image.data[data_index + 2];
            int best_k[2] = {0,0};
            for (int i = 0; i < k; i++){
                int R_dist = k_centers[i][0] - R; 
                int G_dist = k_centers[i][1] - G;
                int B_dist = k_centers[i][2] - B;
                int dist = (int) sqrt(R_dist * R_dist + G_dist * G_dist + B_dist * B_dist);
                if (best_k[0] == 0){best_k[0] = dist;}
                if (dist < best_k[0]){ best_k[0] = dist; best_k[1] = i;}
            }
            k_matrix[index++] = best_k[1]; // allocate best index
            cluster_sum[best_k[1]][0] += R; cluster_sum[best_k[1]][1] += G; cluster_sum[best_k[1]][2] += B; 
            cluster_count[best_k[1]] ++;
            quality += best_k[0];
        }
    }

    // Get centre of each cluster
    for (int i = 0; i < k; i ++){
        if (cluster_count[i] == 0){
            cluster_sum[i][0] = 0; cluster_sum[i][1] = 0; cluster_sum[i][2] = 0;
        }else{
            for (int j = 0; j < 3; j++){cluster_sum[i][j] = (int) cluster_sum[i][j]/cluster_count[i];}
        }
    }

    // Compare to the original one
    int similar = 0; int threshold = 30;
    for (int i = 0; i < k; i++){
        for (int j = 0; j < k; j++){
            int R_dist = k_centers[i][0] - cluster_sum[j][0]; int G_dist = k_centers[i][1] - cluster_sum[j][1]; int B_dist = k_centers[i][2] - cluster_sum[j][2];
            int total_dist = (int) sqrt(R_dist * R_dist + G_dist * G_dist + B_dist * B_dist);
            if (total_dist <= threshold){similar ++;}
        }
    }

    if (similar < k){
        for (int i = 0; i < k; i ++){replace_k_center(i, cluster_sum[i][0], cluster_sum[i][1], cluster_sum[i][2]);}

        if (loop == 100){k_centers_init();}

        if (loop >= 200){
            if (k > best_k){k = k-1;}
            k_centers_init();
            loop = 0;
        }
        goto restart;
    }

    // Check for repeat & zero
    for (int i = 0; i < k; i++){
        for (int j = 0; j < k; j++){
            if (i != j){
                if (k_centers[i][0] == k_centers[j][0] && k_centers[i][1] == k_centers[j][1] && k_centers[i][2] == k_centers[j][2]){
                    k_centers_init();
                    goto restart;
                }
            }
        }
        if (k_centers[i][0] == 0 && k_centers[i][1] == 0 && k_centers[i][2] == 0){
            k_centers_init();
            goto restart;
        }
    }
}

void k_means_clustering::do_k_clustering(Image& the_image, int k_input){
    cout << endl << "K Clustering happening" << endl;
    int loop = 10;
    int result[loop][k][3];int index[loop];
    unsigned long int quality_list[loop];
    
    for (int i = 0; i < loop; i++){
        k_clustering_single_run(the_image);
        cout << "Loop: " << i+1 << ". K of this loop: " << k << endl;
        if (k > best_k) {best_k = k;}
        for (int j = 0; j < k; j++){
            result[i][j][0] = k_centers[j][0]; result[i][j][1] = k_centers[j][1]; result[i][j][2] = k_centers[j][2]; 
        }
        index[i] = k;quality_list[i] = quality;
    }

    // Highest K and lowest quality
    int best = 0;
    for (int i = 0; i < loop; i++){
        if (index[i] == best_k){
            if (best == 0) {
                best = i;
            }else{
                if (quality_list[i] < quality_list[best]){best = i;}
            }
        }
    }

    k = index[best];
    for (int i = 0; i < k; i++){replace_k_center(i, result[best][i][0], result[best][i][1], result[best][i][2]);}
    return;
}