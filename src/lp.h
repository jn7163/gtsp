#ifndef _PROJECT_LP_H_
#define _PROJECT_LP_H_

#include <cplex.h>

#define LP_EPSILON 0.000001

struct LP
{
    CPXENVptr cplex_env;
    CPXLPptr cplex_lp;
};

static const int MAX_NAME_LENGTH = 100;

int LP_open(struct LP *lp);

int LP_create(struct LP *lp, const char *name);

int LP_write(struct LP *lp, const char *fname);

void LP_free(struct LP *lp);

int LP_new_row(struct LP *lp, char sense, double rhs);

int LP_add_rows(
        struct LP *lp,
        int newrows,
        int newnz,
        double *rhs,
        char *sense,
        int *rmatbeg,
        int *rmatind,
        double *rmatval);

int LP_add_cols(
        struct LP *lp,
        int newcols,
        int newnz,
        double *obj,
        int *cmatbeg,
        int *cmatind,
        double *cmatval,
        double *lb,
        double *ub);

int LP_change_bound(struct LP *lp, int col, char lower_or_upper, double bnd);

int LP_optimize(struct LP *lp, int *infeasible);

int LP_get_obj_val(struct LP *lp, double *obj);

int LP_get_x(struct LP *lp, double *x);

int LP_get_num_cols(struct LP *lp);

#endif