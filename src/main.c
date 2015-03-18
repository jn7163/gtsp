#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "lp.h"
#include "util.h"
#include "main.h"
#include "tsp.h"
#include "branch_and_cut.h"

char *INPUT_FILENAME = 0;
unsigned int SEED = 0;
int GEOMETRIC_DATA = 0;
int NODE_COUNT_RAND = 0;
int GRID_SIZE_RAND = 100;

static int parse_arguments(int ac, char **av);

static void print_usage(char *f);

int main(int ac, char **av)
{
    int rval = 0;

    SEED = (unsigned int) get_current_time();

    rval = parse_arguments(ac, av);
    ABORT_IF(rval, "Failed to parse arguments.\n");

    printf("Seed = %d\n", SEED);
    srand(SEED);

    struct BNC bnc;
    struct TSPData data;

    TSP_init_data(&data);

    rval = TSP_read_problem(INPUT_FILENAME, &data);
    ABORT_IF(rval, "TSP_read_problem failed\n");

    rval = BNC_init(&bnc);
    ABORT_IF(rval, "BNC_init failed");

    bnc.best_val = TSP_find_initial_solution(&data);
    bnc.problem_data = (void *) &data;
    bnc.problem_init_lp = (int (*)(struct LP *, void *)) TSP_init_lp;
    bnc.problem_add_cutting_planes =
            (int (*)(struct LP *, void *)) TSP_add_cutting_planes;

    rval = BNC_init_lp(&bnc);
    ABORT_IF(rval, "BNC_init_lp failed");

    rval = BNC_solve(&bnc);
    ABORT_IF(rval, "BNC_solve_node failed\n");

    time_printf("Optimal integral solution:\n");
    time_printf("    obj value = %.2lf **\n", bnc.best_val);

    CLEANUP:
    BNC_free(&bnc);
    TSP_free_data(&data);
    return rval;
}

static int parse_arguments(int ac, char **av)
{
    int rval = 0;

    int c;

    if (ac == 1)
    {
        print_usage(av[0]);
        return 1;
    }

    while ((c = getopt(ac, av, "ab:gk:s:")) != EOF)
    {
        switch (c)
        {
            case 'a':;
                break;
            case 'b':
                GRID_SIZE_RAND = atoi(optarg);
                break;
            case 'g':
                GEOMETRIC_DATA = 1;
                break;
            case 'k':
                NODE_COUNT_RAND = atoi(optarg);
                break;
            case 's':
                SEED = (unsigned) atoi(optarg);
                break;
            case '?':
            default:
                print_usage(av[0]);
                return 1;
        }
    }

    if (optind < ac) INPUT_FILENAME = av[optind++];

    if (optind != ac)
    {
        print_usage(av[0]);
        return 1;
    }

    ABORT_IF(!INPUT_FILENAME && !NODE_COUNT_RAND,
            "Must specify an input file or use -k for random problem\n");

    CLEANUP:
    return rval;
}

static void print_usage(char *f)
{
    fprintf(stderr, "Usage: %s [-see below-] [prob_file]\n"
            "   -a    add all subtours cuts at once\n"
            "   -b d  gridsize d for random problems\n"
            "   -g    prob_file has x-y coordinates\n"
            "   -k d  generate problem with d cities\n"
            "   -s d  random SEED\n", f);
}

