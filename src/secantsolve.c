#include <math.h>
#include <panthera/cii/mem.h>
#include <panthera/exceptions.h>
#include <panthera/secantsolve.h>

SecantSolution *
secant_solve (int              max_iterations,
              double           eps,
              SecantSolverFunc func,
              void *           func_data,
              double           x_0,
              double           x_1)
{
    if (max_iterations < 1)
        RAISE (value_arg_error);

    int    i;
    double delta;
    double x_diff;
    double y_diff;
    double x_computed     = NAN;
    bool   solution_found = false;

    SecantSolution *solution;

    double *x =
        Mem_calloc (max_iterations, sizeof (double), __FILE__, __LINE__);
    double *y =
        Mem_calloc (max_iterations, sizeof (double), __FILE__, __LINE__);

    *(x + 0) = x_0;
    *(x + 1) = x_1;

    *(y + 0) = func (x_0, func_data);
    *(y + 1) = func (x_1, func_data);

    for (i = 2; i < max_iterations; i++) {

        x_diff = *(x + i - 1) - *(x + i - 2);
        y_diff = *(y + i - 1) - *(y + i - 2);

        *(x + i) = *(x + i - 1) - *(y + i - 1) * x_diff / y_diff;
        *(y + i) = func (*(x + i), func_data);

        delta = fabs (*(x + i) - *(x + i - 1));

        if (delta <= eps) {
            solution_found = true;
            x_computed     = *(x + i);
            break;
        }
    }

    NEW (solution);
    solution->n_iterations   = i;
    solution->solution_found = solution_found;
    solution->x_computed     = x_computed;

    Mem_free (x, __FILE__, __LINE__);
    Mem_free (y, __FILE__, __LINE__);

    return solution;
}
