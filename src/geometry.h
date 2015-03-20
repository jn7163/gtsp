#ifndef _PROJECT_GEOMETRY_H_
#define _PROJECT_GEOMETRY_H_

int generate_random_points_2d(
        int node_count,
        int grid_size,
        double *x_coordinates,
        double *y_coordinates);

int generate_random_clusters_2d(
        int node_count,
        int cluster_count,
        int grid_size,
        double *x_coordinates,
        double *y_coordinates,
        int *clusters);

int get_euclidean_distance(
        double *x_coordinates,
        double *y_coordinates,
        int p1_index,
        int p2_index);

#endif //_PROJECT_GEOMETRY_H_