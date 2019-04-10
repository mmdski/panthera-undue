#include <panthera/reach.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main(void) {

    /* cross section parameters */
    int n                   = 9;
    double y[]              = {1,  0.5,   0,  0.5, 1,  0.5,   0,  0.5, 1};
    double z[]              = {0, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2};
    double n_roughness      = 3;
    double roughness[]      = {0.05, 0.01, 0.05};
    double z_roughness[]    = {0.75, 1.25};
    CrossSection xs;

    int i;
    int del_i;
    int n_x = 10;
    double random_x;
    double *x_array = NULL;

    srand(time(NULL));

    /* initialize a coordinate array and reach */
    CoArray ca = coarray_new(n, y, z);
    Reach reach = reach_new();

    /* add 5 cross sections to the reach at the distances defined in x */
    for (i = 0; i < n_x; i++) {
        random_x = (float)rand()/(float)(RAND_MAX/1e3);
        printf("Adding cross section at x = %f\n", random_x);
        xs = xs_new(ca, n_roughness, roughness, z_roughness);
        reach_put(reach, random_x, xs);
        printf("\tn_x = %i\n", reach_size(reach));
    }

    /* add a cross section at the location already contained in the reach */
    xs = xs_new(ca, n_roughness, roughness, z_roughness);
    reach_put(reach, random_x, xs);
    printf("The reach contains %i cross sections\n", reach_size(reach));

    /* print the stream distances of cross sections in ascending order */
    n_x = reach_stream_distance(reach, &x_array);
    printf("n_x = %i\n", n_x);
    for (i = 0; i < n_x; i++)
        printf("x = %f\n", *(x_array + i));

    /* delete most of the cross sections in random order */
    while (reach_size(reach) > n_x/2) {
        del_i = rand() % n_x;
        if (reach_contains(reach, x_array[del_i])) {
            printf("Deleting cross section at x = %f\n", x_array[del_i]);
            reach_delete(reach, x_array[del_i]);
            printf("%i cross sections remaining\n", reach_size(reach));
        }
    }

    /* free x_array and set it to NULL */
    Mem_free(x_array, __FILE__, __LINE__);
    x_array = NULL;

    /* add cross sections to the reach at random stream distances */
    n_x = 4*n_x;
    while (reach_size(reach) < n_x) {
        random_x = (float)rand()/(float)(RAND_MAX/1e3);
        printf("Adding cross section at x = %f\n", random_x);
        xs = xs_new(ca, n_roughness, roughness, z_roughness);
        reach_put(reach, random_x, xs);
        printf("\tn_x = %i\n", reach_size(reach));
    }

    /* print the stream distances in ascending order */
    n_x = reach_stream_distance(reach, &x_array);
    printf("n_x = %i\n", n_x);
    for (i = 0; i < n_x; i++)
        printf("x = %f\n", *(x_array + i));

    /* remove all cross sections */
    while (reach_size(reach) > 0) {
        del_i = rand() % n_x;
        if (reach_contains(reach, x_array[del_i])) {
            printf("Deleting cross section at x = %f\n", x_array[del_i]);
            reach_delete(reach, x_array[del_i]);
            printf("%i cross sections remaining\n", reach_size(reach));
        }
    }

    Mem_free(x_array, __FILE__, __LINE__);
    coarray_free(ca);
    reach_free(reach);

    return 0;
}
