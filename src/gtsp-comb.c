/* Copyright (c) 2015 Alinson Xavier
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gtsp.h"
#include "util.h"
#include <assert.h>

static int add_comb_cut(
        struct LP *lp,
        struct Graph *graph,
        int current_component,
        int *clusters,
        int *components,
        int *component_sizes,
        int *teeth,
        int tooth_count,
        double *x)
{
    int rval = 0;

    char sense = 'G';

    const int node_count = graph->node_count;
    const int edge_count = graph->edge_count;

    struct Row *cut = 0;
    int *rmatind = 0;
    double *rmatval = 0;

    rmatind = (int *) malloc((node_count + edge_count) * sizeof(int));
    rmatval = (double *) malloc((node_count + edge_count) * sizeof(double));
    abort_if(!rmatind, "could not allocate rmatind");
    abort_if(!rmatval, "could not allocate rmatval");

    cut = (struct Row *) malloc(sizeof(struct Row));
    cut->edges = (char *) malloc(graph->edge_count * sizeof(char));

    abort_if(!cut, "could not allocate cut");
    abort_if(!cut->edges, "could not allocate cut->edges");

    for (int i = 0; i < graph->edge_count; i++)
        cut->edges[i] = 0;

    double rhs = -component_sizes[current_component] - tooth_count +
            (tooth_count + 1) / 2;

    int nz = 0;

    // Edges inside handle
    for (int i = 0; i < edge_count; i++)
    {
        struct Edge *e = &graph->edges[i];
        if (components[clusters[e->from->index]] != current_component) continue;
        if (components[clusters[e->to->index]] != current_component) continue;

        cut->edges[e->index] = 1;
        if (e->column < 0) continue;

        rmatind[nz] = e->column;
        rmatval[nz] = -1.0;
        nz++;

        log_verbose("  handle (%d %d)\n", e->from->index, e->to->index);
    }

    // Edges inside each tooth
    for (int i = 0; i < edge_count; i++)
    {
        struct Edge *e = &graph->edges[i];
        struct Node *from = e->from;
        struct Node *to = e->to;

        if (teeth[clusters[from->index]] < 0) continue;
        if (teeth[clusters[to->index]] < 0) continue;
        if (teeth[clusters[from->index]] != teeth[clusters[to->index]])
            continue;

        cut->edges[e->index] = 1;
        if (e->column < 0) continue;

        log_verbose("  tooth (%d %d)\n", e->from->index, e->to->index);

        rmatind[nz] = e->column;
        rmatval[nz] = -1.0;
        nz++;


    }

#if LOG_LEVEL >= LOG_LEVEL_VERBOSE
    log_verbose("Generated cut:\n");
    if (OPTIMAL_X)
    {
        for (int i = 0; i < nz; i++)
        {
            if (OPTIMAL_X[rmatind[i]] < EPSILON) continue;

            if (rmatind[i] >= node_count)
            {
                struct Edge *e = &graph->edges[rmatind[i] - node_count];
                log_verbose("    %.2lf x%d (%d %d %.4lf)\n", rmatval[i],
                        rmatind[i], e->from->index, e->to->index,
                        OPTIMAL_X[rmatind[i]]);
            }
            else
            {
                log_verbose("    %.2lf x%d (%.4lf)\n", rmatval[i], rmatind[i],
                        OPTIMAL_X[rmatind[i]]);
            }
        }
        log_verbose("    %c %.2lf\n", sense, rhs);
    }
#endif

    if (OPTIMAL_X)
    {
        double sum = 0;
        for (int i = 0; i < nz; i++)
            sum += rmatval[i] * OPTIMAL_X[rmatind[i]];
        log_verbose("%.2lf >= %.2lf\n", sum, rhs);
        abort_if(sum <= rhs - EPSILON, "cannot add invalid cut");
    }

    double lhs = 0.0;
    for (int i = 0; i < nz; i++)
        lhs += rmatval[i] * x[rmatind[i]];

    log_verbose("Violation: %.4lf >= %.4lf\n", lhs, rhs);

//    if (lhs + EPSILON > rhs)
//    {
//        free(rmatind);
//        free(rmatval);
//        goto CLEANUP;
//    }

    cut->nz = nz;
    cut->sense = sense;
    cut->rhs = rhs;
    cut->rmatval = rmatval;
    cut->rmatind = rmatind;
    cut->edge_val = -1.0;
    cut->edge_count = graph->edge_count;

    rval = LP_add_cut(lp, cut);
    abort_if(rval, "LP_add_cut failed");

    CLEANUP:
    return rval;
}

static int find_components(
        struct Graph *graph, double *x, int *components, int *component_sizes)
{
    int rval = 0;
    struct Node **stack = 0;

    const int node_count = graph->node_count;

    for (int i = 0; i < node_count; i++)
    {
        components[i] = -1;
        graph->nodes[i].mark = 0;
    }

    int stack_top = 0;
    stack = (struct Node **) malloc(node_count * sizeof(struct Node *));
    abort_if(!stack, "could not allocate stack");

    for (int i = 0; i < node_count; i++)
    {
        struct Node *root = &graph->nodes[i];
        if (root->mark) continue;

        stack[stack_top++] = root;

        while (stack_top > 0)
        {
            struct Node *n = stack[--stack_top];
            components[n->index] = i;

            for (int j = 0; j < n->degree; j++)
            {
                struct Adjacency *adj = &n->adj[j];
                struct Node *neighbor = adj->neighbor;

                if (neighbor->mark) continue;

                double x_e = x[adj->edge->index];

                if (x_e < EPSILON) continue;
                if (x_e > 1 - EPSILON) continue;

                stack[stack_top++] = neighbor;
                neighbor->mark = 1;
            }
        }
    }

    for (int i = 0; i < node_count; i++)
        component_sizes[i] = 0;

    for (int i = 0; i < node_count; i++)
        component_sizes[components[i]]++;

    log_verbose("Components:\n");
    for (int i = 0; i < graph->node_count; i++)
            log_verbose("    %d %d\n", i, components[i]);

    log_verbose("Component sizes:\n");
    for (int i = 0; i < graph->node_count; i++)
            log_verbose("    %d %d\n", i, component_sizes[i]);

    CLEANUP:
    if (stack) free(stack);
    return rval;
}

static int find_teeth(
        struct Graph *graph,
        double *x,
        int current_component,
        int *components,
        int *teeth,
        int *tooth_count)
{
    const int node_count = graph->node_count;
    const int edge_count = graph->edge_count;

    for (int i = 0; i < node_count; i++)
    {
        graph->nodes[i].mark = 0;
        teeth[i] = -1;
    }

    *tooth_count = 0;

    for (int i = 0; i < edge_count; i++)
    {
        struct Edge *e = &graph->edges[i];
        struct Node *from = e->from;
        struct Node *to = e->to;

        if (x[e->index] < 1 - EPSILON) continue;
        if (to->mark || from->mark) continue;

        int z = 0;
        if (components[from->index] == current_component) z++;
        if (components[to->index] == current_component) z++;
        if (z != 1) continue;

        to->mark = 1;
        from->mark = 1;

        teeth[to->index] = *tooth_count;
        teeth[from->index] = *tooth_count;

        (*tooth_count)++;
    }

    return 0;
}

static int shrink_clusters(
        const struct GTSP *data,
        double *x,
        struct Graph *shrunken_graph,
        double *shrunken_x)
{
    int rval = 0;

    double *x_coords = 0;
    double *y_coords = 0;
    int *cluster_sizes = 0;

    const int *clusters = data->node_to_cluster;
    const int cluster_count = data->cluster_count;
    const struct Graph *graph = data->graph;

    int *edges = 0;
    int *edge_map = 0;
    int edge_count = (cluster_count * (cluster_count - 1)) / 2;

    edge_map = (int *) malloc(cluster_count * cluster_count * sizeof(int));
    abort_if(!edge_map, "could not allocate edge_map");

    edges = (int *) malloc(2 * edge_count * sizeof(int));
    abort_if(!edges, "could not allocate edges");

    cluster_sizes = (int *) malloc(cluster_count * sizeof(int));
    x_coords = (double *) malloc(cluster_count * sizeof(double));
    y_coords = (double *) malloc(cluster_count * sizeof(double));

    abort_if(!cluster_sizes, "could not allocate cluster_sizes");
    abort_if(!x_coords, "could not allocate x_coords");
    abort_if(!y_coords, "could not allocate y_coords");

    for (int i = 0; i < cluster_count; i++)
    {
        x_coords[i] = 0.0;
        y_coords[i] = 0.0;
        cluster_sizes[i] = 0;
    }

    for (int i = 0; i < graph->node_count; i++)
    {
        struct Node *n = &graph->nodes[i];
        int c = clusters[n->index];

        cluster_sizes[c]++;
        x_coords[c] += graph->x_coordinates[n->index];
        y_coords[c] += graph->y_coordinates[n->index];
    }

    for (int i = 0; i < cluster_count; i++)
    {
        x_coords[i] = x_coords[i] / cluster_sizes[i];
        y_coords[i] = y_coords[i] / cluster_sizes[i];
    }

    shrunken_graph->x_coordinates = x_coords;
    shrunken_graph->y_coordinates = y_coords;

    int curr_edge = 0;
    for (int i = 0; i < cluster_count; i++)
    {
        for (int j = i + 1; j < cluster_count; j++)
        {
            edges[curr_edge * 2] = i;
            edges[curr_edge * 2 + 1] = j;
            edge_map[i * cluster_count + j] = curr_edge;
            edge_map[j * cluster_count + i] = curr_edge;
            curr_edge++;
        }
    }

    assert(curr_edge == edge_count);

    rval = graph_build(cluster_count, edge_count, edges, 0, shrunken_graph);
    abort_if(rval, "graph_build failed");

    for (int i = 0; i < edge_count; i++)
        shrunken_x[i] = 0.0;

    for (int i = 0; i < graph->edge_count; i++)
    {
        struct Edge *e = &graph->edges[i];
        if(e->column < 0) continue;

        int from = clusters[e->from->index];
        int to = clusters[e->to->index];
        int shunk_e_index = edge_map[from * cluster_count + to];

        shrunken_x[shunk_e_index] += x[e->column];
    }

    CLEANUP:
    if (edges) free(edges);
    if (edge_map) free(edge_map);
    if (cluster_sizes) free(cluster_sizes);
    return rval;
}

int GTSP_find_comb_cuts(struct LP *lp, struct GTSP *data)
{
    int rval = 0;
    double initial_time = get_user_time();

    double *x = 0;
    double *shrunken_x = 0;

    int *teeth = 0;
    int *components = 0;
    int *component_sizes = 0;

    int num_cols = LP_get_num_cols(lp);
    x = (double *) malloc(num_cols * sizeof(double));
    abort_if(!x, "could not allocate x");

    rval = LP_get_x(lp, x);
    abort_if(rval, "LP_get_x failed");

    struct Graph shrunken_graph;
    graph_init(&shrunken_graph);

    const int cluster_count = data->cluster_count;
    const int shrunken_edge_count = (cluster_count * (cluster_count - 1)) / 2;

    shrunken_x = (double *) malloc(shrunken_edge_count * sizeof(double));
    abort_if(!shrunken_x, "could not allocate shrunken_x");

    rval = shrink_clusters(data, x, &shrunken_graph, shrunken_x);
    abort_if(rval, "shrink_clusters failed");

    teeth = (int *) malloc(cluster_count * sizeof(int));
    components = (int *) malloc(cluster_count * sizeof(int));
    component_sizes = (int *) malloc(cluster_count * sizeof(int));

    abort_if(!teeth, "could not allocate teeth");
    abort_if(!components, "could not allocate components");
    abort_if(!component_sizes, "could not allocate component_sizes");

    rval = find_components(&shrunken_graph, shrunken_x, components,
            component_sizes);
    abort_if(rval, "find_components failed");

    int original_cut_pool_size = lp->cut_pool_size;

    for (int i = 0; i < cluster_count; i++)
    {
        if (component_sizes[i] < 3) continue;

        int tooth_count;
        rval = find_teeth(&shrunken_graph, shrunken_x, i, components, teeth,
                &tooth_count);
        abort_if(rval, "find_teeth failed");

        log_verbose("Component %d has %d teeth:\n", i, tooth_count);
        for (int j = 0; j < cluster_count; j++)
        {
            if (teeth[j] < 0) continue;
            log_verbose("    %d %d\n", j, teeth[j]);
        }

        if (tooth_count % 2 == 0)
        {
            for (int k = 0; k < cluster_count; k++)
                if (teeth[k] == tooth_count - 1) teeth[k] = -1;

            tooth_count--;
        }

        rval = add_comb_cut(lp, data->graph, i, data->node_to_cluster,
                components, component_sizes, teeth, tooth_count, x);
        abort_if(rval, "add_comb_cut failed");
    }

    int added_cuts_count = lp->cut_pool_size - original_cut_pool_size;
    log_debug("    %d combs\n", added_cuts_count);

    COMBS_TIME += get_user_time() - initial_time;
    COMBS_COUNT += added_cuts_count;

    CLEANUP:
    graph_free(&shrunken_graph);
    if (teeth) free(teeth);
    if (components) free(components);
    if (component_sizes) free(component_sizes);
    if (shrunken_x) free(shrunken_x);
    if (x) free(x);
    return rval;
}
