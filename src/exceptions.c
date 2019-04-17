#include <panthera/exceptions.h>

const Except_T null_ptr_arg_Error     = {"Invalid NULL pointer as argument"};
const Except_T value_arg_Error        = {"Invalid value as argument"};
const Except_T coarray_n_coords_Error = {"Too few coordinates"};
const Except_T coarray_z_order_Error  = {"Invalid z-value order"};
const Except_T index_Error            = {"Index out of bounds"};
const Except_T empty_table_Error      = {"Unsupported operation on empty"
                                         " table"};
const Except_T reach_x_order_Error    = {"Invalid x-value order"};
const Except_T reach_xs_num_Error     = {"Invalid cross section number"};
const Except_T max_iteration_Error    = {"Maximum number of iterations reach"};
