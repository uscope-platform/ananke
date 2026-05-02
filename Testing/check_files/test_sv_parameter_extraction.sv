`timescale 10 ns / 1 ns

module test_mod #(
    parameter simple_numeric_p = 32,
    sv_numeric_p = 5'o10,
    dimensionless_sv_numeric_p  = 'h3F,
    parameter [31:0] array_parameter [1:0] = '{32, 5},
    parameter repetition_size = 2,
    parameter [31:0] multidim_array_parameter [repetition_size-1:0][1:0] = '{{32,32}, {5,6}},
    parameter bit repetition_parameter [1:0]  = '{repetition_size{1}},
    parameter bit multi_repetition_parameter [3:0]  = '{{repetition_size{1}},{repetition_size{4}}},
    parameter bit mixed_repetition_parameter [3:0]  = '{1,2,{repetition_size{4}}},
    parameter logic [7:0] packed_param = {1'b1,1'b0,1'b1,1'b0,1'b1,1'b0,1'b0,1'b1},
    parameter package_param = test_package::bus_base,
    parameter negative_param = -16'sd32767,
    parameter negative_array_param [1:0] = '{-16'sd32767, 16'sd32767},
    string_p = "423",
    nested_p = string_p
)();

    parameter add_expr_p = simple_numeric_p + sv_numeric_p;
    parameter sub_expr_p = simple_numeric_p - sv_numeric_p;
    parameter mul_expr_p = simple_numeric_p * sv_numeric_p;
    parameter div_expr_p = simple_numeric_p / sv_numeric_p;
    parameter modulo_expr_p = simple_numeric_p % sv_numeric_p;
    parameter chained_expression = add_expr_p + mul_expr_p  * 5;
    parameter simple_log_expr_p = $clog2(add_expr_p);
    parameter complex_log_expr_p = $clog2(add_expr_p+2);
    parameter parenthesised_expr_p = ( add_expr_p + mul_expr_p ) * 5;
    parameter array_parameter_expr_p = array_parameter[sv_numeric_p*0] + array_parameter[1];
    localparam local_p = 74;
    parameter multidim_array_access = multidim_array_parameter[1][0];

endmodule
