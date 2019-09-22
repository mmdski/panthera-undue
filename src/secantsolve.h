#ifndef SECANT_SOLVE_INCLUDED
#define SECANT_SOLVE_INCLUDED

#include <stdbool.h>

/**
 * SECTION: secantsolve.h
 * @short_description: Secant method solver
 * @title: Secant method solver
 *
 * Secant method solver
 */

/**
 * SecantSolution:
 * @solution_found: indicates if a solution has been found
 * @n_iterations:   the number of iterations taken during the solution
 * @x_computed:     the computed x value of the solution
 *
 * Secant solver solution
 */
typedef struct {
    bool   solution_found;
    int    n_iterations;
    double x_computed;
} SecantSolution;

/**
 * SecantSolverFunc:
 * @x:         x-value of function to compute
 * @func_data: data used for computation of solution
 *
 * Solver compute function
 *
 * Returns: computed function
 */
typedef double (*SecantSolverFunc)(double x, void *func_data);

/**
 * secant_solve:
 * @max_iterations: the maximum number of iterations in a solution
 * @eps:            acceptable error for a solution
 * @func:           a #SecantSolverFunc
 * @func_data:      data used by @func for computing a solution
 * @x_0:            initial value of x
 * @x_1:            second value of x
 *
 * Returns: a numerical solution using the secant method
 */
SecantSolution *
secant_solve(int              max_iterations,
             double           eps,
             SecantSolverFunc func,
             void *           func_data,
             double           x_0,
             double           x_1);

#endif
