//  Copyright 2026 Filippo Savi
//  Author: Filippo Savi <filssavi@gmail.com>
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.


#include <gtest/gtest.h>

#include "frontend/analysis/system_verilog/sv_analyzer.hpp"

TEST(sv_coverage_parsing, cross_with_hierarchical_references) {
    // Caliptra aes_cov_if.sv shape: commercial simulators accept hierarchical
    // references in a cross list (implicit coverpoints), but the grammar only
    // allowed plain identifiers, failing with `mismatched input '.'`.
    auto test_pattern = R"(
        interface aes_cov_if (
            input logic clk
        );
            covergroup aes_ocp_lock_flow_grp @(posedge clk);
                ocp_lock_in_progress_cp:  coverpoint ocp_lock_in_progress;
                output_is_kv_mek_slot_cp: coverpoint output_is_kv_mek_slot;
                ocp_lock_flow_mek_dec_X: cross ocp_lock_in_progress_cp,
                                               output_is_kv_mek_slot_cp;
                ocp_lock_flow_kv_write_ctx_clear_X: cross aes2caliptra.kv_data_out_valid, caliptra2aes.clear_secrets, aes_inst.reg2hw_caliptra.trigger.data_out_clear.q;
            endgroup
        endinterface
    )";

    sv_analyzer analyzer;

    auto result = analyzer.analyze("", test_pattern);
    ASSERT_TRUE(result.has_value()) << analyzer.get_error();
    EXPECT_FALSE(analyzer.has_syntax_errors());
}

TEST(sv_coverage_parsing, cross_with_plain_coverpoints) {
    // Strictly LRM-compliant shape keeps working.
    auto test_pattern = R"(
        module test_mod (
            input logic clk, input logic a, input logic b
        );
            covergroup cg @(posedge clk);
                a_cp: coverpoint a;
                b_cp: coverpoint b;
                ab_x: cross a_cp, b_cp;
            endgroup
        endmodule
    )";

    sv_analyzer analyzer;

    auto result = analyzer.analyze("", test_pattern);
    ASSERT_TRUE(result.has_value()) << analyzer.get_error();
    EXPECT_FALSE(analyzer.has_syntax_errors());
}
