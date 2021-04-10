/**
 * Written by Lingke Ding
*/


#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <ctime>

using namespace std;

class k_graph{
    private:
        int ** cluster_center;
        int ** graph_matrix;
        int total_number_of_clusters;
        int w;
        int h;
        int matrix_size;
    public:
        k_graph(int h, int w, int number_of_clusters){
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
            graph_matrix = new int* [w];
            for (int i = 0; i < h; i ++){
                graph_matrix[i] = new int[w];
                // initialise the adjacency matrix to all belonging to ID 0 of the cluster center first
                for (int j = 0; j < w; j ++){
                    graph_matrix[i][j] = 0;
                }
            }
            return;
        };

        void modify_cluster_center(int ID, int R, int G, int B);
        int** retrieve_cluster_center() {return cluster_center;};
        void modify_graph_node(int h, int w, int ID);
        int retrieve_graph_node(int h, int w){return graph_matrix[h][w];};
        void cluster_initialisation();
};

void k_graph::modify_cluster_center(int ID, int R, int G, int B){
    cluster_center[ID][0] = R;
    cluster_center[ID][1] = G;
    cluster_center[ID][2] = B;
}

void k_graph::modify_graph_node(int h, int w, int ID){
    graph_matrix[h][w] = ID;
}

void k_graph::cluster_initialisation(){
    srand(time(0));
    int repeated_color = 1;
    while (repeated_color == 1){
        for (int i = 0; i < total_number_of_clusters; i ++){
            cluster_center[i][0] = rand() % 256;
            cluster_center[i][1] = rand() % 256;
            cluster_center[i][2] = rand() % 256;
        }
        //cout << "Cluster centered initialised ... Checking if got repeated\n";
        // Use a stupid method to compare (anyway we won't be comparing a large dataset)
        repeated_color = 0;
        for (int i = 0; i < total_number_of_clusters - 1; i++){
            for (int j = 1; j < total_number_of_clusters; j++){
                // R
                if (cluster_center[i][0] == cluster_center[j][0]){
                    // G
                    if (cluster_center[i][1] == cluster_center[j][1]){
                        // B
                        if (cluster_center[i][2] == cluster_center[j][2]){
                            repeated_color = 1;
                        }
                    }
                }
            }
        }
        
        if (repeated_color == 1) {
            //cout << "Got repeated, reinitialising now\n";
        }else{
            //cout << "No Repeated\n";
        }
    }
}