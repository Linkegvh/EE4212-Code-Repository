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

using namespace std;

class k_graph{
    private:
        int ** cluster_center;
        int ** graph_matrix;
        int total_number_of_clusters;
        int converged_number_of_clusters;
        int w;
        int h;
        int matrix_size;
        unsigned long int quality_number;
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
            graph_matrix = new int* [h];
            for (int i = 0; i < h; i ++){
                graph_matrix[i] = new int[w];
                // initialise the adjacency matrix to all belonging to ID 0 of the cluster center first
                for (int j = 0; j < w; j ++){
                    graph_matrix[i][j] = 0;
                }
            }

            this->converged_number_of_clusters = 0;
            return;
        };

        void modify_cluster_center(int ID, int R, int G, int B);
        int** retrieve_cluster_center() {return cluster_center;};
        void modify_graph_node(int h, int w, int ID);
        int retrieve_graph_node(int h, int w){return graph_matrix[h][w];};
        void cluster_initialisation();
        void modify_total_number_of_cluster(int new_value){this->total_number_of_clusters = new_value;};
        int retrieve_total_number_of_cluster(){return total_number_of_clusters;};
        void modify_quality_number(unsigned long int number){this->quality_number = number;};
        unsigned long int retrieve_quality_number(){return quality_number;};
        void modify_converged_k(int number){this->converged_number_of_clusters = number;};
        int retrieve_converged_num_of_k(){return converged_number_of_clusters;};
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
        for (int i = 0; i < total_number_of_clusters; i++){
            for (int j = 0; j < total_number_of_clusters; j++){
                if (i != j){
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
        }
        
        if (repeated_color == 1) {
            //cout << "Got repeated, reinitialising now\n";
        }else{
            //cout << "No Repeated\n";
        }
    }
}