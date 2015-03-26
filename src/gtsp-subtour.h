#ifndef _PROJECT_GTSP_SUBTOUR_H_
#define _PROJECT_GTSP_SUBTOUR_H_

#include "gtsp.h"

void deactivate_cluster_node(double *capacities, struct Node *cluster_node);

void activate_cluster_node(double *capacities, struct Node *cluster_node);


int find_exact_subtour_cuts_cluster_to_cluster(
        struct LP *lp,
        struct GTSP *data,
        struct Graph *digraph,
        double *capacities,
        int *added_cuts_count);

int find_exact_subtour_cuts_node_to_cluster(
        struct LP *lp,
        struct GTSP *data,
        double *x,
        struct Graph *digraph,
        double *capacities,
        int *added_cuts_count);

int find_exact_subtour_cuts_node_to_node(
        struct LP *lp,
        struct GTSP *data,
        double *x,
        struct Graph *digraph,
        double *capacities,
        int *added_cuts_count);

int find_exact_subtour_cuts(
        struct LP *lp, struct GTSP *data, int *total_added_cuts);


#endif //_PROJECT_GTSP_SUBTOUR_H_
