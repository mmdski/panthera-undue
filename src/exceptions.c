#include <panthera/exceptions.h>

const Except_T null_ptr_arg_error     = {"Invalid NULL pointer as argument"};
const Except_T value_arg_error        = {"Invalid value as argument"};
const Except_T coarray_n_coords_error = {"Too few coordinates"};
const Except_T coarray_z_order_error  = {"Invalid z-value order"};
const Except_T index_error            = {"Index out of bounds"};
const Except_T empty_table_error      = {"Unsupported operation on empty"
                                    " table"};
const Except_T reach_x_order_error    = {"Invalid x-value order"};
const Except_T reach_xs_num_error     = {"Invalid cross section number"};
const Except_T max_iteration_error    = {"Maximum number of iterations reach"};
const Except_T xsp_depth_error        = {"Depth is less than minimum y-value"
                                  " in cross section."};
Except_T       compute_fail_error     = {"Compute failed"};
