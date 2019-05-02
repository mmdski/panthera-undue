#include <cii/mem.h>
#include <math.h>
#include <panthera/exceptions.h>
#include <panthera/secantsolve.h>

SecantSolution *
secant_solve (int              max_iterations,
              double           eps,
              SecantSolverFunc func,
              void *           func_data,
              double           x_0)
{
    if (max_iterations < 1)
        RAISE (value_arg_error);

    int    i;
    double x_new;
    double x_computed;
    bool   solution_found;

    SecantSolution *solution;

    double *assumed =
        Mem_calloc (max_iterations, sizeof (double), __FILE__, __LINE__);
    double *error =
        Mem_calloc (max_iterations, sizeof (double), __FILE__, __LINE__);
    double assumed_diff;
    double error_diff;

    x_new = x_0;

    solution_found = false;

    for (i = 0; i < max_iterations; i++) {
        x_computed = func (x_new, func_data);

        *(error + i)   = fabs (x_computed - x_new);
        *(assumed + i) = x_new;

        if (*(error + i) <= eps) {
            solution_found = true;
            break;
        }

        if (i == 0) {
            x_new = x_0 + 0.7 * (x_computed - x_0);
        } else {
            assumed_diff = *(assumed + i - 1) - *(assumed + i);
            error_diff   = *(error + i - 1) - *(error + i);
            x_new        = *(assumed + i - 1) -
                    *(error + i - 1) * assumed_diff / error_diff;
        }
    }

    NEW (solution);
    solution->n_iterations   = i;
    solution->solution_found = solution_found;
    solution->x_computed     = x_computed;

    Mem_free (assumed, __FILE__, __LINE__);
    Mem_free (error, __FILE__, __LINE__);

    return solution;
}
