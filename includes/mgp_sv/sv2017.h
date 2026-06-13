
// Generated from /Users/filss/git/makefile_gen/grammars/sv2017.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"


namespace mgp_sv {


class  sv2017 : public antlr4::Parser {
public:
  enum {
    KW_DOLAR_ERROR = 1, KW_DOLAR_FATAL = 2, KW_DOLAR_FULLSKEW = 3, KW_DOLAR_HOLD = 4, 
    KW_DOLAR_INFO = 5, KW_DOLAR_NOCHANGE = 6, KW_DOLAR_PERIOD = 7, KW_DOLAR_RECOVERY = 8, 
    KW_DOLAR_RECREM = 9, KW_DOLAR_REMOVAL = 10, KW_DOLAR_ROOT = 11, KW_DOLAR_SETUP = 12, 
    KW_DOLAR_SETUPHOLD = 13, KW_DOLAR_SKEW = 14, KW_DOLAR_TIMESKEW = 15, 
    KW_DOLAR_UNIT = 16, KW_DOLAR_WARNING = 17, KW_DOLAR_WIDTH = 18, KW_1STEP = 19, 
    KW_PATHPULSE_DOLAR = 20, KW_ACCEPT_ON = 21, KW_ALIAS = 22, KW_ALWAYS = 23, 
    KW_ALWAYS_COMB = 24, KW_ALWAYS_FF = 25, KW_ALWAYS_LATCH = 26, KW_AND = 27, 
    KW_ASSERT = 28, KW_ASSIGN = 29, KW_ASSUME = 30, KW_AUTOMATIC = 31, KW_BEFORE = 32, 
    KW_BEGIN = 33, KW_BIND = 34, KW_BINS = 35, KW_BINSOF = 36, KW_BIT = 37, 
    KW_BREAK = 38, KW_BUF = 39, KW_BUFIF0 = 40, KW_BUFIF1 = 41, KW_BYTE = 42, 
    KW_CASE = 43, KW_CASEX = 44, KW_CASEZ = 45, KW_CELL = 46, KW_CHANDLE = 47, 
    KW_CHECKER = 48, KW_CLASS = 49, KW_CLOCKING = 50, KW_CMOS = 51, KW_CONFIG = 52, 
    KW_CONST = 53, KW_CONSTRAINT = 54, KW_CONTEXT = 55, KW_CONTINUE = 56, 
    KW_COVER = 57, KW_COVERGROUP = 58, KW_COVERPOINT = 59, KW_CROSS = 60, 
    KW_DEASSIGN = 61, KW_DEFAULT = 62, KW_DEFPARAM = 63, KW_DESIGN = 64, 
    KW_DISABLE = 65, KW_DIST = 66, KW_DO = 67, KW_EDGE = 68, KW_ELSE = 69, 
    KW_END = 70, KW_ENDCASE = 71, KW_ENDCHECKER = 72, KW_ENDCLASS = 73, 
    KW_ENDCLOCKING = 74, KW_ENDCONFIG = 75, KW_ENDFUNCTION = 76, KW_ENDGENERATE = 77, 
    KW_ENDGROUP = 78, KW_ENDINTERFACE = 79, KW_ENDMODULE = 80, KW_ENDPACKAGE = 81, 
    KW_ENDPRIMITIVE = 82, KW_ENDPROGRAM = 83, KW_ENDPROPERTY = 84, KW_ENDSEQUENCE = 85, 
    KW_ENDSPECIFY = 86, KW_ENDTABLE = 87, KW_ENDTASK = 88, KW_ENUM = 89, 
    KW_EVENT = 90, KW_EVENTUALLY = 91, KW_EXPECT = 92, KW_EXPORT = 93, KW_EXTENDS = 94, 
    KW_EXTERN = 95, KW_FINAL = 96, KW_FIRST_MATCH = 97, KW_FOR = 98, KW_FORCE = 99, 
    KW_FOREACH = 100, KW_FOREVER = 101, KW_FORK = 102, KW_FORKJOIN = 103, 
    KW_FUNCTION = 104, KW_GENERATE = 105, KW_GENVAR = 106, KW_GLOBAL = 107, 
    KW_HIGHZ0 = 108, KW_HIGHZ1 = 109, KW_IF = 110, KW_IFF = 111, KW_IFNONE = 112, 
    KW_IGNORE_BINS = 113, KW_ILLEGAL_BINS = 114, KW_IMPLEMENTS = 115, KW_IMPLIES = 116, 
    KW_IMPORT = 117, KW_INCDIR = 118, KW_INCLUDE = 119, KW_INITIAL = 120, 
    KW_INOUT = 121, KW_INPUT = 122, KW_INSIDE = 123, KW_INSTANCE = 124, 
    KW_INT = 125, KW_INTEGER = 126, KW_INTERCONNECT = 127, KW_INTERFACE = 128, 
    KW_INTERSECT = 129, KW_JOIN = 130, KW_JOIN_ANY = 131, KW_JOIN_NONE = 132, 
    KW_LARGE = 133, KW_LET = 134, KW_LIBRARY = 135, KW_LIBLIST = 136, KW_LOCAL = 137, 
    KW_LOCALPARAM = 138, KW_LOGIC = 139, KW_LONGINT = 140, KW_MACROMODULE = 141, 
    KW_MATCHES = 142, KW_MEDIUM = 143, KW_MODPORT = 144, KW_MODULE = 145, 
    KW_NAND = 146, KW_NEGEDGE = 147, KW_NETTYPE = 148, KW_NEW = 149, KW_NEXTTIME = 150, 
    KW_NMOS = 151, KW_NOR = 152, KW_NOSHOWCANCELLED = 153, KW_NOT = 154, 
    KW_NOTIF0 = 155, KW_NOTIF1 = 156, KW_NULL = 157, KW_OPTION = 158, KW_OR = 159, 
    KW_OUTPUT = 160, KW_PACKAGE = 161, KW_PACKED = 162, KW_PARAMETER = 163, 
    KW_PMOS = 164, KW_POSEDGE = 165, KW_PRIMITIVE = 166, KW_PRIORITY = 167, 
    KW_PROGRAM = 168, KW_PROPERTY = 169, KW_PROTECTED = 170, KW_PULL0 = 171, 
    KW_PULL1 = 172, KW_PULLDOWN = 173, KW_PULLUP = 174, KW_PULSESTYLE_ONDETECT = 175, 
    KW_PULSESTYLE_ONEVENT = 176, KW_PURE = 177, KW_RAND = 178, KW_RANDC = 179, 
    KW_RANDCASE = 180, KW_RANDOMIZE = 181, KW_RANDSEQUENCE = 182, KW_RCMOS = 183, 
    KW_REAL = 184, KW_REALTIME = 185, KW_REF = 186, KW_REG = 187, KW_REJECT_ON = 188, 
    KW_RELEASE = 189, KW_REPEAT = 190, KW_RESTRICT = 191, KW_RETURN = 192, 
    KW_RNMOS = 193, KW_RPMOS = 194, KW_RTRAN = 195, KW_RTRANIF0 = 196, KW_RTRANIF1 = 197, 
    KW_S_ALWAYS = 198, KW_S_EVENTUALLY = 199, KW_S_NEXTTIME = 200, KW_S_UNTIL = 201, 
    KW_S_UNTIL_WITH = 202, KW_SAMPLE = 203, KW_SCALARED = 204, KW_SEQUENCE = 205, 
    KW_SHORTINT = 206, KW_SHORTREAL = 207, KW_SHOWCANCELLED = 208, KW_SIGNED = 209, 
    KW_SMALL = 210, KW_SOFT = 211, KW_SOLVE = 212, KW_SPECIFY = 213, KW_SPECPARAM = 214, 
    KW_STATIC = 215, KW_STD = 216, KW_STRING = 217, KW_STRONG = 218, KW_STRONG0 = 219, 
    KW_STRONG1 = 220, KW_STRUCT = 221, KW_SUPER = 222, KW_SUPPLY0 = 223, 
    KW_SUPPLY1 = 224, KW_SYNC_ACCEPT_ON = 225, KW_SYNC_REJECT_ON = 226, 
    KW_TABLE = 227, KW_TAGGED = 228, KW_TASK = 229, KW_THIS = 230, KW_THROUGHOUT = 231, 
    KW_TIME = 232, KW_TIMEPRECISION = 233, KW_TIMEUNIT = 234, KW_TRAN = 235, 
    KW_TRANIF0 = 236, KW_TRANIF1 = 237, KW_TRI = 238, KW_TRI0 = 239, KW_TRI1 = 240, 
    KW_TRIAND = 241, KW_TRIOR = 242, KW_TRIREG = 243, KW_TYPE = 244, KW_TYPE_OPTION = 245, 
    KW_TYPEDEF = 246, KW_UNION = 247, KW_UNIQUE = 248, KW_UNIQUE0 = 249, 
    KW_UNSIGNED = 250, KW_UNTIL = 251, KW_UNTIL_WITH = 252, KW_UNTYPED = 253, 
    KW_USE = 254, KW_UWIRE = 255, KW_VAR = 256, KW_VECTORED = 257, KW_VIRTUAL = 258, 
    KW_VOID = 259, KW_WAIT = 260, KW_WAIT_ORDER = 261, KW_WAND = 262, KW_WEAK = 263, 
    KW_WEAK0 = 264, KW_WEAK1 = 265, KW_WHILE = 266, KW_WILDCARD = 267, KW_WIRE = 268, 
    KW_WITH = 269, KW_WITHIN = 270, KW_WOR = 271, KW_XNOR = 272, KW_XOR = 273, 
    EDGE_CONTROL_SPECIFIER = 274, TIME_LITERAL = 275, ANY_BASED_NUMBER = 276, 
    BASED_NUMBER_WITH_SIZE = 277, REAL_NUMBER_WITH_EXP = 278, FIXED_POINT_NUMBER = 279, 
    UNSIGNED_NUMBER = 280, UNBASED_UNSIZED_LITERAL = 281, STRING_LITERAL = 282, 
    C_IDENTIFIER = 283, ESCAPED_IDENTIFIER = 284, SIMPLE_IDENTIFIER = 285, 
    SYSTEM_TF_IDENTIFIER = 286, SEMI = 287, LPAREN = 288, RPAREN = 289, 
    LSQUARE_BR = 290, RSQUARE_BR = 291, LBRACE = 292, RBRACE = 293, APOSTROPHE = 294, 
    APOSTROPHE_LBRACE = 295, SHIFT_LEFT = 296, SHIFT_RIGHT = 297, ARITH_SHIFT_LEFT = 298, 
    ARITH_SHIFT_RIGHT = 299, DOLAR = 300, MOD = 301, NOT = 302, NEG = 303, 
    NAND = 304, NOR = 305, XOR = 306, NXOR = 307, XORN = 308, COMMA = 309, 
    DOT = 310, QUESTIONMARK = 311, COLON = 312, DOUBLE_COLON = 313, EQ = 314, 
    NE = 315, CASE_EQ = 316, CASE_NE = 317, WILDCARD_EQ = 318, WILDCARD_NE = 319, 
    ASSIGN = 320, LT = 321, GT = 322, GE = 323, LE = 324, PLUS_ASSIGN = 325, 
    MINUS_ASSIGN = 326, MUL_ASSIGN = 327, DIV_ASSIGN = 328, MOD_ASSIGN = 329, 
    AND_ASSIGN = 330, OR_ASSIGN = 331, XOR_ASSIGN = 332, SHIFT_LEFT_ASSIGN = 333, 
    SHIFT_RIGHT_ASSIGN = 334, ARITH_SHIFT_LEFT_ASSIGN = 335, ARITH_SHIFT_RIGHT_ASSIGN = 336, 
    PLUS = 337, MINUS = 338, AMPERSAND = 339, AND_LOG = 340, BAR = 341, 
    OR_LOG = 342, BACKSLASH = 343, MUL = 344, DIV = 345, DOUBLESTAR = 346, 
    BI_DIR_ARROW = 347, ARROW = 348, DOUBLE_RIGHT_ARROW = 349, INCR = 350, 
    DECR = 351, DIST_WEIGHT_ASSIGN = 352, OVERLAPPING_IMPL = 353, NONOVERLAPPING_IMPL = 354, 
    IMPLIES = 355, IMPLIES_P = 356, IMPLIES_N = 357, PATH_FULL = 358, HASH_MINUS_HASH = 359, 
    HASH_EQ_HASH = 360, AT = 361, DOUBLE_AT = 362, HASH = 363, DOUBLE_HASH = 364, 
    TRIPLE_AND = 365, ONE_LINE_COMMENT = 366, DOCUMENTATION_COMMENT = 367, 
    BLOCK_COMMENT = 368, WHITE_SPACE = 369, LEVEL_SYMBOL = 370, EDGE_SYMBOL = 371
  };

  enum {
    RuleSource_text = 0, RuleDescription = 1, RuleAssignment_operator = 2, 
    RuleEdge_identifier = 3, RuleIdentifier = 4, RuleInteger_type = 5, RuleInteger_atom_type = 6, 
    RuleInteger_vector_type = 7, RuleNon_integer_type = 8, RuleNet_type = 9, 
    RuleUnary_module_path_operator = 10, RuleUnary_operator = 11, RuleInc_or_dec_operator = 12, 
    RuleImplicit_class_handle = 13, RuleIntegral_number = 14, RuleReal_number = 15, 
    RuleAny_system_tf_identifier = 16, RuleSigning = 17, RuleNumber = 18, 
    RuleTimeunits_declaration = 19, RuleLifetime = 20, RulePort_direction = 21, 
    RuleAlways_keyword = 22, RuleJoin_keyword = 23, RuleUnique_priority = 24, 
    RuleDrive_strength = 25, RuleStrength0 = 26, RuleStrength1 = 27, RuleCharge_strength = 28, 
    RuleSequence_lvar_port_direction = 29, RuleBins_keyword = 30, RuleClass_item_qualifier = 31, 
    RuleRandom_qualifier = 32, RuleProperty_qualifier = 33, RuleMethod_qualifier = 34, 
    RuleConstraint_prototype_qualifier = 35, RuleCmos_switchtype = 36, RuleEnable_gatetype = 37, 
    RuleMos_switchtype = 38, RuleN_input_gatetype = 39, RuleN_output_gatetype = 40, 
    RulePass_en_switchtype = 41, RulePass_switchtype = 42, RuleAny_implication = 43, 
    RuleTiming_check_event_control = 44, RuleImport_export = 45, RuleArray_method_name = 46, 
    RuleOperator_mul_div_mod = 47, RuleOperator_plus_minus = 48, RuleOperator_shift = 49, 
    RuleOperator_cmp = 50, RuleOperator_eq_neq = 51, RuleOperator_xor = 52, 
    RuleOperator_impl = 53, RuleUdp_nonansi_declaration = 54, RuleUdp_ansi_declaration = 55, 
    RuleUdp_declaration = 56, RuleUdp_declaration_port_list = 57, RuleUdp_port_declaration = 58, 
    RuleUdp_output_declaration = 59, RuleUdp_input_declaration = 60, RuleUdp_reg_declaration = 61, 
    RuleUdp_body = 62, RuleCombinational_body = 63, RuleCombinational_entry = 64, 
    RuleSequential_body = 65, RuleUdp_initial_statement = 66, RuleSequential_entry = 67, 
    RuleSeq_input_list = 68, RuleLevel_input_list = 69, RuleEdge_input_list = 70, 
    RuleEdge_indicator = 71, RuleCurrent_state = 72, RuleNext_state = 73, 
    RuleInterface_declaration = 74, RuleInterface_header = 75, RuleInterface_item = 76, 
    RuleModport_declaration = 77, RuleModport_item = 78, RuleModport_ports_declaration = 79, 
    RuleModport_clocking_declaration = 80, RuleModport_simple_ports_declaration = 81, 
    RuleModport_simple_port = 82, RuleModport_tf_ports_declaration = 83, 
    RuleModport_tf_port = 84, RuleStatement_or_null = 85, RuleInitial_construct = 86, 
    RuleDefault_clocking_or_dissable_construct = 87, RuleStatement = 88, 
    RuleStatement_item = 89, RuleCycle_delay = 90, RuleClocking_drive = 91, 
    RuleClockvar_expression = 92, RuleFinal_construct = 93, RuleBlocking_assignment = 94, 
    RuleProcedural_timing_control_statement = 95, RuleProcedural_timing_control = 96, 
    RuleEvent_control = 97, RuleDelay_or_event_control = 98, RuleDelay3 = 99, 
    RuleDelay2 = 100, RuleDelay_value = 101, RuleDelay_control = 102, RuleNonblocking_assignment = 103, 
    RuleProcedural_continuous_assignment = 104, RuleVariable_assignment = 105, 
    RuleAction_block = 106, RuleSeq_block = 107, RulePar_block = 108, RuleCase_statement = 109, 
    RuleCase_keyword = 110, RuleCase_item = 111, RuleCase_pattern_item = 112, 
    RuleCase_inside_item = 113, RuleRandcase_statement = 114, RuleRandcase_item = 115, 
    RuleCond_predicate = 116, RuleConditional_statement = 117, RuleSubroutine_call_statement = 118, 
    RuleDisable_statement = 119, RuleEvent_trigger = 120, RuleLoop_statement = 121, 
    RuleList_of_variable_assignments = 122, RuleFor_initialization = 123, 
    RuleFor_end_expression = 124, RuleFor_variable_declaration_var_assign = 125, 
    RuleFor_variable_declaration = 126, RuleFor_step = 127, RuleLoop_variables = 128, 
    RuleJump_statement = 129, RuleWait_statement = 130, RuleName_of_instance = 131, 
    RuleChecker_instantiation = 132, RuleList_of_checker_port_connections = 133, 
    RuleOrdered_checker_port_connection = 134, RuleNamed_checker_port_connection = 135, 
    RuleProcedural_assertion_statement = 136, RuleConcurrent_assertion_statement = 137, 
    RuleAssertion_item = 138, RuleConcurrent_assertion_item = 139, RuleImmediate_assertion_statement = 140, 
    RuleSimple_immediate_assertion_statement = 141, RuleSimple_immediate_assert_statement = 142, 
    RuleSimple_immediate_assume_statement = 143, RuleSimple_immediate_cover_statement = 144, 
    RuleDeferred_immediate_assertion_statement = 145, RulePrimitive_delay = 146, 
    RuleDeferred_immediate_assert_statement = 147, RuleDeferred_immediate_assume_statement = 148, 
    RuleDeferred_immediate_cover_statement = 149, RuleWeight_specification = 150, 
    RuleProduction_item = 151, RuleRs_code_block = 152, RuleRandsequence_statement = 153, 
    RuleRs_prod = 154, RuleRs_if_else = 155, RuleRs_repeat = 156, RuleRs_case = 157, 
    RuleRs_case_item = 158, RuleRs_rule = 159, RuleRs_production_list = 160, 
    RuleProduction = 161, RuleTf_item_declaration = 162, RuleTf_port_list = 163, 
    RuleTf_port_item = 164, RuleTf_port_direction = 165, RuleTf_port_declaration = 166, 
    RuleList_of_tf_variable_identifiers_item = 167, RuleList_of_tf_variable_identifiers = 168, 
    RuleExpect_property_statement = 169, RuleBlock_item_declaration = 170, 
    RuleParam_assignment = 171, RuleType_assignment = 172, RuleList_of_type_assignments = 173, 
    RuleList_of_param_assignments = 174, RuleLocal_parameter_declaration = 175, 
    RuleParameter_declaration = 176, RuleType_declaration = 177, RuleNet_type_declaration = 178, 
    RuleLet_declaration = 179, RuleLet_port_list = 180, RuleLet_port_item = 181, 
    RuleLet_formal_type = 182, RulePackage_import_declaration = 183, RulePackage_import_item = 184, 
    RuleProperty_list_of_arguments = 185, RuleProperty_actual_arg = 186, 
    RuleProperty_formal_type = 187, RuleSequence_formal_type = 188, RuleProperty_instance = 189, 
    RuleProperty_spec = 190, RuleProperty_expr = 191, RuleProperty_case_item = 192, 
    RuleBit_select = 193, RuleIdentifier_with_bit_select = 194, RulePackage_or_class_scoped_hier_id_with_select = 195, 
    RulePackage_or_class_scoped_path_item = 196, RulePackage_or_class_scoped_path = 197, 
    RuleHierarchical_identifier = 198, RulePackage_or_class_scoped_id = 199, 
    RuleSelect = 200, RuleEvent_expression_item = 201, RuleEvent_expression = 202, 
    RuleBoolean_abbrev = 203, RuleSequence_abbrev = 204, RuleConsecutive_repetition = 205, 
    RuleNon_consecutive_repetition = 206, RuleGoto_repetition = 207, RuleCycle_delay_const_range_expression = 208, 
    RuleSequence_instance = 209, RuleSequence_expr = 210, RuleSequence_match_item = 211, 
    RuleOperator_assignment = 212, RuleSequence_actual_arg = 213, RuleDist_weight = 214, 
    RuleClocking_declaration = 215, RuleClocking_item = 216, RuleList_of_clocking_decl_assign = 217, 
    RuleClocking_decl_assign = 218, RuleDefault_skew = 219, RuleClocking_direction = 220, 
    RuleClocking_skew = 221, RuleClocking_event = 222, RuleCycle_delay_range = 223, 
    RuleExpression_or_dist = 224, RuleCovergroup_declaration = 225, RuleCover_cross = 226, 
    RuleIdentifier_list_2plus = 227, RuleCross_body = 228, RuleCross_body_item = 229, 
    RuleBins_selection_or_option = 230, RuleBins_selection = 231, RuleSelect_expression = 232, 
    RuleSelect_condition = 233, RuleBins_expression = 234, RuleCovergroup_range_list = 235, 
    RuleCovergroup_value_range = 236, RuleCovergroup_expression = 237, RuleCoverage_spec_or_option = 238, 
    RuleCoverage_option = 239, RuleCoverage_spec = 240, RuleCover_point = 241, 
    RuleBins_or_empty = 242, RuleBins_or_options = 243, RuleTrans_list = 244, 
    RuleTrans_set = 245, RuleTrans_range_list = 246, RuleRepeat_range = 247, 
    RuleCoverage_event = 248, RuleBlock_event_expression = 249, RuleHierarchical_btf_identifier = 250, 
    RuleAssertion_variable_declaration = 251, RuleDist_item = 252, RuleValue_range = 253, 
    RuleAttribute_instance = 254, RuleAttr_spec = 255, RuleClass_new = 256, 
    RuleParam_expression = 257, RuleConstant_param_expression = 258, RuleUnpacked_dimension = 259, 
    RulePacked_dimension = 260, RuleVariable_dimension = 261, RuleStruct_union = 262, 
    RuleEnum_base_type = 263, RuleData_type_primitive = 264, RuleData_type = 265, 
    RuleData_type_or_implicit = 266, RuleImplicit_data_type = 267, RuleSequence_list_of_arguments_named_item = 268, 
    RuleSequence_list_of_arguments = 269, RuleList_of_arguments_named_item = 270, 
    RuleList_of_arguments = 271, RulePrimary_literal = 272, RuleType_reference = 273, 
    RulePackage_scope = 274, RulePs_identifier = 275, RuleList_of_parameter_value_assignments = 276, 
    RuleParameter_value_assignment = 277, RuleClass_type = 278, RuleClass_scope = 279, 
    RuleFirst_range_identifier = 280, RuleSecond_range_identifier = 281, 
    RuleRange_expression = 282, RuleConstant_range_expression = 283, RuleConstant_mintypmax_expression = 284, 
    RuleMintypmax_expression = 285, RuleNamed_parameter_assignment = 286, 
    RulePrimary = 287, RuleCast_separator = 288, RuleConstant_expression = 289, 
    RuleInc_or_dec_expression = 290, RuleExpression = 291, RuleOperator_log_and = 292, 
    RuleOperator_log_or = 293, RuleOperator_power = 294, RuleOperator_bitwise_and = 295, 
    RuleOperator_bitwise_or = 296, RuleReplication = 297, RuleReplication_size = 298, 
    RuleReplication_value = 299, RuleConcatenation = 300, RuleDynamic_array_new = 301, 
    RuleConst_or_range_expression = 302, RuleVariable_decl_assignment = 303, 
    RuleAssignment_pattern_variable_lvalue = 304, RuleStream_operator = 305, 
    RuleSlice_size = 306, RuleStreaming_concatenation = 307, RuleStream_concatenation = 308, 
    RuleStream_expression = 309, RuleArray_range_expression = 310, RuleRange_separator = 311, 
    RuleOpen_range_list = 312, RulePattern = 313, RuleAssignment_pattern = 314, 
    RuleReplication_assignment = 315, RuleStructure_pattern_key = 316, RuleArray_pattern_key = 317, 
    RuleAssignment_pattern_key = 318, RuleStruct_union_member = 319, RuleData_type_or_void = 320, 
    RuleEnum_name_declaration = 321, RuleAssignment_pattern_expression = 322, 
    RuleAssignment_pattern_expression_type = 323, RuleNet_lvalue = 324, 
    RuleVariable_lvalue = 325, RuleSolve_before_list = 326, RuleConstraint_block_item = 327, 
    RuleConstraint_expression = 328, RuleUniqueness_constraint = 329, RuleConstraint_set = 330, 
    RuleRandomize_call = 331, RuleModule_header_common = 332, RuleModule_declaration = 333, 
    RuleModule_keyword = 334, RuleNet_port_type = 335, RuleVar_data_type = 336, 
    RuleNet_or_var_data_type = 337, RuleList_of_defparam_assignments = 338, 
    RuleList_of_net_decl_assignments = 339, RuleList_of_specparam_assignments = 340, 
    RuleList_of_variable_decl_assignments = 341, RuleList_of_variable_identifiers_item = 342, 
    RuleList_of_variable_identifiers = 343, RuleList_of_variable_port_identifiers = 344, 
    RuleDefparam_assignment = 345, RuleNet_decl_assignment = 346, RuleSpecparam_assignment = 347, 
    RuleError_limit_value = 348, RuleReject_limit_value = 349, RulePulse_control_specparam = 350, 
    RuleIdentifier_doted_index_at_end = 351, RuleSpecify_terminal_descriptor = 352, 
    RuleSpecify_input_terminal_descriptor = 353, RuleSpecify_output_terminal_descriptor = 354, 
    RuleSpecify_item = 355, RulePulsestyle_declaration = 356, RuleShowcancelled_declaration = 357, 
    RulePath_declaration = 358, RuleSimple_path_declaration = 359, RulePath_delay_value = 360, 
    RuleList_of_path_outputs = 361, RuleList_of_path_inputs = 362, RuleList_of_paths = 363, 
    RuleList_of_path_delay_expressions = 364, RuleT_path_delay_expression = 365, 
    RuleTrise_path_delay_expression = 366, RuleTfall_path_delay_expression = 367, 
    RuleTz_path_delay_expression = 368, RuleT01_path_delay_expression = 369, 
    RuleT10_path_delay_expression = 370, RuleT0z_path_delay_expression = 371, 
    RuleTz1_path_delay_expression = 372, RuleT1z_path_delay_expression = 373, 
    RuleTz0_path_delay_expression = 374, RuleT0x_path_delay_expression = 375, 
    RuleTx1_path_delay_expression = 376, RuleT1x_path_delay_expression = 377, 
    RuleTx0_path_delay_expression = 378, RuleTxz_path_delay_expression = 379, 
    RuleTzx_path_delay_expression = 380, RuleParallel_path_description = 381, 
    RuleFull_path_description = 382, RuleIdentifier_list = 383, RuleSpecparam_declaration = 384, 
    RuleEdge_sensitive_path_declaration = 385, RuleParallel_edge_sensitive_path_description = 386, 
    RuleFull_edge_sensitive_path_description = 387, RuleData_source_expression = 388, 
    RuleData_declaration = 389, RuleModule_path_expression = 390, RuleState_dependent_path_declaration = 391, 
    RulePackage_export_declaration = 392, RuleGenvar_declaration = 393, 
    RuleNet_declaration = 394, RuleParameter_port_list = 395, RuleParameter_port_declaration = 396, 
    RuleList_of_port_declarations_ansi_item = 397, RuleList_of_port_declarations = 398, 
    RuleNonansi_port_declaration = 399, RuleNonansi_port = 400, RuleNonansi_port__expr = 401, 
    RulePort_identifier = 402, RuleAnsi_port_declaration = 403, RuleSystem_timing_check = 404, 
    RuleDolar_setup_timing_check = 405, RuleDolar_hold_timing_check = 406, 
    RuleDolar_setuphold_timing_check = 407, RuleDolar_recovery_timing_check = 408, 
    RuleDolar_removal_timing_check = 409, RuleDolar_recrem_timing_check = 410, 
    RuleDolar_skew_timing_check = 411, RuleDolar_timeskew_timing_check = 412, 
    RuleDolar_fullskew_timing_check = 413, RuleDolar_period_timing_check = 414, 
    RuleDolar_width_timing_check = 415, RuleDolar_nochange_timing_check = 416, 
    RuleTimecheck_condition = 417, RuleControlled_reference_event = 418, 
    RuleDelayed_reference = 419, RuleEnd_edge_offset = 420, RuleEvent_based_flag = 421, 
    RuleNotifier = 422, RuleRemain_active_flag = 423, RuleTimestamp_condition = 424, 
    RuleStart_edge_offset = 425, RuleThreshold = 426, RuleTiming_check_limit = 427, 
    RuleTiming_check_event = 428, RuleTiming_check_condition = 429, RuleScalar_timing_check_condition = 430, 
    RuleControlled_timing_check_event = 431, RuleFunction_data_type_or_implicit = 432, 
    RuleExtern_tf_declaration = 433, RuleFunction_declaration = 434, RuleUntyped_function_declaration = 435, 
    RuleTask_prototype = 436, RuleFunction_prototype = 437, RuleDpi_import_export = 438, 
    RuleDpi_function_import_property = 439, RuleDpi_task_import_property = 440, 
    RuleTask_and_function_declaration_common = 441, RuleTask_declaration = 442, 
    RuleMethod_prototype = 443, RuleExtern_constraint_declaration = 444, 
    RuleConstraint_block = 445, RuleChecker_port_list = 446, RuleChecker_port_item = 447, 
    RuleChecker_port_direction = 448, RuleChecker_declaration = 449, RuleClass_declaration = 450, 
    RuleAlways_construct = 451, RuleInterface_class_type = 452, RuleInterface_class_declaration = 453, 
    RuleInterface_class_item = 454, RuleInterface_class_method = 455, RulePackage_declaration = 456, 
    RulePackage_item = 457, RuleProgram_declaration = 458, RuleProgram_header = 459, 
    RuleProgram_item = 460, RuleNon_port_program_item = 461, RuleAnonymous_program = 462, 
    RuleAnonymous_program_item = 463, RuleSequence_declaration = 464, RuleSequence_port_list = 465, 
    RuleSequence_port_item = 466, RuleProperty_declaration = 467, RuleProperty_port_list = 468, 
    RuleProperty_port_item = 469, RuleContinuous_assign = 470, RuleChecker_or_generate_item = 471, 
    RuleConstraint_prototype = 472, RuleClass_constraint = 473, RuleConstraint_declaration = 474, 
    RuleClass_constructor_declaration = 475, RuleClass_property = 476, RuleClass_method = 477, 
    RuleClass_constructor_prototype = 478, RuleClass_item = 479, RuleParameter_override = 480, 
    RuleGate_instantiation = 481, RuleEnable_gate_or_mos_switch_or_cmos_switch_instance = 482, 
    RuleN_input_gate_instance = 483, RuleN_output_gate_instance = 484, RulePass_switch_instance = 485, 
    RulePass_enable_switch_instance = 486, RulePull_gate_instance = 487, 
    RulePulldown_strength = 488, RulePullup_strength = 489, RuleEnable_terminal = 490, 
    RuleInout_terminal = 491, RuleInput_terminal = 492, RuleOutput_terminal = 493, 
    RuleUdp_instantiation = 494, RuleUdp_instance = 495, RuleUdp_instance_body = 496, 
    RuleModule_or_interface_or_program_or_udp_instantiation = 497, RuleHierarchical_instance = 498, 
    RuleList_of_port_connections = 499, RuleOrdered_port_connection = 500, 
    RuleNamed_port_connection = 501, RulePort_expression_connection = 502, 
    RulePort_concatenation_connection = 503, RuleConcatenation_item = 504, 
    RulePort_replication_connection = 505, RuleBind_directive = 506, RuleBind_target_instance = 507, 
    RuleBind_target_instance_list = 508, RuleBind_instantiation = 509, RuleConfig_declaration = 510, 
    RuleDesign_statement = 511, RuleConfig_rule_statement = 512, RuleInst_clause = 513, 
    RuleInst_name = 514, RuleCell_clause = 515, RuleLiblist_clause = 516, 
    RuleUse_clause = 517, RuleNet_alias = 518, RuleSpecify_block = 519, 
    RuleGenerate_region = 520, RuleGenvar_expression = 521, RuleLoop_generate_construct = 522, 
    RuleGenvar_initialization = 523, RuleGenvar_iteration = 524, RuleConditional_generate_construct = 525, 
    RuleIf_generate_construct = 526, RuleCase_generate_construct = 527, 
    RuleCase_generate_item = 528, RuleGenerate_begin_end_block = 529, RuleGenerate_item = 530, 
    RuleProgram_generate_item = 531, RuleModule_or_generate_or_interface_or_checker_item = 532, 
    RuleModule_or_generate_or_interface_item = 533, RuleModule_or_generate_item = 534, 
    RuleElaboration_system_task = 535, RuleModule_item_item = 536, RuleModule_item = 537
  };

  explicit sv2017(antlr4::TokenStream *input);

  sv2017(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~sv2017() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class Source_textContext;
  class DescriptionContext;
  class Assignment_operatorContext;
  class Edge_identifierContext;
  class IdentifierContext;
  class Integer_typeContext;
  class Integer_atom_typeContext;
  class Integer_vector_typeContext;
  class Non_integer_typeContext;
  class Net_typeContext;
  class Unary_module_path_operatorContext;
  class Unary_operatorContext;
  class Inc_or_dec_operatorContext;
  class Implicit_class_handleContext;
  class Integral_numberContext;
  class Real_numberContext;
  class Any_system_tf_identifierContext;
  class SigningContext;
  class NumberContext;
  class Timeunits_declarationContext;
  class LifetimeContext;
  class Port_directionContext;
  class Always_keywordContext;
  class Join_keywordContext;
  class Unique_priorityContext;
  class Drive_strengthContext;
  class Strength0Context;
  class Strength1Context;
  class Charge_strengthContext;
  class Sequence_lvar_port_directionContext;
  class Bins_keywordContext;
  class Class_item_qualifierContext;
  class Random_qualifierContext;
  class Property_qualifierContext;
  class Method_qualifierContext;
  class Constraint_prototype_qualifierContext;
  class Cmos_switchtypeContext;
  class Enable_gatetypeContext;
  class Mos_switchtypeContext;
  class N_input_gatetypeContext;
  class N_output_gatetypeContext;
  class Pass_en_switchtypeContext;
  class Pass_switchtypeContext;
  class Any_implicationContext;
  class Timing_check_event_controlContext;
  class Import_exportContext;
  class Array_method_nameContext;
  class Operator_mul_div_modContext;
  class Operator_plus_minusContext;
  class Operator_shiftContext;
  class Operator_cmpContext;
  class Operator_eq_neqContext;
  class Operator_xorContext;
  class Operator_implContext;
  class Udp_nonansi_declarationContext;
  class Udp_ansi_declarationContext;
  class Udp_declarationContext;
  class Udp_declaration_port_listContext;
  class Udp_port_declarationContext;
  class Udp_output_declarationContext;
  class Udp_input_declarationContext;
  class Udp_reg_declarationContext;
  class Udp_bodyContext;
  class Combinational_bodyContext;
  class Combinational_entryContext;
  class Sequential_bodyContext;
  class Udp_initial_statementContext;
  class Sequential_entryContext;
  class Seq_input_listContext;
  class Level_input_listContext;
  class Edge_input_listContext;
  class Edge_indicatorContext;
  class Current_stateContext;
  class Next_stateContext;
  class Interface_declarationContext;
  class Interface_headerContext;
  class Interface_itemContext;
  class Modport_declarationContext;
  class Modport_itemContext;
  class Modport_ports_declarationContext;
  class Modport_clocking_declarationContext;
  class Modport_simple_ports_declarationContext;
  class Modport_simple_portContext;
  class Modport_tf_ports_declarationContext;
  class Modport_tf_portContext;
  class Statement_or_nullContext;
  class Initial_constructContext;
  class Default_clocking_or_dissable_constructContext;
  class StatementContext;
  class Statement_itemContext;
  class Cycle_delayContext;
  class Clocking_driveContext;
  class Clockvar_expressionContext;
  class Final_constructContext;
  class Blocking_assignmentContext;
  class Procedural_timing_control_statementContext;
  class Procedural_timing_controlContext;
  class Event_controlContext;
  class Delay_or_event_controlContext;
  class Delay3Context;
  class Delay2Context;
  class Delay_valueContext;
  class Delay_controlContext;
  class Nonblocking_assignmentContext;
  class Procedural_continuous_assignmentContext;
  class Variable_assignmentContext;
  class Action_blockContext;
  class Seq_blockContext;
  class Par_blockContext;
  class Case_statementContext;
  class Case_keywordContext;
  class Case_itemContext;
  class Case_pattern_itemContext;
  class Case_inside_itemContext;
  class Randcase_statementContext;
  class Randcase_itemContext;
  class Cond_predicateContext;
  class Conditional_statementContext;
  class Subroutine_call_statementContext;
  class Disable_statementContext;
  class Event_triggerContext;
  class Loop_statementContext;
  class List_of_variable_assignmentsContext;
  class For_initializationContext;
  class For_end_expressionContext;
  class For_variable_declaration_var_assignContext;
  class For_variable_declarationContext;
  class For_stepContext;
  class Loop_variablesContext;
  class Jump_statementContext;
  class Wait_statementContext;
  class Name_of_instanceContext;
  class Checker_instantiationContext;
  class List_of_checker_port_connectionsContext;
  class Ordered_checker_port_connectionContext;
  class Named_checker_port_connectionContext;
  class Procedural_assertion_statementContext;
  class Concurrent_assertion_statementContext;
  class Assertion_itemContext;
  class Concurrent_assertion_itemContext;
  class Immediate_assertion_statementContext;
  class Simple_immediate_assertion_statementContext;
  class Simple_immediate_assert_statementContext;
  class Simple_immediate_assume_statementContext;
  class Simple_immediate_cover_statementContext;
  class Deferred_immediate_assertion_statementContext;
  class Primitive_delayContext;
  class Deferred_immediate_assert_statementContext;
  class Deferred_immediate_assume_statementContext;
  class Deferred_immediate_cover_statementContext;
  class Weight_specificationContext;
  class Production_itemContext;
  class Rs_code_blockContext;
  class Randsequence_statementContext;
  class Rs_prodContext;
  class Rs_if_elseContext;
  class Rs_repeatContext;
  class Rs_caseContext;
  class Rs_case_itemContext;
  class Rs_ruleContext;
  class Rs_production_listContext;
  class ProductionContext;
  class Tf_item_declarationContext;
  class Tf_port_listContext;
  class Tf_port_itemContext;
  class Tf_port_directionContext;
  class Tf_port_declarationContext;
  class List_of_tf_variable_identifiers_itemContext;
  class List_of_tf_variable_identifiersContext;
  class Expect_property_statementContext;
  class Block_item_declarationContext;
  class Param_assignmentContext;
  class Type_assignmentContext;
  class List_of_type_assignmentsContext;
  class List_of_param_assignmentsContext;
  class Local_parameter_declarationContext;
  class Parameter_declarationContext;
  class Type_declarationContext;
  class Net_type_declarationContext;
  class Let_declarationContext;
  class Let_port_listContext;
  class Let_port_itemContext;
  class Let_formal_typeContext;
  class Package_import_declarationContext;
  class Package_import_itemContext;
  class Property_list_of_argumentsContext;
  class Property_actual_argContext;
  class Property_formal_typeContext;
  class Sequence_formal_typeContext;
  class Property_instanceContext;
  class Property_specContext;
  class Property_exprContext;
  class Property_case_itemContext;
  class Bit_selectContext;
  class Identifier_with_bit_selectContext;
  class Package_or_class_scoped_hier_id_with_selectContext;
  class Package_or_class_scoped_path_itemContext;
  class Package_or_class_scoped_pathContext;
  class Hierarchical_identifierContext;
  class Package_or_class_scoped_idContext;
  class SelectContext;
  class Event_expression_itemContext;
  class Event_expressionContext;
  class Boolean_abbrevContext;
  class Sequence_abbrevContext;
  class Consecutive_repetitionContext;
  class Non_consecutive_repetitionContext;
  class Goto_repetitionContext;
  class Cycle_delay_const_range_expressionContext;
  class Sequence_instanceContext;
  class Sequence_exprContext;
  class Sequence_match_itemContext;
  class Operator_assignmentContext;
  class Sequence_actual_argContext;
  class Dist_weightContext;
  class Clocking_declarationContext;
  class Clocking_itemContext;
  class List_of_clocking_decl_assignContext;
  class Clocking_decl_assignContext;
  class Default_skewContext;
  class Clocking_directionContext;
  class Clocking_skewContext;
  class Clocking_eventContext;
  class Cycle_delay_rangeContext;
  class Expression_or_distContext;
  class Covergroup_declarationContext;
  class Cover_crossContext;
  class Identifier_list_2plusContext;
  class Cross_bodyContext;
  class Cross_body_itemContext;
  class Bins_selection_or_optionContext;
  class Bins_selectionContext;
  class Select_expressionContext;
  class Select_conditionContext;
  class Bins_expressionContext;
  class Covergroup_range_listContext;
  class Covergroup_value_rangeContext;
  class Covergroup_expressionContext;
  class Coverage_spec_or_optionContext;
  class Coverage_optionContext;
  class Coverage_specContext;
  class Cover_pointContext;
  class Bins_or_emptyContext;
  class Bins_or_optionsContext;
  class Trans_listContext;
  class Trans_setContext;
  class Trans_range_listContext;
  class Repeat_rangeContext;
  class Coverage_eventContext;
  class Block_event_expressionContext;
  class Hierarchical_btf_identifierContext;
  class Assertion_variable_declarationContext;
  class Dist_itemContext;
  class Value_rangeContext;
  class Attribute_instanceContext;
  class Attr_specContext;
  class Class_newContext;
  class Param_expressionContext;
  class Constant_param_expressionContext;
  class Unpacked_dimensionContext;
  class Packed_dimensionContext;
  class Variable_dimensionContext;
  class Struct_unionContext;
  class Enum_base_typeContext;
  class Data_type_primitiveContext;
  class Data_typeContext;
  class Data_type_or_implicitContext;
  class Implicit_data_typeContext;
  class Sequence_list_of_arguments_named_itemContext;
  class Sequence_list_of_argumentsContext;
  class List_of_arguments_named_itemContext;
  class List_of_argumentsContext;
  class Primary_literalContext;
  class Type_referenceContext;
  class Package_scopeContext;
  class Ps_identifierContext;
  class List_of_parameter_value_assignmentsContext;
  class Parameter_value_assignmentContext;
  class Class_typeContext;
  class Class_scopeContext;
  class First_range_identifierContext;
  class Second_range_identifierContext;
  class Range_expressionContext;
  class Constant_range_expressionContext;
  class Constant_mintypmax_expressionContext;
  class Mintypmax_expressionContext;
  class Named_parameter_assignmentContext;
  class PrimaryContext;
  class Cast_separatorContext;
  class Constant_expressionContext;
  class Inc_or_dec_expressionContext;
  class ExpressionContext;
  class Operator_log_andContext;
  class Operator_log_orContext;
  class Operator_powerContext;
  class Operator_bitwise_andContext;
  class Operator_bitwise_orContext;
  class ReplicationContext;
  class Replication_sizeContext;
  class Replication_valueContext;
  class ConcatenationContext;
  class Dynamic_array_newContext;
  class Const_or_range_expressionContext;
  class Variable_decl_assignmentContext;
  class Assignment_pattern_variable_lvalueContext;
  class Stream_operatorContext;
  class Slice_sizeContext;
  class Streaming_concatenationContext;
  class Stream_concatenationContext;
  class Stream_expressionContext;
  class Array_range_expressionContext;
  class Range_separatorContext;
  class Open_range_listContext;
  class PatternContext;
  class Assignment_patternContext;
  class Replication_assignmentContext;
  class Structure_pattern_keyContext;
  class Array_pattern_keyContext;
  class Assignment_pattern_keyContext;
  class Struct_union_memberContext;
  class Data_type_or_voidContext;
  class Enum_name_declarationContext;
  class Assignment_pattern_expressionContext;
  class Assignment_pattern_expression_typeContext;
  class Net_lvalueContext;
  class Variable_lvalueContext;
  class Solve_before_listContext;
  class Constraint_block_itemContext;
  class Constraint_expressionContext;
  class Uniqueness_constraintContext;
  class Constraint_setContext;
  class Randomize_callContext;
  class Module_header_commonContext;
  class Module_declarationContext;
  class Module_keywordContext;
  class Net_port_typeContext;
  class Var_data_typeContext;
  class Net_or_var_data_typeContext;
  class List_of_defparam_assignmentsContext;
  class List_of_net_decl_assignmentsContext;
  class List_of_specparam_assignmentsContext;
  class List_of_variable_decl_assignmentsContext;
  class List_of_variable_identifiers_itemContext;
  class List_of_variable_identifiersContext;
  class List_of_variable_port_identifiersContext;
  class Defparam_assignmentContext;
  class Net_decl_assignmentContext;
  class Specparam_assignmentContext;
  class Error_limit_valueContext;
  class Reject_limit_valueContext;
  class Pulse_control_specparamContext;
  class Identifier_doted_index_at_endContext;
  class Specify_terminal_descriptorContext;
  class Specify_input_terminal_descriptorContext;
  class Specify_output_terminal_descriptorContext;
  class Specify_itemContext;
  class Pulsestyle_declarationContext;
  class Showcancelled_declarationContext;
  class Path_declarationContext;
  class Simple_path_declarationContext;
  class Path_delay_valueContext;
  class List_of_path_outputsContext;
  class List_of_path_inputsContext;
  class List_of_pathsContext;
  class List_of_path_delay_expressionsContext;
  class T_path_delay_expressionContext;
  class Trise_path_delay_expressionContext;
  class Tfall_path_delay_expressionContext;
  class Tz_path_delay_expressionContext;
  class T01_path_delay_expressionContext;
  class T10_path_delay_expressionContext;
  class T0z_path_delay_expressionContext;
  class Tz1_path_delay_expressionContext;
  class T1z_path_delay_expressionContext;
  class Tz0_path_delay_expressionContext;
  class T0x_path_delay_expressionContext;
  class Tx1_path_delay_expressionContext;
  class T1x_path_delay_expressionContext;
  class Tx0_path_delay_expressionContext;
  class Txz_path_delay_expressionContext;
  class Tzx_path_delay_expressionContext;
  class Parallel_path_descriptionContext;
  class Full_path_descriptionContext;
  class Identifier_listContext;
  class Specparam_declarationContext;
  class Edge_sensitive_path_declarationContext;
  class Parallel_edge_sensitive_path_descriptionContext;
  class Full_edge_sensitive_path_descriptionContext;
  class Data_source_expressionContext;
  class Data_declarationContext;
  class Module_path_expressionContext;
  class State_dependent_path_declarationContext;
  class Package_export_declarationContext;
  class Genvar_declarationContext;
  class Net_declarationContext;
  class Parameter_port_listContext;
  class Parameter_port_declarationContext;
  class List_of_port_declarations_ansi_itemContext;
  class List_of_port_declarationsContext;
  class Nonansi_port_declarationContext;
  class Nonansi_portContext;
  class Nonansi_port__exprContext;
  class Port_identifierContext;
  class Ansi_port_declarationContext;
  class System_timing_checkContext;
  class Dolar_setup_timing_checkContext;
  class Dolar_hold_timing_checkContext;
  class Dolar_setuphold_timing_checkContext;
  class Dolar_recovery_timing_checkContext;
  class Dolar_removal_timing_checkContext;
  class Dolar_recrem_timing_checkContext;
  class Dolar_skew_timing_checkContext;
  class Dolar_timeskew_timing_checkContext;
  class Dolar_fullskew_timing_checkContext;
  class Dolar_period_timing_checkContext;
  class Dolar_width_timing_checkContext;
  class Dolar_nochange_timing_checkContext;
  class Timecheck_conditionContext;
  class Controlled_reference_eventContext;
  class Delayed_referenceContext;
  class End_edge_offsetContext;
  class Event_based_flagContext;
  class NotifierContext;
  class Remain_active_flagContext;
  class Timestamp_conditionContext;
  class Start_edge_offsetContext;
  class ThresholdContext;
  class Timing_check_limitContext;
  class Timing_check_eventContext;
  class Timing_check_conditionContext;
  class Scalar_timing_check_conditionContext;
  class Controlled_timing_check_eventContext;
  class Function_data_type_or_implicitContext;
  class Extern_tf_declarationContext;
  class Function_declarationContext;
  class Untyped_function_declarationContext;
  class Task_prototypeContext;
  class Function_prototypeContext;
  class Dpi_import_exportContext;
  class Dpi_function_import_propertyContext;
  class Dpi_task_import_propertyContext;
  class Task_and_function_declaration_commonContext;
  class Task_declarationContext;
  class Method_prototypeContext;
  class Extern_constraint_declarationContext;
  class Constraint_blockContext;
  class Checker_port_listContext;
  class Checker_port_itemContext;
  class Checker_port_directionContext;
  class Checker_declarationContext;
  class Class_declarationContext;
  class Always_constructContext;
  class Interface_class_typeContext;
  class Interface_class_declarationContext;
  class Interface_class_itemContext;
  class Interface_class_methodContext;
  class Package_declarationContext;
  class Package_itemContext;
  class Program_declarationContext;
  class Program_headerContext;
  class Program_itemContext;
  class Non_port_program_itemContext;
  class Anonymous_programContext;
  class Anonymous_program_itemContext;
  class Sequence_declarationContext;
  class Sequence_port_listContext;
  class Sequence_port_itemContext;
  class Property_declarationContext;
  class Property_port_listContext;
  class Property_port_itemContext;
  class Continuous_assignContext;
  class Checker_or_generate_itemContext;
  class Constraint_prototypeContext;
  class Class_constraintContext;
  class Constraint_declarationContext;
  class Class_constructor_declarationContext;
  class Class_propertyContext;
  class Class_methodContext;
  class Class_constructor_prototypeContext;
  class Class_itemContext;
  class Parameter_overrideContext;
  class Gate_instantiationContext;
  class Enable_gate_or_mos_switch_or_cmos_switch_instanceContext;
  class N_input_gate_instanceContext;
  class N_output_gate_instanceContext;
  class Pass_switch_instanceContext;
  class Pass_enable_switch_instanceContext;
  class Pull_gate_instanceContext;
  class Pulldown_strengthContext;
  class Pullup_strengthContext;
  class Enable_terminalContext;
  class Inout_terminalContext;
  class Input_terminalContext;
  class Output_terminalContext;
  class Udp_instantiationContext;
  class Udp_instanceContext;
  class Udp_instance_bodyContext;
  class Module_or_interface_or_program_or_udp_instantiationContext;
  class Hierarchical_instanceContext;
  class List_of_port_connectionsContext;
  class Ordered_port_connectionContext;
  class Named_port_connectionContext;
  class Port_expression_connectionContext;
  class Port_concatenation_connectionContext;
  class Concatenation_itemContext;
  class Port_replication_connectionContext;
  class Bind_directiveContext;
  class Bind_target_instanceContext;
  class Bind_target_instance_listContext;
  class Bind_instantiationContext;
  class Config_declarationContext;
  class Design_statementContext;
  class Config_rule_statementContext;
  class Inst_clauseContext;
  class Inst_nameContext;
  class Cell_clauseContext;
  class Liblist_clauseContext;
  class Use_clauseContext;
  class Net_aliasContext;
  class Specify_blockContext;
  class Generate_regionContext;
  class Genvar_expressionContext;
  class Loop_generate_constructContext;
  class Genvar_initializationContext;
  class Genvar_iterationContext;
  class Conditional_generate_constructContext;
  class If_generate_constructContext;
  class Case_generate_constructContext;
  class Case_generate_itemContext;
  class Generate_begin_end_blockContext;
  class Generate_itemContext;
  class Program_generate_itemContext;
  class Module_or_generate_or_interface_or_checker_itemContext;
  class Module_or_generate_or_interface_itemContext;
  class Module_or_generate_itemContext;
  class Elaboration_system_taskContext;
  class Module_item_itemContext;
  class Module_itemContext; 

  class  Source_textContext : public antlr4::ParserRuleContext {
  public:
    Source_textContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    Timeunits_declarationContext *timeunits_declaration();
    std::vector<DescriptionContext *> description();
    DescriptionContext* description(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Source_textContext* source_text();

  class  DescriptionContext : public antlr4::ParserRuleContext {
  public:
    DescriptionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Module_declarationContext *module_declaration();
    Udp_declarationContext *udp_declaration();
    Interface_declarationContext *interface_declaration();
    Program_declarationContext *program_declaration();
    Package_declarationContext *package_declaration();
    Package_itemContext *package_item();
    Bind_directiveContext *bind_directive();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    Config_declarationContext *config_declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DescriptionContext* description();

  class  Assignment_operatorContext : public antlr4::ParserRuleContext {
  public:
    Assignment_operatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ASSIGN();
    antlr4::tree::TerminalNode *PLUS_ASSIGN();
    antlr4::tree::TerminalNode *MINUS_ASSIGN();
    antlr4::tree::TerminalNode *MUL_ASSIGN();
    antlr4::tree::TerminalNode *DIV_ASSIGN();
    antlr4::tree::TerminalNode *MOD_ASSIGN();
    antlr4::tree::TerminalNode *AND_ASSIGN();
    antlr4::tree::TerminalNode *OR_ASSIGN();
    antlr4::tree::TerminalNode *XOR_ASSIGN();
    antlr4::tree::TerminalNode *SHIFT_LEFT_ASSIGN();
    antlr4::tree::TerminalNode *SHIFT_RIGHT_ASSIGN();
    antlr4::tree::TerminalNode *ARITH_SHIFT_LEFT_ASSIGN();
    antlr4::tree::TerminalNode *ARITH_SHIFT_RIGHT_ASSIGN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Assignment_operatorContext* assignment_operator();

  class  Edge_identifierContext : public antlr4::ParserRuleContext {
  public:
    Edge_identifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_POSEDGE();
    antlr4::tree::TerminalNode *KW_NEGEDGE();
    antlr4::tree::TerminalNode *KW_EDGE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Edge_identifierContext* edge_identifier();

  class  IdentifierContext : public antlr4::ParserRuleContext {
  public:
    IdentifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *C_IDENTIFIER();
    antlr4::tree::TerminalNode *SIMPLE_IDENTIFIER();
    antlr4::tree::TerminalNode *ESCAPED_IDENTIFIER();
    antlr4::tree::TerminalNode *KW_SAMPLE();
    antlr4::tree::TerminalNode *KW_RANDOMIZE();
    antlr4::tree::TerminalNode *KW_TYPE_OPTION();
    antlr4::tree::TerminalNode *KW_OPTION();
    antlr4::tree::TerminalNode *KW_STD();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IdentifierContext* identifier();

  class  Integer_typeContext : public antlr4::ParserRuleContext {
  public:
    Integer_typeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Integer_vector_typeContext *integer_vector_type();
    Integer_atom_typeContext *integer_atom_type();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Integer_typeContext* integer_type();

  class  Integer_atom_typeContext : public antlr4::ParserRuleContext {
  public:
    Integer_atom_typeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_BYTE();
    antlr4::tree::TerminalNode *KW_SHORTINT();
    antlr4::tree::TerminalNode *KW_INT();
    antlr4::tree::TerminalNode *KW_LONGINT();
    antlr4::tree::TerminalNode *KW_INTEGER();
    antlr4::tree::TerminalNode *KW_TIME();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Integer_atom_typeContext* integer_atom_type();

  class  Integer_vector_typeContext : public antlr4::ParserRuleContext {
  public:
    Integer_vector_typeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_BIT();
    antlr4::tree::TerminalNode *KW_LOGIC();
    antlr4::tree::TerminalNode *KW_REG();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Integer_vector_typeContext* integer_vector_type();

  class  Non_integer_typeContext : public antlr4::ParserRuleContext {
  public:
    Non_integer_typeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_SHORTREAL();
    antlr4::tree::TerminalNode *KW_REAL();
    antlr4::tree::TerminalNode *KW_REALTIME();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Non_integer_typeContext* non_integer_type();

  class  Net_typeContext : public antlr4::ParserRuleContext {
  public:
    Net_typeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_SUPPLY0();
    antlr4::tree::TerminalNode *KW_SUPPLY1();
    antlr4::tree::TerminalNode *KW_TRI();
    antlr4::tree::TerminalNode *KW_TRIAND();
    antlr4::tree::TerminalNode *KW_TRIOR();
    antlr4::tree::TerminalNode *KW_TRIREG();
    antlr4::tree::TerminalNode *KW_TRI0();
    antlr4::tree::TerminalNode *KW_TRI1();
    antlr4::tree::TerminalNode *KW_UWIRE();
    antlr4::tree::TerminalNode *KW_WIRE();
    antlr4::tree::TerminalNode *KW_WAND();
    antlr4::tree::TerminalNode *KW_WOR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Net_typeContext* net_type();

  class  Unary_module_path_operatorContext : public antlr4::ParserRuleContext {
  public:
    Unary_module_path_operatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NOT();
    antlr4::tree::TerminalNode *NEG();
    antlr4::tree::TerminalNode *AMPERSAND();
    antlr4::tree::TerminalNode *NAND();
    antlr4::tree::TerminalNode *BAR();
    antlr4::tree::TerminalNode *NOR();
    antlr4::tree::TerminalNode *XOR();
    antlr4::tree::TerminalNode *NXOR();
    antlr4::tree::TerminalNode *XORN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Unary_module_path_operatorContext* unary_module_path_operator();

  class  Unary_operatorContext : public antlr4::ParserRuleContext {
  public:
    Unary_operatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *PLUS();
    antlr4::tree::TerminalNode *MINUS();
    Unary_module_path_operatorContext *unary_module_path_operator();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Unary_operatorContext* unary_operator();

  class  Inc_or_dec_operatorContext : public antlr4::ParserRuleContext {
  public:
    Inc_or_dec_operatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *INCR();
    antlr4::tree::TerminalNode *DECR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Inc_or_dec_operatorContext* inc_or_dec_operator();

  class  Implicit_class_handleContext : public antlr4::ParserRuleContext {
  public:
    Implicit_class_handleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_THIS();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *KW_SUPER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Implicit_class_handleContext* implicit_class_handle();

  class  Integral_numberContext : public antlr4::ParserRuleContext {
  public:
    Integral_numberContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *BASED_NUMBER_WITH_SIZE();
    antlr4::tree::TerminalNode *ANY_BASED_NUMBER();
    antlr4::tree::TerminalNode *UNSIGNED_NUMBER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Integral_numberContext* integral_number();

  class  Real_numberContext : public antlr4::ParserRuleContext {
  public:
    Real_numberContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *REAL_NUMBER_WITH_EXP();
    antlr4::tree::TerminalNode *FIXED_POINT_NUMBER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Real_numberContext* real_number();

  class  Any_system_tf_identifierContext : public antlr4::ParserRuleContext {
  public:
    Any_system_tf_identifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SYSTEM_TF_IDENTIFIER();
    antlr4::tree::TerminalNode *KW_DOLAR_SETUPHOLD();
    antlr4::tree::TerminalNode *KW_DOLAR_SETUP();
    antlr4::tree::TerminalNode *KW_DOLAR_FULLSKEW();
    antlr4::tree::TerminalNode *KW_DOLAR_WARNING();
    antlr4::tree::TerminalNode *KW_DOLAR_WIDTH();
    antlr4::tree::TerminalNode *KW_DOLAR_ROOT();
    antlr4::tree::TerminalNode *KW_DOLAR_RECOVERY();
    antlr4::tree::TerminalNode *KW_DOLAR_SKEW();
    antlr4::tree::TerminalNode *KW_DOLAR_FATAL();
    antlr4::tree::TerminalNode *KW_DOLAR_REMOVAL();
    antlr4::tree::TerminalNode *KW_DOLAR_RECREM();
    antlr4::tree::TerminalNode *KW_DOLAR_ERROR();
    antlr4::tree::TerminalNode *KW_DOLAR_PERIOD();
    antlr4::tree::TerminalNode *KW_DOLAR_HOLD();
    antlr4::tree::TerminalNode *KW_DOLAR_INFO();
    antlr4::tree::TerminalNode *KW_DOLAR_UNIT();
    antlr4::tree::TerminalNode *KW_DOLAR_TIMESKEW();
    antlr4::tree::TerminalNode *KW_DOLAR_NOCHANGE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Any_system_tf_identifierContext* any_system_tf_identifier();

  class  SigningContext : public antlr4::ParserRuleContext {
  public:
    SigningContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_SIGNED();
    antlr4::tree::TerminalNode *KW_UNSIGNED();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SigningContext* signing();

  class  NumberContext : public antlr4::ParserRuleContext {
  public:
    NumberContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Integral_numberContext *integral_number();
    Real_numberContext *real_number();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  NumberContext* number();

  class  Timeunits_declarationContext : public antlr4::ParserRuleContext {
  public:
    Timeunits_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_TIMEUNIT();
    std::vector<antlr4::tree::TerminalNode *> TIME_LITERAL();
    antlr4::tree::TerminalNode* TIME_LITERAL(size_t i);
    std::vector<antlr4::tree::TerminalNode *> SEMI();
    antlr4::tree::TerminalNode* SEMI(size_t i);
    antlr4::tree::TerminalNode *DIV();
    antlr4::tree::TerminalNode *KW_TIMEPRECISION();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Timeunits_declarationContext* timeunits_declaration();

  class  LifetimeContext : public antlr4::ParserRuleContext {
  public:
    LifetimeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_STATIC();
    antlr4::tree::TerminalNode *KW_AUTOMATIC();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LifetimeContext* lifetime();

  class  Port_directionContext : public antlr4::ParserRuleContext {
  public:
    Port_directionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_INPUT();
    antlr4::tree::TerminalNode *KW_OUTPUT();
    antlr4::tree::TerminalNode *KW_INOUT();
    antlr4::tree::TerminalNode *KW_REF();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Port_directionContext* port_direction();

  class  Always_keywordContext : public antlr4::ParserRuleContext {
  public:
    Always_keywordContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_ALWAYS();
    antlr4::tree::TerminalNode *KW_ALWAYS_COMB();
    antlr4::tree::TerminalNode *KW_ALWAYS_LATCH();
    antlr4::tree::TerminalNode *KW_ALWAYS_FF();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Always_keywordContext* always_keyword();

  class  Join_keywordContext : public antlr4::ParserRuleContext {
  public:
    Join_keywordContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_JOIN();
    antlr4::tree::TerminalNode *KW_JOIN_ANY();
    antlr4::tree::TerminalNode *KW_JOIN_NONE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Join_keywordContext* join_keyword();

  class  Unique_priorityContext : public antlr4::ParserRuleContext {
  public:
    Unique_priorityContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_UNIQUE();
    antlr4::tree::TerminalNode *KW_UNIQUE0();
    antlr4::tree::TerminalNode *KW_PRIORITY();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Unique_priorityContext* unique_priority();

  class  Drive_strengthContext : public antlr4::ParserRuleContext {
  public:
    Drive_strengthContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *KW_HIGHZ0();
    antlr4::tree::TerminalNode *COMMA();
    Strength1Context *strength1();
    antlr4::tree::TerminalNode *KW_HIGHZ1();
    Strength0Context *strength0();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Drive_strengthContext* drive_strength();

  class  Strength0Context : public antlr4::ParserRuleContext {
  public:
    Strength0Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_SUPPLY0();
    antlr4::tree::TerminalNode *KW_STRONG0();
    antlr4::tree::TerminalNode *KW_PULL0();
    antlr4::tree::TerminalNode *KW_WEAK0();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Strength0Context* strength0();

  class  Strength1Context : public antlr4::ParserRuleContext {
  public:
    Strength1Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_SUPPLY1();
    antlr4::tree::TerminalNode *KW_STRONG1();
    antlr4::tree::TerminalNode *KW_PULL1();
    antlr4::tree::TerminalNode *KW_WEAK1();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Strength1Context* strength1();

  class  Charge_strengthContext : public antlr4::ParserRuleContext {
  public:
    Charge_strengthContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *KW_SMALL();
    antlr4::tree::TerminalNode *KW_MEDIUM();
    antlr4::tree::TerminalNode *KW_LARGE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Charge_strengthContext* charge_strength();

  class  Sequence_lvar_port_directionContext : public antlr4::ParserRuleContext {
  public:
    Sequence_lvar_port_directionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_INPUT();
    antlr4::tree::TerminalNode *KW_INOUT();
    antlr4::tree::TerminalNode *KW_OUTPUT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sequence_lvar_port_directionContext* sequence_lvar_port_direction();

  class  Bins_keywordContext : public antlr4::ParserRuleContext {
  public:
    Bins_keywordContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_BINS();
    antlr4::tree::TerminalNode *KW_ILLEGAL_BINS();
    antlr4::tree::TerminalNode *KW_IGNORE_BINS();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Bins_keywordContext* bins_keyword();

  class  Class_item_qualifierContext : public antlr4::ParserRuleContext {
  public:
    Class_item_qualifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_STATIC();
    antlr4::tree::TerminalNode *KW_PROTECTED();
    antlr4::tree::TerminalNode *KW_LOCAL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Class_item_qualifierContext* class_item_qualifier();

  class  Random_qualifierContext : public antlr4::ParserRuleContext {
  public:
    Random_qualifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_RAND();
    antlr4::tree::TerminalNode *KW_RANDC();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Random_qualifierContext* random_qualifier();

  class  Property_qualifierContext : public antlr4::ParserRuleContext {
  public:
    Property_qualifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Random_qualifierContext *random_qualifier();
    Class_item_qualifierContext *class_item_qualifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Property_qualifierContext* property_qualifier();

  class  Method_qualifierContext : public antlr4::ParserRuleContext {
  public:
    Method_qualifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_VIRTUAL();
    antlr4::tree::TerminalNode *KW_PURE();
    Class_item_qualifierContext *class_item_qualifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Method_qualifierContext* method_qualifier();

  class  Constraint_prototype_qualifierContext : public antlr4::ParserRuleContext {
  public:
    Constraint_prototype_qualifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_EXTERN();
    antlr4::tree::TerminalNode *KW_PURE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Constraint_prototype_qualifierContext* constraint_prototype_qualifier();

  class  Cmos_switchtypeContext : public antlr4::ParserRuleContext {
  public:
    Cmos_switchtypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_CMOS();
    antlr4::tree::TerminalNode *KW_RCMOS();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Cmos_switchtypeContext* cmos_switchtype();

  class  Enable_gatetypeContext : public antlr4::ParserRuleContext {
  public:
    Enable_gatetypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_BUFIF0();
    antlr4::tree::TerminalNode *KW_BUFIF1();
    antlr4::tree::TerminalNode *KW_NOTIF0();
    antlr4::tree::TerminalNode *KW_NOTIF1();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Enable_gatetypeContext* enable_gatetype();

  class  Mos_switchtypeContext : public antlr4::ParserRuleContext {
  public:
    Mos_switchtypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_NMOS();
    antlr4::tree::TerminalNode *KW_PMOS();
    antlr4::tree::TerminalNode *KW_RNMOS();
    antlr4::tree::TerminalNode *KW_RPMOS();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Mos_switchtypeContext* mos_switchtype();

  class  N_input_gatetypeContext : public antlr4::ParserRuleContext {
  public:
    N_input_gatetypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_AND();
    antlr4::tree::TerminalNode *KW_NAND();
    antlr4::tree::TerminalNode *KW_OR();
    antlr4::tree::TerminalNode *KW_NOR();
    antlr4::tree::TerminalNode *KW_XOR();
    antlr4::tree::TerminalNode *KW_XNOR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  N_input_gatetypeContext* n_input_gatetype();

  class  N_output_gatetypeContext : public antlr4::ParserRuleContext {
  public:
    N_output_gatetypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_BUF();
    antlr4::tree::TerminalNode *KW_NOT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  N_output_gatetypeContext* n_output_gatetype();

  class  Pass_en_switchtypeContext : public antlr4::ParserRuleContext {
  public:
    Pass_en_switchtypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_TRANIF0();
    antlr4::tree::TerminalNode *KW_TRANIF1();
    antlr4::tree::TerminalNode *KW_RTRANIF1();
    antlr4::tree::TerminalNode *KW_RTRANIF0();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Pass_en_switchtypeContext* pass_en_switchtype();

  class  Pass_switchtypeContext : public antlr4::ParserRuleContext {
  public:
    Pass_switchtypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_TRAN();
    antlr4::tree::TerminalNode *KW_RTRAN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Pass_switchtypeContext* pass_switchtype();

  class  Any_implicationContext : public antlr4::ParserRuleContext {
  public:
    Any_implicationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IMPLIES();
    antlr4::tree::TerminalNode *IMPLIES_P();
    antlr4::tree::TerminalNode *IMPLIES_N();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Any_implicationContext* any_implication();

  class  Timing_check_event_controlContext : public antlr4::ParserRuleContext {
  public:
    Timing_check_event_controlContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_POSEDGE();
    antlr4::tree::TerminalNode *KW_NEGEDGE();
    antlr4::tree::TerminalNode *KW_EDGE();
    antlr4::tree::TerminalNode *EDGE_CONTROL_SPECIFIER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Timing_check_event_controlContext* timing_check_event_control();

  class  Import_exportContext : public antlr4::ParserRuleContext {
  public:
    Import_exportContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_IMPORT();
    antlr4::tree::TerminalNode *KW_EXPORT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Import_exportContext* import_export();

  class  Array_method_nameContext : public antlr4::ParserRuleContext {
  public:
    Array_method_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_UNIQUE();
    antlr4::tree::TerminalNode *KW_AND();
    antlr4::tree::TerminalNode *KW_OR();
    antlr4::tree::TerminalNode *KW_XOR();
    IdentifierContext *identifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Array_method_nameContext* array_method_name();

  class  Operator_mul_div_modContext : public antlr4::ParserRuleContext {
  public:
    Operator_mul_div_modContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MUL();
    antlr4::tree::TerminalNode *DIV();
    antlr4::tree::TerminalNode *MOD();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Operator_mul_div_modContext* operator_mul_div_mod();

  class  Operator_plus_minusContext : public antlr4::ParserRuleContext {
  public:
    Operator_plus_minusContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *PLUS();
    antlr4::tree::TerminalNode *MINUS();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Operator_plus_minusContext* operator_plus_minus();

  class  Operator_shiftContext : public antlr4::ParserRuleContext {
  public:
    Operator_shiftContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SHIFT_LEFT();
    antlr4::tree::TerminalNode *SHIFT_RIGHT();
    antlr4::tree::TerminalNode *ARITH_SHIFT_LEFT();
    antlr4::tree::TerminalNode *ARITH_SHIFT_RIGHT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Operator_shiftContext* operator_shift();

  class  Operator_cmpContext : public antlr4::ParserRuleContext {
  public:
    Operator_cmpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LT();
    antlr4::tree::TerminalNode *LE();
    antlr4::tree::TerminalNode *GT();
    antlr4::tree::TerminalNode *GE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Operator_cmpContext* operator_cmp();

  class  Operator_eq_neqContext : public antlr4::ParserRuleContext {
  public:
    Operator_eq_neqContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EQ();
    antlr4::tree::TerminalNode *NE();
    antlr4::tree::TerminalNode *CASE_EQ();
    antlr4::tree::TerminalNode *CASE_NE();
    antlr4::tree::TerminalNode *WILDCARD_EQ();
    antlr4::tree::TerminalNode *WILDCARD_NE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Operator_eq_neqContext* operator_eq_neq();

  class  Operator_xorContext : public antlr4::ParserRuleContext {
  public:
    Operator_xorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *XOR();
    antlr4::tree::TerminalNode *NXOR();
    antlr4::tree::TerminalNode *XORN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Operator_xorContext* operator_xor();

  class  Operator_implContext : public antlr4::ParserRuleContext {
  public:
    Operator_implContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ARROW();
    antlr4::tree::TerminalNode *BI_DIR_ARROW();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Operator_implContext* operator_impl();

  class  Udp_nonansi_declarationContext : public antlr4::ParserRuleContext {
  public:
    Udp_nonansi_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_PRIMITIVE();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *LPAREN();
    Identifier_list_2plusContext *identifier_list_2plus();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Udp_nonansi_declarationContext* udp_nonansi_declaration();

  class  Udp_ansi_declarationContext : public antlr4::ParserRuleContext {
  public:
    Udp_ansi_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_PRIMITIVE();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *LPAREN();
    Udp_declaration_port_listContext *udp_declaration_port_list();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Udp_ansi_declarationContext* udp_ansi_declaration();

  class  Udp_declarationContext : public antlr4::ParserRuleContext {
  public:
    Udp_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_EXTERN();
    Udp_nonansi_declarationContext *udp_nonansi_declaration();
    Udp_ansi_declarationContext *udp_ansi_declaration();
    Udp_bodyContext *udp_body();
    antlr4::tree::TerminalNode *KW_ENDPRIMITIVE();
    antlr4::tree::TerminalNode *COLON();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    std::vector<Udp_port_declarationContext *> udp_port_declaration();
    Udp_port_declarationContext* udp_port_declaration(size_t i);
    antlr4::tree::TerminalNode *KW_PRIMITIVE();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *MUL();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Udp_declarationContext* udp_declaration();

  class  Udp_declaration_port_listContext : public antlr4::ParserRuleContext {
  public:
    Udp_declaration_port_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Udp_output_declarationContext *udp_output_declaration();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<Udp_input_declarationContext *> udp_input_declaration();
    Udp_input_declarationContext* udp_input_declaration(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Udp_declaration_port_listContext* udp_declaration_port_list();

  class  Udp_port_declarationContext : public antlr4::ParserRuleContext {
  public:
    Udp_port_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SEMI();
    Udp_output_declarationContext *udp_output_declaration();
    Udp_input_declarationContext *udp_input_declaration();
    Udp_reg_declarationContext *udp_reg_declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Udp_port_declarationContext* udp_port_declaration();

  class  Udp_output_declarationContext : public antlr4::ParserRuleContext {
  public:
    Udp_output_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_OUTPUT();
    antlr4::tree::TerminalNode *KW_REG();
    IdentifierContext *identifier();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    antlr4::tree::TerminalNode *ASSIGN();
    Constant_expressionContext *constant_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Udp_output_declarationContext* udp_output_declaration();

  class  Udp_input_declarationContext : public antlr4::ParserRuleContext {
  public:
    Udp_input_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_INPUT();
    Identifier_listContext *identifier_list();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Udp_input_declarationContext* udp_input_declaration();

  class  Udp_reg_declarationContext : public antlr4::ParserRuleContext {
  public:
    Udp_reg_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_REG();
    IdentifierContext *identifier();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Udp_reg_declarationContext* udp_reg_declaration();

  class  Udp_bodyContext : public antlr4::ParserRuleContext {
  public:
    Udp_bodyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Combinational_bodyContext *combinational_body();
    Sequential_bodyContext *sequential_body();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Udp_bodyContext* udp_body();

  class  Combinational_bodyContext : public antlr4::ParserRuleContext {
  public:
    Combinational_bodyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_TABLE();
    antlr4::tree::TerminalNode *KW_ENDTABLE();
    std::vector<Combinational_entryContext *> combinational_entry();
    Combinational_entryContext* combinational_entry(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Combinational_bodyContext* combinational_body();

  class  Combinational_entryContext : public antlr4::ParserRuleContext {
  public:
    Combinational_entryContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Level_input_listContext *level_input_list();
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *LEVEL_SYMBOL();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Combinational_entryContext* combinational_entry();

  class  Sequential_bodyContext : public antlr4::ParserRuleContext {
  public:
    Sequential_bodyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_TABLE();
    antlr4::tree::TerminalNode *KW_ENDTABLE();
    Udp_initial_statementContext *udp_initial_statement();
    std::vector<Sequential_entryContext *> sequential_entry();
    Sequential_entryContext* sequential_entry(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sequential_bodyContext* sequential_body();

  class  Udp_initial_statementContext : public antlr4::ParserRuleContext {
  public:
    Udp_initial_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_INITIAL();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *ASSIGN();
    Integral_numberContext *integral_number();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Udp_initial_statementContext* udp_initial_statement();

  class  Sequential_entryContext : public antlr4::ParserRuleContext {
  public:
    Sequential_entryContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Seq_input_listContext *seq_input_list();
    std::vector<antlr4::tree::TerminalNode *> COLON();
    antlr4::tree::TerminalNode* COLON(size_t i);
    Current_stateContext *current_state();
    Next_stateContext *next_state();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sequential_entryContext* sequential_entry();

  class  Seq_input_listContext : public antlr4::ParserRuleContext {
  public:
    Seq_input_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Level_input_listContext *level_input_list();
    Edge_input_listContext *edge_input_list();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Seq_input_listContext* seq_input_list();

  class  Level_input_listContext : public antlr4::ParserRuleContext {
  public:
    Level_input_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> LEVEL_SYMBOL();
    antlr4::tree::TerminalNode* LEVEL_SYMBOL(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Level_input_listContext* level_input_list();

  class  Edge_input_listContext : public antlr4::ParserRuleContext {
  public:
    Edge_input_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Edge_indicatorContext *edge_indicator();
    std::vector<antlr4::tree::TerminalNode *> LEVEL_SYMBOL();
    antlr4::tree::TerminalNode* LEVEL_SYMBOL(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Edge_input_listContext* edge_input_list();

  class  Edge_indicatorContext : public antlr4::ParserRuleContext {
  public:
    Edge_indicatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<antlr4::tree::TerminalNode *> LEVEL_SYMBOL();
    antlr4::tree::TerminalNode* LEVEL_SYMBOL(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *EDGE_SYMBOL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Edge_indicatorContext* edge_indicator();

  class  Current_stateContext : public antlr4::ParserRuleContext {
  public:
    Current_stateContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LEVEL_SYMBOL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Current_stateContext* current_state();

  class  Next_stateContext : public antlr4::ParserRuleContext {
  public:
    Next_stateContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LEVEL_SYMBOL();
    antlr4::tree::TerminalNode *MINUS();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Next_stateContext* next_state();

  class  Interface_declarationContext : public antlr4::ParserRuleContext {
  public:
    Interface_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_EXTERN();
    Interface_headerContext *interface_header();
    antlr4::tree::TerminalNode *KW_ENDINTERFACE();
    antlr4::tree::TerminalNode *COLON();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *KW_INTERFACE();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *MUL();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *SEMI();
    Timeunits_declarationContext *timeunits_declaration();
    std::vector<Interface_itemContext *> interface_item();
    Interface_itemContext* interface_item(size_t i);
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interface_declarationContext* interface_declaration();

  class  Interface_headerContext : public antlr4::ParserRuleContext {
  public:
    Interface_headerContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_INTERFACE();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    LifetimeContext *lifetime();
    std::vector<Package_import_declarationContext *> package_import_declaration();
    Package_import_declarationContext* package_import_declaration(size_t i);
    Parameter_port_listContext *parameter_port_list();
    List_of_port_declarationsContext *list_of_port_declarations();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interface_headerContext* interface_header();

  class  Interface_itemContext : public antlr4::ParserRuleContext {
  public:
    Interface_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Ansi_port_declarationContext *ansi_port_declaration();
    antlr4::tree::TerminalNode *SEMI();
    Generate_regionContext *generate_region();
    Module_or_generate_or_interface_itemContext *module_or_generate_or_interface_item();
    Extern_tf_declarationContext *extern_tf_declaration();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    Program_declarationContext *program_declaration();
    Modport_declarationContext *modport_declaration();
    Interface_declarationContext *interface_declaration();
    Timeunits_declarationContext *timeunits_declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interface_itemContext* interface_item();

  class  Modport_declarationContext : public antlr4::ParserRuleContext {
  public:
    Modport_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_MODPORT();
    std::vector<Modport_itemContext *> modport_item();
    Modport_itemContext* modport_item(size_t i);
    antlr4::tree::TerminalNode *SEMI();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Modport_declarationContext* modport_declaration();

  class  Modport_itemContext : public antlr4::ParserRuleContext {
  public:
    Modport_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Modport_ports_declarationContext *> modport_ports_declaration();
    Modport_ports_declarationContext* modport_ports_declaration(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Modport_itemContext* modport_item();

  class  Modport_ports_declarationContext : public antlr4::ParserRuleContext {
  public:
    Modport_ports_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Modport_simple_ports_declarationContext *modport_simple_ports_declaration();
    Modport_tf_ports_declarationContext *modport_tf_ports_declaration();
    Modport_clocking_declarationContext *modport_clocking_declaration();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Modport_ports_declarationContext* modport_ports_declaration();

  class  Modport_clocking_declarationContext : public antlr4::ParserRuleContext {
  public:
    Modport_clocking_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_CLOCKING();
    IdentifierContext *identifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Modport_clocking_declarationContext* modport_clocking_declaration();

  class  Modport_simple_ports_declarationContext : public antlr4::ParserRuleContext {
  public:
    Modport_simple_ports_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Port_directionContext *port_direction();
    std::vector<Modport_simple_portContext *> modport_simple_port();
    Modport_simple_portContext* modport_simple_port(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Modport_simple_ports_declarationContext* modport_simple_ports_declaration();

  class  Modport_simple_portContext : public antlr4::ParserRuleContext {
  public:
    Modport_simple_portContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    List_of_arguments_named_itemContext *list_of_arguments_named_item();
    IdentifierContext *identifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Modport_simple_portContext* modport_simple_port();

  class  Modport_tf_ports_declarationContext : public antlr4::ParserRuleContext {
  public:
    Modport_tf_ports_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Import_exportContext *import_export();
    std::vector<Modport_tf_portContext *> modport_tf_port();
    Modport_tf_portContext* modport_tf_port(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Modport_tf_ports_declarationContext* modport_tf_ports_declaration();

  class  Modport_tf_portContext : public antlr4::ParserRuleContext {
  public:
    Modport_tf_portContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Method_prototypeContext *method_prototype();
    IdentifierContext *identifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Modport_tf_portContext* modport_tf_port();

  class  Statement_or_nullContext : public antlr4::ParserRuleContext {
  public:
    Statement_or_nullContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    StatementContext *statement();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Statement_or_nullContext* statement_or_null();

  class  Initial_constructContext : public antlr4::ParserRuleContext {
  public:
    Initial_constructContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_INITIAL();
    Statement_or_nullContext *statement_or_null();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Initial_constructContext* initial_construct();

  class  Default_clocking_or_dissable_constructContext : public antlr4::ParserRuleContext {
  public:
    Default_clocking_or_dissable_constructContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_DEFAULT();
    antlr4::tree::TerminalNode *KW_CLOCKING();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *KW_DISABLE();
    antlr4::tree::TerminalNode *KW_IFF();
    Expression_or_distContext *expression_or_dist();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Default_clocking_or_dissable_constructContext* default_clocking_or_dissable_construct();

  class  StatementContext : public antlr4::ParserRuleContext {
  public:
    StatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Statement_itemContext *statement_item();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *COLON();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  StatementContext* statement();

  class  Statement_itemContext : public antlr4::ParserRuleContext {
  public:
    Statement_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SEMI();
    Blocking_assignmentContext *blocking_assignment();
    Nonblocking_assignmentContext *nonblocking_assignment();
    Procedural_continuous_assignmentContext *procedural_continuous_assignment();
    Inc_or_dec_expressionContext *inc_or_dec_expression();
    PrimaryContext *primary();
    Clocking_driveContext *clocking_drive();
    Case_statementContext *case_statement();
    Conditional_statementContext *conditional_statement();
    Subroutine_call_statementContext *subroutine_call_statement();
    Disable_statementContext *disable_statement();
    Event_triggerContext *event_trigger();
    Loop_statementContext *loop_statement();
    Jump_statementContext *jump_statement();
    Par_blockContext *par_block();
    Procedural_timing_control_statementContext *procedural_timing_control_statement();
    Seq_blockContext *seq_block();
    Wait_statementContext *wait_statement();
    Procedural_assertion_statementContext *procedural_assertion_statement();
    Randsequence_statementContext *randsequence_statement();
    Randcase_statementContext *randcase_statement();
    Expect_property_statementContext *expect_property_statement();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Statement_itemContext* statement_item();

  class  Cycle_delayContext : public antlr4::ParserRuleContext {
  public:
    Cycle_delayContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DOUBLE_HASH();
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();
    Integral_numberContext *integral_number();
    IdentifierContext *identifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Cycle_delayContext* cycle_delay();

  class  Clocking_driveContext : public antlr4::ParserRuleContext {
  public:
    Clocking_driveContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Clockvar_expressionContext *clockvar_expression();
    antlr4::tree::TerminalNode *LE();
    Cycle_delayContext *cycle_delay();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Clocking_driveContext* clocking_drive();

  class  Clockvar_expressionContext : public antlr4::ParserRuleContext {
  public:
    Clockvar_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Hierarchical_identifierContext *hierarchical_identifier();
    SelectContext *select();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Clockvar_expressionContext* clockvar_expression();

  class  Final_constructContext : public antlr4::ParserRuleContext {
  public:
    Final_constructContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_FINAL();
    StatementContext *statement();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Final_constructContext* final_construct();

  class  Blocking_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Blocking_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Variable_lvalueContext *variable_lvalue();
    antlr4::tree::TerminalNode *ASSIGN();
    Delay_or_event_controlContext *delay_or_event_control();
    ExpressionContext *expression();
    Dynamic_array_newContext *dynamic_array_new();
    Package_or_class_scoped_hier_id_with_selectContext *package_or_class_scoped_hier_id_with_select();
    Class_newContext *class_new();
    Operator_assignmentContext *operator_assignment();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Blocking_assignmentContext* blocking_assignment();

  class  Procedural_timing_control_statementContext : public antlr4::ParserRuleContext {
  public:
    Procedural_timing_control_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Procedural_timing_controlContext *procedural_timing_control();
    Statement_or_nullContext *statement_or_null();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Procedural_timing_control_statementContext* procedural_timing_control_statement();

  class  Procedural_timing_controlContext : public antlr4::ParserRuleContext {
  public:
    Procedural_timing_controlContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Delay_controlContext *delay_control();
    Event_controlContext *event_control();
    Cycle_delayContext *cycle_delay();
    Cycle_delay_rangeContext *cycle_delay_range();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Procedural_timing_controlContext* procedural_timing_control();

  class  Event_controlContext : public antlr4::ParserRuleContext {
  public:
    Event_controlContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *AT();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *MUL();
    Package_or_class_scoped_hier_id_with_selectContext *package_or_class_scoped_hier_id_with_select();
    Event_expressionContext *event_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Event_controlContext* event_control();

  class  Delay_or_event_controlContext : public antlr4::ParserRuleContext {
  public:
    Delay_or_event_controlContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Delay_controlContext *delay_control();
    Event_controlContext *event_control();
    antlr4::tree::TerminalNode *KW_REPEAT();
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Delay_or_event_controlContext* delay_or_event_control();

  class  Delay3Context : public antlr4::ParserRuleContext {
  public:
    Delay3Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *HASH();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Mintypmax_expressionContext *> mintypmax_expression();
    Mintypmax_expressionContext* mintypmax_expression(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    Delay_valueContext *delay_value();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Delay3Context* delay3();

  class  Delay2Context : public antlr4::ParserRuleContext {
  public:
    Delay2Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *HASH();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Mintypmax_expressionContext *> mintypmax_expression();
    Mintypmax_expressionContext* mintypmax_expression(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    Delay_valueContext *delay_value();
    antlr4::tree::TerminalNode *COMMA();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Delay2Context* delay2();

  class  Delay_valueContext : public antlr4::ParserRuleContext {
  public:
    Delay_valueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *UNSIGNED_NUMBER();
    antlr4::tree::TerminalNode *TIME_LITERAL();
    antlr4::tree::TerminalNode *KW_1STEP();
    Real_numberContext *real_number();
    Ps_identifierContext *ps_identifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Delay_valueContext* delay_value();

  class  Delay_controlContext : public antlr4::ParserRuleContext {
  public:
    Delay_controlContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *HASH();
    antlr4::tree::TerminalNode *LPAREN();
    Mintypmax_expressionContext *mintypmax_expression();
    antlr4::tree::TerminalNode *RPAREN();
    Delay_valueContext *delay_value();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Delay_controlContext* delay_control();

  class  Nonblocking_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Nonblocking_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Variable_lvalueContext *variable_lvalue();
    antlr4::tree::TerminalNode *LE();
    ExpressionContext *expression();
    Delay_or_event_controlContext *delay_or_event_control();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Nonblocking_assignmentContext* nonblocking_assignment();

  class  Procedural_continuous_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Procedural_continuous_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_ASSIGN();
    Variable_assignmentContext *variable_assignment();
    antlr4::tree::TerminalNode *KW_DEASSIGN();
    Variable_lvalueContext *variable_lvalue();
    antlr4::tree::TerminalNode *KW_FORCE();
    antlr4::tree::TerminalNode *KW_RELEASE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Procedural_continuous_assignmentContext* procedural_continuous_assignment();

  class  Variable_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Variable_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Variable_lvalueContext *variable_lvalue();
    antlr4::tree::TerminalNode *ASSIGN();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Variable_assignmentContext* variable_assignment();

  class  Action_blockContext : public antlr4::ParserRuleContext {
  public:
    Action_blockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_ELSE();
    std::vector<Statement_or_nullContext *> statement_or_null();
    Statement_or_nullContext* statement_or_null(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Action_blockContext* action_block();

  class  Seq_blockContext : public antlr4::ParserRuleContext {
  public:
    Seq_blockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_BEGIN();
    antlr4::tree::TerminalNode *KW_END();
    std::vector<antlr4::tree::TerminalNode *> COLON();
    antlr4::tree::TerminalNode* COLON(size_t i);
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    std::vector<Block_item_declarationContext *> block_item_declaration();
    Block_item_declarationContext* block_item_declaration(size_t i);
    std::vector<Statement_or_nullContext *> statement_or_null();
    Statement_or_nullContext* statement_or_null(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Seq_blockContext* seq_block();

  class  Par_blockContext : public antlr4::ParserRuleContext {
  public:
    Par_blockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_FORK();
    Join_keywordContext *join_keyword();
    std::vector<antlr4::tree::TerminalNode *> COLON();
    antlr4::tree::TerminalNode* COLON(size_t i);
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    std::vector<Block_item_declarationContext *> block_item_declaration();
    Block_item_declarationContext* block_item_declaration(size_t i);
    std::vector<Statement_or_nullContext *> statement_or_null();
    Statement_or_nullContext* statement_or_null(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Par_blockContext* par_block();

  class  Case_statementContext : public antlr4::ParserRuleContext {
  public:
    Case_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_ENDCASE();
    antlr4::tree::TerminalNode *KW_CASE();
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *KW_INSIDE();
    Case_keywordContext *case_keyword();
    Unique_priorityContext *unique_priority();
    antlr4::tree::TerminalNode *KW_MATCHES();
    std::vector<Case_inside_itemContext *> case_inside_item();
    Case_inside_itemContext* case_inside_item(size_t i);
    std::vector<Case_pattern_itemContext *> case_pattern_item();
    Case_pattern_itemContext* case_pattern_item(size_t i);
    std::vector<Case_itemContext *> case_item();
    Case_itemContext* case_item(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Case_statementContext* case_statement();

  class  Case_keywordContext : public antlr4::ParserRuleContext {
  public:
    Case_keywordContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_CASE();
    antlr4::tree::TerminalNode *KW_CASEZ();
    antlr4::tree::TerminalNode *KW_CASEX();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Case_keywordContext* case_keyword();

  class  Case_itemContext : public antlr4::ParserRuleContext {
  public:
    Case_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Statement_or_nullContext *statement_or_null();
    antlr4::tree::TerminalNode *KW_DEFAULT();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    antlr4::tree::TerminalNode *COLON();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Case_itemContext* case_item();

  class  Case_pattern_itemContext : public antlr4::ParserRuleContext {
  public:
    Case_pattern_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Statement_or_nullContext *statement_or_null();
    antlr4::tree::TerminalNode *KW_DEFAULT();
    PatternContext *pattern();
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *TRIPLE_AND();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Case_pattern_itemContext* case_pattern_item();

  class  Case_inside_itemContext : public antlr4::ParserRuleContext {
  public:
    Case_inside_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Statement_or_nullContext *statement_or_null();
    antlr4::tree::TerminalNode *KW_DEFAULT();
    Open_range_listContext *open_range_list();
    antlr4::tree::TerminalNode *COLON();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Case_inside_itemContext* case_inside_item();

  class  Randcase_statementContext : public antlr4::ParserRuleContext {
  public:
    Randcase_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_RANDCASE();
    antlr4::tree::TerminalNode *KW_ENDCASE();
    std::vector<Randcase_itemContext *> randcase_item();
    Randcase_itemContext* randcase_item(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Randcase_statementContext* randcase_statement();

  class  Randcase_itemContext : public antlr4::ParserRuleContext {
  public:
    Randcase_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *COLON();
    Statement_or_nullContext *statement_or_null();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Randcase_itemContext* randcase_item();

  class  Cond_predicateContext : public antlr4::ParserRuleContext {
  public:
    Cond_predicateContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    std::vector<antlr4::tree::TerminalNode *> KW_MATCHES();
    antlr4::tree::TerminalNode* KW_MATCHES(size_t i);
    std::vector<PatternContext *> pattern();
    PatternContext* pattern(size_t i);
    std::vector<antlr4::tree::TerminalNode *> TRIPLE_AND();
    antlr4::tree::TerminalNode* TRIPLE_AND(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Cond_predicateContext* cond_predicate();

  class  Conditional_statementContext : public antlr4::ParserRuleContext {
  public:
    Conditional_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_IF();
    antlr4::tree::TerminalNode *LPAREN();
    Cond_predicateContext *cond_predicate();
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<Statement_or_nullContext *> statement_or_null();
    Statement_or_nullContext* statement_or_null(size_t i);
    antlr4::tree::TerminalNode *KW_ELSE();
    Unique_priorityContext *unique_priority();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Conditional_statementContext* conditional_statement();

  class  Subroutine_call_statementContext : public antlr4::ParserRuleContext {
  public:
    Subroutine_call_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_VOID();
    antlr4::tree::TerminalNode *APOSTROPHE();
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Subroutine_call_statementContext* subroutine_call_statement();

  class  Disable_statementContext : public antlr4::ParserRuleContext {
  public:
    Disable_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_DISABLE();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_FORK();
    Hierarchical_identifierContext *hierarchical_identifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Disable_statementContext* disable_statement();

  class  Event_triggerContext : public antlr4::ParserRuleContext {
  public:
    Event_triggerContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Hierarchical_identifierContext *hierarchical_identifier();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *ARROW();
    antlr4::tree::TerminalNode *DOUBLE_RIGHT_ARROW();
    Delay_or_event_controlContext *delay_or_event_control();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Event_triggerContext* event_trigger();

  class  Loop_statementContext : public antlr4::ParserRuleContext {
  public:
    Loop_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Statement_or_nullContext *statement_or_null();
    antlr4::tree::TerminalNode *KW_FOREVER();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *KW_FOR();
    std::vector<antlr4::tree::TerminalNode *> SEMI();
    antlr4::tree::TerminalNode* SEMI(size_t i);
    antlr4::tree::TerminalNode *KW_REPEAT();
    antlr4::tree::TerminalNode *KW_WHILE();
    For_initializationContext *for_initialization();
    For_end_expressionContext *for_end_expression();
    For_stepContext *for_step();
    antlr4::tree::TerminalNode *KW_DO();
    antlr4::tree::TerminalNode *KW_FOREACH();
    Package_or_class_scoped_hier_id_with_selectContext *package_or_class_scoped_hier_id_with_select();
    antlr4::tree::TerminalNode *LSQUARE_BR();
    Loop_variablesContext *loop_variables();
    antlr4::tree::TerminalNode *RSQUARE_BR();
    StatementContext *statement();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Loop_statementContext* loop_statement();

  class  List_of_variable_assignmentsContext : public antlr4::ParserRuleContext {
  public:
    List_of_variable_assignmentsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Variable_assignmentContext *> variable_assignment();
    Variable_assignmentContext* variable_assignment(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  List_of_variable_assignmentsContext* list_of_variable_assignments();

  class  For_initializationContext : public antlr4::ParserRuleContext {
  public:
    For_initializationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    List_of_variable_assignmentsContext *list_of_variable_assignments();
    std::vector<For_variable_declarationContext *> for_variable_declaration();
    For_variable_declarationContext* for_variable_declaration(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  For_initializationContext* for_initialization();

  class  For_end_expressionContext : public antlr4::ParserRuleContext {
  public:
    For_end_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  For_end_expressionContext* for_end_expression();

  class  For_variable_declaration_var_assignContext : public antlr4::ParserRuleContext {
  public:
    For_variable_declaration_var_assignContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *ASSIGN();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  For_variable_declaration_var_assignContext* for_variable_declaration_var_assign();

  class  For_variable_declarationContext : public antlr4::ParserRuleContext {
  public:
    For_variable_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Data_typeContext *data_type();
    std::vector<For_variable_declaration_var_assignContext *> for_variable_declaration_var_assign();
    For_variable_declaration_var_assignContext* for_variable_declaration_var_assign(size_t i);
    antlr4::tree::TerminalNode *KW_VAR();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  For_variable_declarationContext* for_variable_declaration();

  class  For_stepContext : public antlr4::ParserRuleContext {
  public:
    For_stepContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Sequence_match_itemContext *> sequence_match_item();
    Sequence_match_itemContext* sequence_match_item(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  For_stepContext* for_step();

  class  Loop_variablesContext : public antlr4::ParserRuleContext {
  public:
    Loop_variablesContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Loop_variablesContext* loop_variables();

  class  Jump_statementContext : public antlr4::ParserRuleContext {
  public:
    Jump_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_RETURN();
    antlr4::tree::TerminalNode *KW_BREAK();
    antlr4::tree::TerminalNode *KW_CONTINUE();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Jump_statementContext* jump_statement();

  class  Wait_statementContext : public antlr4::ParserRuleContext {
  public:
    Wait_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_WAIT();
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();
    Statement_or_nullContext *statement_or_null();
    antlr4::tree::TerminalNode *KW_FORK();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_WAIT_ORDER();
    std::vector<Hierarchical_identifierContext *> hierarchical_identifier();
    Hierarchical_identifierContext* hierarchical_identifier(size_t i);
    Action_blockContext *action_block();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Wait_statementContext* wait_statement();

  class  Name_of_instanceContext : public antlr4::ParserRuleContext {
  public:
    Name_of_instanceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    std::vector<Unpacked_dimensionContext *> unpacked_dimension();
    Unpacked_dimensionContext* unpacked_dimension(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Name_of_instanceContext* name_of_instance();

  class  Checker_instantiationContext : public antlr4::ParserRuleContext {
  public:
    Checker_instantiationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Ps_identifierContext *ps_identifier();
    Name_of_instanceContext *name_of_instance();
    antlr4::tree::TerminalNode *LPAREN();
    List_of_checker_port_connectionsContext *list_of_checker_port_connections();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Checker_instantiationContext* checker_instantiation();

  class  List_of_checker_port_connectionsContext : public antlr4::ParserRuleContext {
  public:
    List_of_checker_port_connectionsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Ordered_checker_port_connectionContext *> ordered_checker_port_connection();
    Ordered_checker_port_connectionContext* ordered_checker_port_connection(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<Named_checker_port_connectionContext *> named_checker_port_connection();
    Named_checker_port_connectionContext* named_checker_port_connection(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  List_of_checker_port_connectionsContext* list_of_checker_port_connections();

  class  Ordered_checker_port_connectionContext : public antlr4::ParserRuleContext {
  public:
    Ordered_checker_port_connectionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    Property_actual_argContext *property_actual_arg();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Ordered_checker_port_connectionContext* ordered_checker_port_connection();

  class  Named_checker_port_connectionContext : public antlr4::ParserRuleContext {
  public:
    Named_checker_port_connectionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *MUL();
    IdentifierContext *identifier();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    Property_actual_argContext *property_actual_arg();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Named_checker_port_connectionContext* named_checker_port_connection();

  class  Procedural_assertion_statementContext : public antlr4::ParserRuleContext {
  public:
    Procedural_assertion_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Concurrent_assertion_statementContext *concurrent_assertion_statement();
    Immediate_assertion_statementContext *immediate_assertion_statement();
    Checker_instantiationContext *checker_instantiation();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Procedural_assertion_statementContext* procedural_assertion_statement();

  class  Concurrent_assertion_statementContext : public antlr4::ParserRuleContext {
  public:
    Concurrent_assertion_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_PROPERTY();
    std::vector<antlr4::tree::TerminalNode *> LPAREN();
    antlr4::tree::TerminalNode* LPAREN(size_t i);
    Property_specContext *property_spec();
    std::vector<antlr4::tree::TerminalNode *> RPAREN();
    antlr4::tree::TerminalNode* RPAREN(size_t i);
    Action_blockContext *action_block();
    antlr4::tree::TerminalNode *KW_ASSERT();
    antlr4::tree::TerminalNode *KW_ASSUME();
    antlr4::tree::TerminalNode *KW_COVER();
    Statement_or_nullContext *statement_or_null();
    antlr4::tree::TerminalNode *KW_SEQUENCE();
    Sequence_exprContext *sequence_expr();
    Clocking_eventContext *clocking_event();
    antlr4::tree::TerminalNode *KW_DISABLE();
    antlr4::tree::TerminalNode *KW_IFF();
    Expression_or_distContext *expression_or_dist();
    antlr4::tree::TerminalNode *KW_RESTRICT();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Concurrent_assertion_statementContext* concurrent_assertion_statement();

  class  Assertion_itemContext : public antlr4::ParserRuleContext {
  public:
    Assertion_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Concurrent_assertion_itemContext *concurrent_assertion_item();
    Deferred_immediate_assertion_statementContext *deferred_immediate_assertion_statement();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *COLON();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Assertion_itemContext* assertion_item();

  class  Concurrent_assertion_itemContext : public antlr4::ParserRuleContext {
  public:
    Concurrent_assertion_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Concurrent_assertion_statementContext *concurrent_assertion_statement();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *COLON();
    Checker_instantiationContext *checker_instantiation();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Concurrent_assertion_itemContext* concurrent_assertion_item();

  class  Immediate_assertion_statementContext : public antlr4::ParserRuleContext {
  public:
    Immediate_assertion_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Simple_immediate_assertion_statementContext *simple_immediate_assertion_statement();
    Deferred_immediate_assertion_statementContext *deferred_immediate_assertion_statement();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Immediate_assertion_statementContext* immediate_assertion_statement();

  class  Simple_immediate_assertion_statementContext : public antlr4::ParserRuleContext {
  public:
    Simple_immediate_assertion_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Simple_immediate_assert_statementContext *simple_immediate_assert_statement();
    Simple_immediate_assume_statementContext *simple_immediate_assume_statement();
    Simple_immediate_cover_statementContext *simple_immediate_cover_statement();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Simple_immediate_assertion_statementContext* simple_immediate_assertion_statement();

  class  Simple_immediate_assert_statementContext : public antlr4::ParserRuleContext {
  public:
    Simple_immediate_assert_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_ASSERT();
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();
    Action_blockContext *action_block();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Simple_immediate_assert_statementContext* simple_immediate_assert_statement();

  class  Simple_immediate_assume_statementContext : public antlr4::ParserRuleContext {
  public:
    Simple_immediate_assume_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_ASSUME();
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();
    Action_blockContext *action_block();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Simple_immediate_assume_statementContext* simple_immediate_assume_statement();

  class  Simple_immediate_cover_statementContext : public antlr4::ParserRuleContext {
  public:
    Simple_immediate_cover_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_COVER();
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();
    Statement_or_nullContext *statement_or_null();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Simple_immediate_cover_statementContext* simple_immediate_cover_statement();

  class  Deferred_immediate_assertion_statementContext : public antlr4::ParserRuleContext {
  public:
    Deferred_immediate_assertion_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Deferred_immediate_assert_statementContext *deferred_immediate_assert_statement();
    Deferred_immediate_assume_statementContext *deferred_immediate_assume_statement();
    Deferred_immediate_cover_statementContext *deferred_immediate_cover_statement();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Deferred_immediate_assertion_statementContext* deferred_immediate_assertion_statement();

  class  Primitive_delayContext : public antlr4::ParserRuleContext {
  public:
    Primitive_delayContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *HASH();
    antlr4::tree::TerminalNode *UNSIGNED_NUMBER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Primitive_delayContext* primitive_delay();

  class  Deferred_immediate_assert_statementContext : public antlr4::ParserRuleContext {
  public:
    Deferred_immediate_assert_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_ASSERT();
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();
    Action_blockContext *action_block();
    antlr4::tree::TerminalNode *KW_FINAL();
    Primitive_delayContext *primitive_delay();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Deferred_immediate_assert_statementContext* deferred_immediate_assert_statement();

  class  Deferred_immediate_assume_statementContext : public antlr4::ParserRuleContext {
  public:
    Deferred_immediate_assume_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_ASSUME();
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();
    Action_blockContext *action_block();
    antlr4::tree::TerminalNode *KW_FINAL();
    Primitive_delayContext *primitive_delay();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Deferred_immediate_assume_statementContext* deferred_immediate_assume_statement();

  class  Deferred_immediate_cover_statementContext : public antlr4::ParserRuleContext {
  public:
    Deferred_immediate_cover_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_COVER();
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();
    Statement_or_nullContext *statement_or_null();
    antlr4::tree::TerminalNode *KW_FINAL();
    Primitive_delayContext *primitive_delay();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Deferred_immediate_cover_statementContext* deferred_immediate_cover_statement();

  class  Weight_specificationContext : public antlr4::ParserRuleContext {
  public:
    Weight_specificationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();
    Integral_numberContext *integral_number();
    Ps_identifierContext *ps_identifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Weight_specificationContext* weight_specification();

  class  Production_itemContext : public antlr4::ParserRuleContext {
  public:
    Production_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    List_of_argumentsContext *list_of_arguments();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Production_itemContext* production_item();

  class  Rs_code_blockContext : public antlr4::ParserRuleContext {
  public:
    Rs_code_blockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<Data_declarationContext *> data_declaration();
    Data_declarationContext* data_declaration(size_t i);
    std::vector<Statement_or_nullContext *> statement_or_null();
    Statement_or_nullContext* statement_or_null(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Rs_code_blockContext* rs_code_block();

  class  Randsequence_statementContext : public antlr4::ParserRuleContext {
  public:
    Randsequence_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_RANDSEQUENCE();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *KW_ENDSEQUENCE();
    IdentifierContext *identifier();
    std::vector<ProductionContext *> production();
    ProductionContext* production(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Randsequence_statementContext* randsequence_statement();

  class  Rs_prodContext : public antlr4::ParserRuleContext {
  public:
    Rs_prodContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Production_itemContext *production_item();
    Rs_code_blockContext *rs_code_block();
    Rs_if_elseContext *rs_if_else();
    Rs_repeatContext *rs_repeat();
    Rs_caseContext *rs_case();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Rs_prodContext* rs_prod();

  class  Rs_if_elseContext : public antlr4::ParserRuleContext {
  public:
    Rs_if_elseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_IF();
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<Production_itemContext *> production_item();
    Production_itemContext* production_item(size_t i);
    antlr4::tree::TerminalNode *KW_ELSE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Rs_if_elseContext* rs_if_else();

  class  Rs_repeatContext : public antlr4::ParserRuleContext {
  public:
    Rs_repeatContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_REPEAT();
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();
    Production_itemContext *production_item();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Rs_repeatContext* rs_repeat();

  class  Rs_caseContext : public antlr4::ParserRuleContext {
  public:
    Rs_caseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_CASE();
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *KW_ENDCASE();
    std::vector<Rs_case_itemContext *> rs_case_item();
    Rs_case_itemContext* rs_case_item(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Rs_caseContext* rs_case();

  class  Rs_case_itemContext : public antlr4::ParserRuleContext {
  public:
    Rs_case_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Production_itemContext *production_item();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_DEFAULT();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    antlr4::tree::TerminalNode *COLON();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Rs_case_itemContext* rs_case_item();

  class  Rs_ruleContext : public antlr4::ParserRuleContext {
  public:
    Rs_ruleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Rs_production_listContext *rs_production_list();
    antlr4::tree::TerminalNode *DIST_WEIGHT_ASSIGN();
    Weight_specificationContext *weight_specification();
    Rs_code_blockContext *rs_code_block();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Rs_ruleContext* rs_rule();

  class  Rs_production_listContext : public antlr4::ParserRuleContext {
  public:
    Rs_production_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_RAND();
    antlr4::tree::TerminalNode *KW_JOIN();
    std::vector<Production_itemContext *> production_item();
    Production_itemContext* production_item(size_t i);
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<Rs_prodContext *> rs_prod();
    Rs_prodContext* rs_prod(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Rs_production_listContext* rs_production_list();

  class  ProductionContext : public antlr4::ParserRuleContext {
  public:
    ProductionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *COLON();
    std::vector<Rs_ruleContext *> rs_rule();
    Rs_ruleContext* rs_rule(size_t i);
    antlr4::tree::TerminalNode *SEMI();
    Data_type_or_voidContext *data_type_or_void();
    antlr4::tree::TerminalNode *LPAREN();
    Tf_port_listContext *tf_port_list();
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<antlr4::tree::TerminalNode *> BAR();
    antlr4::tree::TerminalNode* BAR(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ProductionContext* production();

  class  Tf_item_declarationContext : public antlr4::ParserRuleContext {
  public:
    Tf_item_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Block_item_declarationContext *block_item_declaration();
    Tf_port_declarationContext *tf_port_declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Tf_item_declarationContext* tf_item_declaration();

  class  Tf_port_listContext : public antlr4::ParserRuleContext {
  public:
    Tf_port_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Tf_port_itemContext *> tf_port_item();
    Tf_port_itemContext* tf_port_item(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Tf_port_listContext* tf_port_list();

  class  Tf_port_itemContext : public antlr4::ParserRuleContext {
  public:
    Tf_port_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    Tf_port_directionContext *tf_port_direction();
    antlr4::tree::TerminalNode *KW_VAR();
    Data_type_or_implicitContext *data_type_or_implicit();
    IdentifierContext *identifier();
    std::vector<Variable_dimensionContext *> variable_dimension();
    Variable_dimensionContext* variable_dimension(size_t i);
    antlr4::tree::TerminalNode *ASSIGN();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Tf_port_itemContext* tf_port_item();

  class  Tf_port_directionContext : public antlr4::ParserRuleContext {
  public:
    Tf_port_directionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_CONST();
    antlr4::tree::TerminalNode *KW_REF();
    Port_directionContext *port_direction();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Tf_port_directionContext* tf_port_direction();

  class  Tf_port_declarationContext : public antlr4::ParserRuleContext {
  public:
    Tf_port_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Tf_port_directionContext *tf_port_direction();
    List_of_tf_variable_identifiersContext *list_of_tf_variable_identifiers();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    antlr4::tree::TerminalNode *KW_VAR();
    Data_type_or_implicitContext *data_type_or_implicit();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Tf_port_declarationContext* tf_port_declaration();

  class  List_of_tf_variable_identifiers_itemContext : public antlr4::ParserRuleContext {
  public:
    List_of_tf_variable_identifiers_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    std::vector<Variable_dimensionContext *> variable_dimension();
    Variable_dimensionContext* variable_dimension(size_t i);
    antlr4::tree::TerminalNode *ASSIGN();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  List_of_tf_variable_identifiers_itemContext* list_of_tf_variable_identifiers_item();

  class  List_of_tf_variable_identifiersContext : public antlr4::ParserRuleContext {
  public:
    List_of_tf_variable_identifiersContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<List_of_tf_variable_identifiers_itemContext *> list_of_tf_variable_identifiers_item();
    List_of_tf_variable_identifiers_itemContext* list_of_tf_variable_identifiers_item(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  List_of_tf_variable_identifiersContext* list_of_tf_variable_identifiers();

  class  Expect_property_statementContext : public antlr4::ParserRuleContext {
  public:
    Expect_property_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_EXPECT();
    antlr4::tree::TerminalNode *LPAREN();
    Property_specContext *property_spec();
    antlr4::tree::TerminalNode *RPAREN();
    Action_blockContext *action_block();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Expect_property_statementContext* expect_property_statement();

  class  Block_item_declarationContext : public antlr4::ParserRuleContext {
  public:
    Block_item_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Data_declarationContext *data_declaration();
    antlr4::tree::TerminalNode *SEMI();
    Let_declarationContext *let_declaration();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    Local_parameter_declarationContext *local_parameter_declaration();
    Parameter_declarationContext *parameter_declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Block_item_declarationContext* block_item_declaration();

  class  Param_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Param_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    std::vector<Unpacked_dimensionContext *> unpacked_dimension();
    Unpacked_dimensionContext* unpacked_dimension(size_t i);
    antlr4::tree::TerminalNode *ASSIGN();
    Constant_param_expressionContext *constant_param_expression();
    antlr4::tree::TerminalNode *LBRACE();
    Replication_assignmentContext *replication_assignment();
    antlr4::tree::TerminalNode *RBRACE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Param_assignmentContext* param_assignment();

  class  Type_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Type_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *ASSIGN();
    Data_typeContext *data_type();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Type_assignmentContext* type_assignment();

  class  List_of_type_assignmentsContext : public antlr4::ParserRuleContext {
  public:
    List_of_type_assignmentsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Type_assignmentContext *> type_assignment();
    Type_assignmentContext* type_assignment(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  List_of_type_assignmentsContext* list_of_type_assignments();

  class  List_of_param_assignmentsContext : public antlr4::ParserRuleContext {
  public:
    List_of_param_assignmentsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Param_assignmentContext *> param_assignment();
    Param_assignmentContext* param_assignment(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  List_of_param_assignmentsContext* list_of_param_assignments();

  class  Local_parameter_declarationContext : public antlr4::ParserRuleContext {
  public:
    Local_parameter_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_LOCALPARAM();
    antlr4::tree::TerminalNode *KW_TYPE();
    List_of_type_assignmentsContext *list_of_type_assignments();
    List_of_param_assignmentsContext *list_of_param_assignments();
    Data_type_or_implicitContext *data_type_or_implicit();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Local_parameter_declarationContext* local_parameter_declaration();

  class  Parameter_declarationContext : public antlr4::ParserRuleContext {
  public:
    Parameter_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_PARAMETER();
    antlr4::tree::TerminalNode *KW_TYPE();
    List_of_type_assignmentsContext *list_of_type_assignments();
    List_of_param_assignmentsContext *list_of_param_assignments();
    Data_type_or_implicitContext *data_type_or_implicit();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Parameter_declarationContext* parameter_declaration();

  class  Type_declarationContext : public antlr4::ParserRuleContext {
  public:
    Type_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_TYPEDEF();
    antlr4::tree::TerminalNode *SEMI();
    Data_typeContext *data_type();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    std::vector<Variable_dimensionContext *> variable_dimension();
    Variable_dimensionContext* variable_dimension(size_t i);
    antlr4::tree::TerminalNode *KW_ENUM();
    antlr4::tree::TerminalNode *KW_STRUCT();
    antlr4::tree::TerminalNode *KW_UNION();
    Identifier_with_bit_selectContext *identifier_with_bit_select();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *KW_CLASS();
    antlr4::tree::TerminalNode *KW_INTERFACE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Type_declarationContext* type_declaration();

  class  Net_type_declarationContext : public antlr4::ParserRuleContext {
  public:
    Net_type_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_NETTYPE();
    antlr4::tree::TerminalNode *SEMI();
    Data_typeContext *data_type();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *KW_WITH();
    Package_or_class_scoped_idContext *package_or_class_scoped_id();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Net_type_declarationContext* net_type_declaration();

  class  Let_declarationContext : public antlr4::ParserRuleContext {
  public:
    Let_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_LET();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *ASSIGN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    Let_port_listContext *let_port_list();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Let_declarationContext* let_declaration();

  class  Let_port_listContext : public antlr4::ParserRuleContext {
  public:
    Let_port_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Let_port_itemContext *> let_port_item();
    Let_port_itemContext* let_port_item(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Let_port_listContext* let_port_list();

  class  Let_port_itemContext : public antlr4::ParserRuleContext {
  public:
    Let_port_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    Let_formal_typeContext *let_formal_type();
    std::vector<Variable_dimensionContext *> variable_dimension();
    Variable_dimensionContext* variable_dimension(size_t i);
    antlr4::tree::TerminalNode *ASSIGN();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Let_port_itemContext* let_port_item();

  class  Let_formal_typeContext : public antlr4::ParserRuleContext {
  public:
    Let_formal_typeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_UNTYPED();
    Data_type_or_implicitContext *data_type_or_implicit();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Let_formal_typeContext* let_formal_type();

  class  Package_import_declarationContext : public antlr4::ParserRuleContext {
  public:
    Package_import_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_IMPORT();
    std::vector<Package_import_itemContext *> package_import_item();
    Package_import_itemContext* package_import_item(size_t i);
    antlr4::tree::TerminalNode *SEMI();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Package_import_declarationContext* package_import_declaration();

  class  Package_import_itemContext : public antlr4::ParserRuleContext {
  public:
    Package_import_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *DOUBLE_COLON();
    antlr4::tree::TerminalNode *MUL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Package_import_itemContext* package_import_item();

  class  Property_list_of_argumentsContext : public antlr4::ParserRuleContext {
  public:
    Property_list_of_argumentsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> DOT();
    antlr4::tree::TerminalNode* DOT(size_t i);
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    std::vector<antlr4::tree::TerminalNode *> LPAREN();
    antlr4::tree::TerminalNode* LPAREN(size_t i);
    std::vector<antlr4::tree::TerminalNode *> RPAREN();
    antlr4::tree::TerminalNode* RPAREN(size_t i);
    std::vector<Property_actual_argContext *> property_actual_arg();
    Property_actual_argContext* property_actual_arg(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Property_list_of_argumentsContext* property_list_of_arguments();

  class  Property_actual_argContext : public antlr4::ParserRuleContext {
  public:
    Property_actual_argContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Property_exprContext *property_expr();
    Sequence_actual_argContext *sequence_actual_arg();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Property_actual_argContext* property_actual_arg();

  class  Property_formal_typeContext : public antlr4::ParserRuleContext {
  public:
    Property_formal_typeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_PROPERTY();
    Sequence_formal_typeContext *sequence_formal_type();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Property_formal_typeContext* property_formal_type();

  class  Sequence_formal_typeContext : public antlr4::ParserRuleContext {
  public:
    Sequence_formal_typeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_SEQUENCE();
    antlr4::tree::TerminalNode *KW_UNTYPED();
    Data_type_or_implicitContext *data_type_or_implicit();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sequence_formal_typeContext* sequence_formal_type();

  class  Property_instanceContext : public antlr4::ParserRuleContext {
  public:
    Property_instanceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Package_or_class_scoped_idContext *package_or_class_scoped_id();
    antlr4::tree::TerminalNode *LPAREN();
    Property_list_of_argumentsContext *property_list_of_arguments();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Property_instanceContext* property_instance();

  class  Property_specContext : public antlr4::ParserRuleContext {
  public:
    Property_specContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Property_exprContext *property_expr();
    Clocking_eventContext *clocking_event();
    antlr4::tree::TerminalNode *KW_DISABLE();
    antlr4::tree::TerminalNode *KW_IFF();
    antlr4::tree::TerminalNode *LPAREN();
    Expression_or_distContext *expression_or_dist();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Property_specContext* property_spec();

  class  Property_exprContext : public antlr4::ParserRuleContext {
  public:
    Property_exprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Property_exprContext *> property_expr();
    Property_exprContext* property_expr(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *KW_STRONG();
    antlr4::tree::TerminalNode *KW_WEAK();
    antlr4::tree::TerminalNode *KW_IF();
    Expression_or_distContext *expression_or_dist();
    antlr4::tree::TerminalNode *KW_ELSE();
    antlr4::tree::TerminalNode *KW_CASE();
    antlr4::tree::TerminalNode *KW_ENDCASE();
    std::vector<Property_case_itemContext *> property_case_item();
    Property_case_itemContext* property_case_item(size_t i);
    Sequence_exprContext *sequence_expr();
    antlr4::tree::TerminalNode *OVERLAPPING_IMPL();
    antlr4::tree::TerminalNode *NONOVERLAPPING_IMPL();
    antlr4::tree::TerminalNode *HASH_MINUS_HASH();
    antlr4::tree::TerminalNode *HASH_EQ_HASH();
    antlr4::tree::TerminalNode *KW_NOT();
    antlr4::tree::TerminalNode *LSQUARE_BR();
    Range_expressionContext *range_expression();
    antlr4::tree::TerminalNode *RSQUARE_BR();
    Clocking_eventContext *clocking_event();
    antlr4::tree::TerminalNode *KW_S_ALWAYS();
    antlr4::tree::TerminalNode *KW_EVENTUALLY();
    antlr4::tree::TerminalNode *KW_ACCEPT_ON();
    antlr4::tree::TerminalNode *KW_REJECT_ON();
    antlr4::tree::TerminalNode *KW_SYNC_ACCEPT_ON();
    antlr4::tree::TerminalNode *KW_SYNC_REJECT_ON();
    antlr4::tree::TerminalNode *KW_NEXTTIME();
    antlr4::tree::TerminalNode *KW_S_NEXTTIME();
    antlr4::tree::TerminalNode *KW_ALWAYS();
    antlr4::tree::TerminalNode *KW_S_EVENTUALLY();
    ExpressionContext *expression();
    Cycle_delay_const_range_expressionContext *cycle_delay_const_range_expression();
    Property_instanceContext *property_instance();
    antlr4::tree::TerminalNode *KW_OR();
    antlr4::tree::TerminalNode *KW_AND();
    antlr4::tree::TerminalNode *KW_UNTIL();
    antlr4::tree::TerminalNode *KW_S_UNTIL();
    antlr4::tree::TerminalNode *KW_UNTIL_WITH();
    antlr4::tree::TerminalNode *KW_S_UNTIL_WITH();
    antlr4::tree::TerminalNode *KW_IMPLIES();
    antlr4::tree::TerminalNode *KW_IFF();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Property_exprContext* property_expr();
  Property_exprContext* property_expr(int precedence);
  class  Property_case_itemContext : public antlr4::ParserRuleContext {
  public:
    Property_case_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Property_exprContext *property_expr();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_DEFAULT();
    std::vector<Expression_or_distContext *> expression_or_dist();
    Expression_or_distContext* expression_or_dist(size_t i);
    antlr4::tree::TerminalNode *COLON();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Property_case_itemContext* property_case_item();

  class  Bit_selectContext : public antlr4::ParserRuleContext {
  public:
    Bit_selectContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LSQUARE_BR();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RSQUARE_BR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Bit_selectContext* bit_select();

  class  Identifier_with_bit_selectContext : public antlr4::ParserRuleContext {
  public:
    Identifier_with_bit_selectContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    std::vector<Bit_selectContext *> bit_select();
    Bit_selectContext* bit_select(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Identifier_with_bit_selectContext* identifier_with_bit_select();

  class  Package_or_class_scoped_hier_id_with_selectContext : public antlr4::ParserRuleContext {
  public:
    Package_or_class_scoped_hier_id_with_selectContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Package_or_class_scoped_pathContext *package_or_class_scoped_path();
    std::vector<Bit_selectContext *> bit_select();
    Bit_selectContext* bit_select(size_t i);
    std::vector<antlr4::tree::TerminalNode *> DOT();
    antlr4::tree::TerminalNode* DOT(size_t i);
    std::vector<Identifier_with_bit_selectContext *> identifier_with_bit_select();
    Identifier_with_bit_selectContext* identifier_with_bit_select(size_t i);
    antlr4::tree::TerminalNode *LSQUARE_BR();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *RSQUARE_BR();
    Operator_plus_minusContext *operator_plus_minus();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Package_or_class_scoped_hier_id_with_selectContext* package_or_class_scoped_hier_id_with_select();

  class  Package_or_class_scoped_path_itemContext : public antlr4::ParserRuleContext {
  public:
    Package_or_class_scoped_path_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    Parameter_value_assignmentContext *parameter_value_assignment();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Package_or_class_scoped_path_itemContext* package_or_class_scoped_path_item();

  class  Package_or_class_scoped_pathContext : public antlr4::ParserRuleContext {
  public:
    Package_or_class_scoped_pathContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_DOLAR_ROOT();
    Implicit_class_handleContext *implicit_class_handle();
    antlr4::tree::TerminalNode *KW_DOLAR_UNIT();
    std::vector<Package_or_class_scoped_path_itemContext *> package_or_class_scoped_path_item();
    Package_or_class_scoped_path_itemContext* package_or_class_scoped_path_item(size_t i);
    antlr4::tree::TerminalNode *KW_LOCAL();
    std::vector<antlr4::tree::TerminalNode *> DOUBLE_COLON();
    antlr4::tree::TerminalNode* DOUBLE_COLON(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Package_or_class_scoped_pathContext* package_or_class_scoped_path();

  class  Hierarchical_identifierContext : public antlr4::ParserRuleContext {
  public:
    Hierarchical_identifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *KW_DOLAR_ROOT();
    std::vector<antlr4::tree::TerminalNode *> DOT();
    antlr4::tree::TerminalNode* DOT(size_t i);
    std::vector<Identifier_with_bit_selectContext *> identifier_with_bit_select();
    Identifier_with_bit_selectContext* identifier_with_bit_select(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Hierarchical_identifierContext* hierarchical_identifier();

  class  Package_or_class_scoped_idContext : public antlr4::ParserRuleContext {
  public:
    Package_or_class_scoped_idContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_DOLAR_UNIT();
    std::vector<Package_or_class_scoped_path_itemContext *> package_or_class_scoped_path_item();
    Package_or_class_scoped_path_itemContext* package_or_class_scoped_path_item(size_t i);
    std::vector<antlr4::tree::TerminalNode *> DOUBLE_COLON();
    antlr4::tree::TerminalNode* DOUBLE_COLON(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Package_or_class_scoped_idContext* package_or_class_scoped_id();

  class  SelectContext : public antlr4::ParserRuleContext {
  public:
    SelectContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> DOT();
    antlr4::tree::TerminalNode* DOT(size_t i);
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    std::vector<Bit_selectContext *> bit_select();
    Bit_selectContext* bit_select(size_t i);
    antlr4::tree::TerminalNode *LSQUARE_BR();
    Array_range_expressionContext *array_range_expression();
    antlr4::tree::TerminalNode *RSQUARE_BR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SelectContext* select();

  class  Event_expression_itemContext : public antlr4::ParserRuleContext {
  public:
    Event_expression_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    Event_expressionContext *event_expression();
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    Edge_identifierContext *edge_identifier();
    antlr4::tree::TerminalNode *KW_IFF();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Event_expression_itemContext* event_expression_item();

  class  Event_expressionContext : public antlr4::ParserRuleContext {
  public:
    Event_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Event_expression_itemContext *> event_expression_item();
    Event_expression_itemContext* event_expression_item(size_t i);
    std::vector<antlr4::tree::TerminalNode *> KW_OR();
    antlr4::tree::TerminalNode* KW_OR(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Event_expressionContext* event_expression();

  class  Boolean_abbrevContext : public antlr4::ParserRuleContext {
  public:
    Boolean_abbrevContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Consecutive_repetitionContext *consecutive_repetition();
    Non_consecutive_repetitionContext *non_consecutive_repetition();
    Goto_repetitionContext *goto_repetition();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Boolean_abbrevContext* boolean_abbrev();

  class  Sequence_abbrevContext : public antlr4::ParserRuleContext {
  public:
    Sequence_abbrevContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Consecutive_repetitionContext *consecutive_repetition();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sequence_abbrevContext* sequence_abbrev();

  class  Consecutive_repetitionContext : public antlr4::ParserRuleContext {
  public:
    Consecutive_repetitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LSQUARE_BR();
    antlr4::tree::TerminalNode *RSQUARE_BR();
    antlr4::tree::TerminalNode *MUL();
    antlr4::tree::TerminalNode *PLUS();
    Const_or_range_expressionContext *const_or_range_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Consecutive_repetitionContext* consecutive_repetition();

  class  Non_consecutive_repetitionContext : public antlr4::ParserRuleContext {
  public:
    Non_consecutive_repetitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LSQUARE_BR();
    antlr4::tree::TerminalNode *ASSIGN();
    Const_or_range_expressionContext *const_or_range_expression();
    antlr4::tree::TerminalNode *RSQUARE_BR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Non_consecutive_repetitionContext* non_consecutive_repetition();

  class  Goto_repetitionContext : public antlr4::ParserRuleContext {
  public:
    Goto_repetitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LSQUARE_BR();
    antlr4::tree::TerminalNode *ARROW();
    Const_or_range_expressionContext *const_or_range_expression();
    antlr4::tree::TerminalNode *RSQUARE_BR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Goto_repetitionContext* goto_repetition();

  class  Cycle_delay_const_range_expressionContext : public antlr4::ParserRuleContext {
  public:
    Cycle_delay_const_range_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *DOLAR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Cycle_delay_const_range_expressionContext* cycle_delay_const_range_expression();

  class  Sequence_instanceContext : public antlr4::ParserRuleContext {
  public:
    Sequence_instanceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Package_or_class_scoped_pathContext *package_or_class_scoped_path();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    Sequence_list_of_argumentsContext *sequence_list_of_arguments();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sequence_instanceContext* sequence_instance();

  class  Sequence_exprContext : public antlr4::ParserRuleContext {
  public:
    Sequence_exprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_FIRST_MATCH();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Sequence_exprContext *> sequence_expr();
    Sequence_exprContext* sequence_expr(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<Sequence_match_itemContext *> sequence_match_item();
    Sequence_match_itemContext* sequence_match_item(size_t i);
    std::vector<Cycle_delay_rangeContext *> cycle_delay_range();
    Cycle_delay_rangeContext* cycle_delay_range(size_t i);
    Expression_or_distContext *expression_or_dist();
    antlr4::tree::TerminalNode *KW_THROUGHOUT();
    Boolean_abbrevContext *boolean_abbrev();
    Sequence_instanceContext *sequence_instance();
    Sequence_abbrevContext *sequence_abbrev();
    Clocking_eventContext *clocking_event();
    antlr4::tree::TerminalNode *KW_AND();
    antlr4::tree::TerminalNode *KW_INTERSECT();
    antlr4::tree::TerminalNode *KW_OR();
    antlr4::tree::TerminalNode *KW_WITHIN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sequence_exprContext* sequence_expr();
  Sequence_exprContext* sequence_expr(int precedence);
  class  Sequence_match_itemContext : public antlr4::ParserRuleContext {
  public:
    Sequence_match_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Operator_assignmentContext *operator_assignment();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sequence_match_itemContext* sequence_match_item();

  class  Operator_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Operator_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Variable_lvalueContext *variable_lvalue();
    Assignment_operatorContext *assignment_operator();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Operator_assignmentContext* operator_assignment();

  class  Sequence_actual_argContext : public antlr4::ParserRuleContext {
  public:
    Sequence_actual_argContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Event_expressionContext *event_expression();
    Sequence_exprContext *sequence_expr();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sequence_actual_argContext* sequence_actual_arg();

  class  Dist_weightContext : public antlr4::ParserRuleContext {
  public:
    Dist_weightContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *DIST_WEIGHT_ASSIGN();
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *DIV();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Dist_weightContext* dist_weight();

  class  Clocking_declarationContext : public antlr4::ParserRuleContext {
  public:
    Clocking_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_ENDCLOCKING();
    antlr4::tree::TerminalNode *KW_GLOBAL();
    antlr4::tree::TerminalNode *KW_CLOCKING();
    Clocking_eventContext *clocking_event();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *COLON();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *KW_DEFAULT();
    std::vector<Clocking_itemContext *> clocking_item();
    Clocking_itemContext* clocking_item(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Clocking_declarationContext* clocking_declaration();

  class  Clocking_itemContext : public antlr4::ParserRuleContext {
  public:
    Clocking_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_DEFAULT();
    Default_skewContext *default_skew();
    Clocking_directionContext *clocking_direction();
    List_of_clocking_decl_assignContext *list_of_clocking_decl_assign();
    Property_declarationContext *property_declaration();
    Sequence_declarationContext *sequence_declaration();
    Let_declarationContext *let_declaration();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Clocking_itemContext* clocking_item();

  class  List_of_clocking_decl_assignContext : public antlr4::ParserRuleContext {
  public:
    List_of_clocking_decl_assignContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Clocking_decl_assignContext *> clocking_decl_assign();
    Clocking_decl_assignContext* clocking_decl_assign(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  List_of_clocking_decl_assignContext* list_of_clocking_decl_assign();

  class  Clocking_decl_assignContext : public antlr4::ParserRuleContext {
  public:
    Clocking_decl_assignContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Attr_specContext *attr_spec();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Clocking_decl_assignContext* clocking_decl_assign();

  class  Default_skewContext : public antlr4::ParserRuleContext {
  public:
    Default_skewContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_INPUT();
    std::vector<Clocking_skewContext *> clocking_skew();
    Clocking_skewContext* clocking_skew(size_t i);
    antlr4::tree::TerminalNode *KW_OUTPUT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Default_skewContext* default_skew();

  class  Clocking_directionContext : public antlr4::ParserRuleContext {
  public:
    Clocking_directionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_INPUT();
    std::vector<Clocking_skewContext *> clocking_skew();
    Clocking_skewContext* clocking_skew(size_t i);
    antlr4::tree::TerminalNode *KW_OUTPUT();
    antlr4::tree::TerminalNode *KW_INOUT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Clocking_directionContext* clocking_direction();

  class  Clocking_skewContext : public antlr4::ParserRuleContext {
  public:
    Clocking_skewContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Edge_identifierContext *edge_identifier();
    Delay_controlContext *delay_control();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Clocking_skewContext* clocking_skew();

  class  Clocking_eventContext : public antlr4::ParserRuleContext {
  public:
    Clocking_eventContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *AT();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *LPAREN();
    Event_expressionContext *event_expression();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Clocking_eventContext* clocking_event();

  class  Cycle_delay_rangeContext : public antlr4::ParserRuleContext {
  public:
    Cycle_delay_rangeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DOUBLE_HASH();
    antlr4::tree::TerminalNode *LSQUARE_BR();
    antlr4::tree::TerminalNode *RSQUARE_BR();
    PrimaryContext *primary();
    antlr4::tree::TerminalNode *MUL();
    antlr4::tree::TerminalNode *PLUS();
    Cycle_delay_const_range_expressionContext *cycle_delay_const_range_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Cycle_delay_rangeContext* cycle_delay_range();

  class  Expression_or_distContext : public antlr4::ParserRuleContext {
  public:
    Expression_or_distContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *KW_DIST();
    antlr4::tree::TerminalNode *LBRACE();
    std::vector<Dist_itemContext *> dist_item();
    Dist_itemContext* dist_item(size_t i);
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Expression_or_distContext* expression_or_dist();

  class  Covergroup_declarationContext : public antlr4::ParserRuleContext {
  public:
    Covergroup_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_COVERGROUP();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_ENDGROUP();
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *LPAREN();
    Tf_port_listContext *tf_port_list();
    antlr4::tree::TerminalNode *RPAREN();
    Coverage_eventContext *coverage_event();
    std::vector<Coverage_spec_or_optionContext *> coverage_spec_or_option();
    Coverage_spec_or_optionContext* coverage_spec_or_option(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Covergroup_declarationContext* covergroup_declaration();

  class  Cover_crossContext : public antlr4::ParserRuleContext {
  public:
    Cover_crossContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_CROSS();
    Identifier_list_2plusContext *identifier_list_2plus();
    Cross_bodyContext *cross_body();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *KW_IFF();
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Cover_crossContext* cover_cross();

  class  Identifier_list_2plusContext : public antlr4::ParserRuleContext {
  public:
    Identifier_list_2plusContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Identifier_list_2plusContext* identifier_list_2plus();

  class  Cross_bodyContext : public antlr4::ParserRuleContext {
  public:
    Cross_bodyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<Cross_body_itemContext *> cross_body_item();
    Cross_body_itemContext* cross_body_item(size_t i);
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Cross_bodyContext* cross_body();

  class  Cross_body_itemContext : public antlr4::ParserRuleContext {
  public:
    Cross_body_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Function_declarationContext *function_declaration();
    Bins_selection_or_optionContext *bins_selection_or_option();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Cross_body_itemContext* cross_body_item();

  class  Bins_selection_or_optionContext : public antlr4::ParserRuleContext {
  public:
    Bins_selection_or_optionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Coverage_optionContext *coverage_option();
    Bins_selectionContext *bins_selection();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Bins_selection_or_optionContext* bins_selection_or_option();

  class  Bins_selectionContext : public antlr4::ParserRuleContext {
  public:
    Bins_selectionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Bins_keywordContext *bins_keyword();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *ASSIGN();
    Select_expressionContext *select_expression();
    antlr4::tree::TerminalNode *KW_IFF();
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Bins_selectionContext* bins_selection();

  class  Select_expressionContext : public antlr4::ParserRuleContext {
  public:
    Select_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Select_expressionContext *> select_expression();
    Select_expressionContext* select_expression(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    Select_conditionContext *select_condition();
    antlr4::tree::TerminalNode *NOT();
    std::vector<Covergroup_expressionContext *> covergroup_expression();
    Covergroup_expressionContext* covergroup_expression(size_t i);
    antlr4::tree::TerminalNode *KW_MATCHES();
    antlr4::tree::TerminalNode *AND_LOG();
    antlr4::tree::TerminalNode *OR_LOG();
    antlr4::tree::TerminalNode *KW_WITH();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Select_expressionContext* select_expression();
  Select_expressionContext* select_expression(int precedence);
  class  Select_conditionContext : public antlr4::ParserRuleContext {
  public:
    Select_conditionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_BINSOF();
    antlr4::tree::TerminalNode *LPAREN();
    Bins_expressionContext *bins_expression();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *KW_INTERSECT();
    antlr4::tree::TerminalNode *LBRACE();
    Covergroup_range_listContext *covergroup_range_list();
    antlr4::tree::TerminalNode *RBRACE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Select_conditionContext* select_condition();

  class  Bins_expressionContext : public antlr4::ParserRuleContext {
  public:
    Bins_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *DOT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Bins_expressionContext* bins_expression();

  class  Covergroup_range_listContext : public antlr4::ParserRuleContext {
  public:
    Covergroup_range_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Covergroup_value_rangeContext *> covergroup_value_range();
    Covergroup_value_rangeContext* covergroup_value_range(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Covergroup_range_listContext* covergroup_range_list();

  class  Covergroup_value_rangeContext : public antlr4::ParserRuleContext {
  public:
    Covergroup_value_rangeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LSQUARE_BR();
    std::vector<Covergroup_expressionContext *> covergroup_expression();
    Covergroup_expressionContext* covergroup_expression(size_t i);
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *RSQUARE_BR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Covergroup_value_rangeContext* covergroup_value_range();

  class  Covergroup_expressionContext : public antlr4::ParserRuleContext {
  public:
    Covergroup_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Covergroup_expressionContext* covergroup_expression();

  class  Coverage_spec_or_optionContext : public antlr4::ParserRuleContext {
  public:
    Coverage_spec_or_optionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Coverage_specContext *coverage_spec();
    Coverage_optionContext *coverage_option();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Coverage_spec_or_optionContext* coverage_spec_or_option();

  class  Coverage_optionContext : public antlr4::ParserRuleContext {
  public:
    Coverage_optionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_OPTION();
    antlr4::tree::TerminalNode *DOT();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *ASSIGN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *KW_TYPE_OPTION();
    Constant_expressionContext *constant_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Coverage_optionContext* coverage_option();

  class  Coverage_specContext : public antlr4::ParserRuleContext {
  public:
    Coverage_specContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Cover_pointContext *cover_point();
    Cover_crossContext *cover_cross();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Coverage_specContext* coverage_spec();

  class  Cover_pointContext : public antlr4::ParserRuleContext {
  public:
    Cover_pointContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_COVERPOINT();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    Bins_or_emptyContext *bins_or_empty();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *KW_IFF();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    Data_type_or_implicitContext *data_type_or_implicit();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Cover_pointContext* cover_point();

  class  Bins_or_emptyContext : public antlr4::ParserRuleContext {
  public:
    Bins_or_emptyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    std::vector<Bins_or_optionsContext *> bins_or_options();
    Bins_or_optionsContext* bins_or_options(size_t i);
    std::vector<antlr4::tree::TerminalNode *> SEMI();
    antlr4::tree::TerminalNode* SEMI(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Bins_or_emptyContext* bins_or_empty();

  class  Bins_or_optionsContext : public antlr4::ParserRuleContext {
  public:
    Bins_or_optionsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Coverage_optionContext *coverage_option();
    Bins_keywordContext *bins_keyword();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *KW_IFF();
    std::vector<antlr4::tree::TerminalNode *> LPAREN();
    antlr4::tree::TerminalNode* LPAREN(size_t i);
    ExpressionContext *expression();
    std::vector<antlr4::tree::TerminalNode *> RPAREN();
    antlr4::tree::TerminalNode* RPAREN(size_t i);
    antlr4::tree::TerminalNode *ASSIGN();
    Trans_listContext *trans_list();
    antlr4::tree::TerminalNode *KW_DEFAULT();
    antlr4::tree::TerminalNode *KW_SEQUENCE();
    antlr4::tree::TerminalNode *KW_WILDCARD();
    antlr4::tree::TerminalNode *LBRACE();
    Covergroup_range_listContext *covergroup_range_list();
    antlr4::tree::TerminalNode *RBRACE();
    antlr4::tree::TerminalNode *KW_WITH();
    std::vector<Covergroup_expressionContext *> covergroup_expression();
    Covergroup_expressionContext* covergroup_expression(size_t i);
    antlr4::tree::TerminalNode *LSQUARE_BR();
    antlr4::tree::TerminalNode *RSQUARE_BR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Bins_or_optionsContext* bins_or_options();

  class  Trans_listContext : public antlr4::ParserRuleContext {
  public:
    Trans_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> LPAREN();
    antlr4::tree::TerminalNode* LPAREN(size_t i);
    std::vector<Trans_setContext *> trans_set();
    Trans_setContext* trans_set(size_t i);
    std::vector<antlr4::tree::TerminalNode *> RPAREN();
    antlr4::tree::TerminalNode* RPAREN(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Trans_listContext* trans_list();

  class  Trans_setContext : public antlr4::ParserRuleContext {
  public:
    Trans_setContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Trans_range_listContext *> trans_range_list();
    Trans_range_listContext* trans_range_list(size_t i);
    std::vector<antlr4::tree::TerminalNode *> IMPLIES();
    antlr4::tree::TerminalNode* IMPLIES(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Trans_setContext* trans_set();

  class  Trans_range_listContext : public antlr4::ParserRuleContext {
  public:
    Trans_range_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Covergroup_range_listContext *covergroup_range_list();
    antlr4::tree::TerminalNode *LSQUARE_BR();
    Repeat_rangeContext *repeat_range();
    antlr4::tree::TerminalNode *RSQUARE_BR();
    antlr4::tree::TerminalNode *MUL();
    antlr4::tree::TerminalNode *ARROW();
    antlr4::tree::TerminalNode *ASSIGN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Trans_range_listContext* trans_range_list();

  class  Repeat_rangeContext : public antlr4::ParserRuleContext {
  public:
    Repeat_rangeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Covergroup_expressionContext *> covergroup_expression();
    Covergroup_expressionContext* covergroup_expression(size_t i);
    antlr4::tree::TerminalNode *COLON();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Repeat_rangeContext* repeat_range();

  class  Coverage_eventContext : public antlr4::ParserRuleContext {
  public:
    Coverage_eventContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *KW_WITH();
    antlr4::tree::TerminalNode *KW_FUNCTION();
    antlr4::tree::TerminalNode *KW_SAMPLE();
    antlr4::tree::TerminalNode *LPAREN();
    Tf_port_listContext *tf_port_list();
    antlr4::tree::TerminalNode *DOUBLE_AT();
    Block_event_expressionContext *block_event_expression();
    Clocking_eventContext *clocking_event();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Coverage_eventContext* coverage_event();

  class  Block_event_expressionContext : public antlr4::ParserRuleContext {
  public:
    Block_event_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Hierarchical_btf_identifierContext *hierarchical_btf_identifier();
    antlr4::tree::TerminalNode *KW_BEGIN();
    antlr4::tree::TerminalNode *KW_END();
    std::vector<Block_event_expressionContext *> block_event_expression();
    Block_event_expressionContext* block_event_expression(size_t i);
    antlr4::tree::TerminalNode *KW_OR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Block_event_expressionContext* block_event_expression();
  Block_event_expressionContext* block_event_expression(int precedence);
  class  Hierarchical_btf_identifierContext : public antlr4::ParserRuleContext {
  public:
    Hierarchical_btf_identifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Hierarchical_identifierContext *hierarchical_identifier();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *DOT();
    Class_scopeContext *class_scope();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Hierarchical_btf_identifierContext* hierarchical_btf_identifier();

  class  Assertion_variable_declarationContext : public antlr4::ParserRuleContext {
  public:
    Assertion_variable_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    List_of_variable_decl_assignmentsContext *list_of_variable_decl_assignments();
    antlr4::tree::TerminalNode *SEMI();
    Var_data_typeContext *var_data_type();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Assertion_variable_declarationContext* assertion_variable_declaration();

  class  Dist_itemContext : public antlr4::ParserRuleContext {
  public:
    Dist_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Value_rangeContext *value_range();
    Dist_weightContext *dist_weight();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Dist_itemContext* dist_item();

  class  Value_rangeContext : public antlr4::ParserRuleContext {
  public:
    Value_rangeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LSQUARE_BR();
    Range_expressionContext *range_expression();
    antlr4::tree::TerminalNode *RSQUARE_BR();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Value_rangeContext* value_range();

  class  Attribute_instanceContext : public antlr4::ParserRuleContext {
  public:
    Attribute_instanceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<antlr4::tree::TerminalNode *> MUL();
    antlr4::tree::TerminalNode* MUL(size_t i);
    std::vector<Attr_specContext *> attr_spec();
    Attr_specContext* attr_spec(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Attribute_instanceContext* attribute_instance();

  class  Attr_specContext : public antlr4::ParserRuleContext {
  public:
    Attr_specContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *ASSIGN();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Attr_specContext* attr_spec();

  class  Class_newContext : public antlr4::ParserRuleContext {
  public:
    Class_newContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_NEW();
    ExpressionContext *expression();
    Class_scopeContext *class_scope();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    List_of_argumentsContext *list_of_arguments();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Class_newContext* class_new();

  class  Param_expressionContext : public antlr4::ParserRuleContext {
  public:
    Param_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Mintypmax_expressionContext *mintypmax_expression();
    Data_typeContext *data_type();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Param_expressionContext* param_expression();

  class  Constant_param_expressionContext : public antlr4::ParserRuleContext {
  public:
    Constant_param_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ReplicationContext *replication();
    ConcatenationContext *concatenation();
    Param_expressionContext *param_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Constant_param_expressionContext* constant_param_expression();

  class  Unpacked_dimensionContext : public antlr4::ParserRuleContext {
  public:
    Unpacked_dimensionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LSQUARE_BR();
    Range_expressionContext *range_expression();
    antlr4::tree::TerminalNode *RSQUARE_BR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Unpacked_dimensionContext* unpacked_dimension();

  class  Packed_dimensionContext : public antlr4::ParserRuleContext {
  public:
    Packed_dimensionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LSQUARE_BR();
    antlr4::tree::TerminalNode *RSQUARE_BR();
    Range_expressionContext *range_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Packed_dimensionContext* packed_dimension();

  class  Variable_dimensionContext : public antlr4::ParserRuleContext {
  public:
    Variable_dimensionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LSQUARE_BR();
    antlr4::tree::TerminalNode *RSQUARE_BR();
    antlr4::tree::TerminalNode *MUL();
    Data_typeContext *data_type();
    Array_range_expressionContext *array_range_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Variable_dimensionContext* variable_dimension();

  class  Struct_unionContext : public antlr4::ParserRuleContext {
  public:
    Struct_unionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_STRUCT();
    antlr4::tree::TerminalNode *KW_UNION();
    antlr4::tree::TerminalNode *KW_TAGGED();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Struct_unionContext* struct_union();

  class  Enum_base_typeContext : public antlr4::ParserRuleContext {
  public:
    Enum_base_typeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Integer_atom_typeContext *integer_atom_type();
    SigningContext *signing();
    Integer_vector_typeContext *integer_vector_type();
    Package_or_class_scoped_idContext *package_or_class_scoped_id();
    Variable_dimensionContext *variable_dimension();
    Packed_dimensionContext *packed_dimension();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Enum_base_typeContext* enum_base_type();

  class  Data_type_primitiveContext : public antlr4::ParserRuleContext {
  public:
    Data_type_primitiveContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Integer_typeContext *integer_type();
    SigningContext *signing();
    Non_integer_typeContext *non_integer_type();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Data_type_primitiveContext* data_type_primitive();

  class  Data_typeContext : public antlr4::ParserRuleContext {
  public:
    Data_typeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_STRING();
    antlr4::tree::TerminalNode *KW_CHANDLE();
    antlr4::tree::TerminalNode *KW_VIRTUAL();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *KW_INTERFACE();
    Parameter_value_assignmentContext *parameter_value_assignment();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *KW_EVENT();
    Data_type_primitiveContext *data_type_primitive();
    antlr4::tree::TerminalNode *KW_ENUM();
    antlr4::tree::TerminalNode *LBRACE();
    std::vector<Enum_name_declarationContext *> enum_name_declaration();
    Enum_name_declarationContext* enum_name_declaration(size_t i);
    antlr4::tree::TerminalNode *RBRACE();
    Struct_unionContext *struct_union();
    Package_or_class_scoped_pathContext *package_or_class_scoped_path();
    std::vector<Variable_dimensionContext *> variable_dimension();
    Variable_dimensionContext* variable_dimension(size_t i);
    Enum_base_typeContext *enum_base_type();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    antlr4::tree::TerminalNode *KW_PACKED();
    std::vector<Struct_union_memberContext *> struct_union_member();
    Struct_union_memberContext* struct_union_member(size_t i);
    SigningContext *signing();
    Type_referenceContext *type_reference();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Data_typeContext* data_type();

  class  Data_type_or_implicitContext : public antlr4::ParserRuleContext {
  public:
    Data_type_or_implicitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Data_typeContext *data_type();
    Implicit_data_typeContext *implicit_data_type();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Data_type_or_implicitContext* data_type_or_implicit();

  class  Implicit_data_typeContext : public antlr4::ParserRuleContext {
  public:
    Implicit_data_typeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    SigningContext *signing();
    std::vector<Packed_dimensionContext *> packed_dimension();
    Packed_dimensionContext* packed_dimension(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Implicit_data_typeContext* implicit_data_type();

  class  Sequence_list_of_arguments_named_itemContext : public antlr4::ParserRuleContext {
  public:
    Sequence_list_of_arguments_named_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DOT();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    Sequence_actual_argContext *sequence_actual_arg();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sequence_list_of_arguments_named_itemContext* sequence_list_of_arguments_named_item();

  class  Sequence_list_of_argumentsContext : public antlr4::ParserRuleContext {
  public:
    Sequence_list_of_argumentsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Sequence_list_of_arguments_named_itemContext *> sequence_list_of_arguments_named_item();
    Sequence_list_of_arguments_named_itemContext* sequence_list_of_arguments_named_item(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<Sequence_actual_argContext *> sequence_actual_arg();
    Sequence_actual_argContext* sequence_actual_arg(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sequence_list_of_argumentsContext* sequence_list_of_arguments();

  class  List_of_arguments_named_itemContext : public antlr4::ParserRuleContext {
  public:
    List_of_arguments_named_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DOT();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  List_of_arguments_named_itemContext* list_of_arguments_named_item();

  class  List_of_argumentsContext : public antlr4::ParserRuleContext {
  public:
    List_of_argumentsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<List_of_arguments_named_itemContext *> list_of_arguments_named_item();
    List_of_arguments_named_itemContext* list_of_arguments_named_item(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  List_of_argumentsContext* list_of_arguments();

  class  Primary_literalContext : public antlr4::ParserRuleContext {
  public:
    Primary_literalContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *TIME_LITERAL();
    antlr4::tree::TerminalNode *UNBASED_UNSIZED_LITERAL();
    antlr4::tree::TerminalNode *STRING_LITERAL();
    NumberContext *number();
    antlr4::tree::TerminalNode *KW_NULL();
    antlr4::tree::TerminalNode *KW_THIS();
    antlr4::tree::TerminalNode *DOLAR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Primary_literalContext* primary_literal();

  class  Type_referenceContext : public antlr4::ParserRuleContext {
  public:
    Type_referenceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_TYPE();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    ExpressionContext *expression();
    Data_typeContext *data_type();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Type_referenceContext* type_reference();

  class  Package_scopeContext : public antlr4::ParserRuleContext {
  public:
    Package_scopeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DOUBLE_COLON();
    antlr4::tree::TerminalNode *KW_DOLAR_UNIT();
    IdentifierContext *identifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Package_scopeContext* package_scope();

  class  Ps_identifierContext : public antlr4::ParserRuleContext {
  public:
    Ps_identifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    Package_scopeContext *package_scope();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Ps_identifierContext* ps_identifier();

  class  List_of_parameter_value_assignmentsContext : public antlr4::ParserRuleContext {
  public:
    List_of_parameter_value_assignmentsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Param_expressionContext *> param_expression();
    Param_expressionContext* param_expression(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<Named_parameter_assignmentContext *> named_parameter_assignment();
    Named_parameter_assignmentContext* named_parameter_assignment(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  List_of_parameter_value_assignmentsContext* list_of_parameter_value_assignments();

  class  Parameter_value_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Parameter_value_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *HASH();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    List_of_parameter_value_assignmentsContext *list_of_parameter_value_assignments();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Parameter_value_assignmentContext* parameter_value_assignment();

  class  Class_typeContext : public antlr4::ParserRuleContext {
  public:
    Class_typeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Ps_identifierContext *ps_identifier();
    std::vector<Parameter_value_assignmentContext *> parameter_value_assignment();
    Parameter_value_assignmentContext* parameter_value_assignment(size_t i);
    std::vector<antlr4::tree::TerminalNode *> DOUBLE_COLON();
    antlr4::tree::TerminalNode* DOUBLE_COLON(size_t i);
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Class_typeContext* class_type();

  class  Class_scopeContext : public antlr4::ParserRuleContext {
  public:
    Class_scopeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Class_typeContext *class_type();
    antlr4::tree::TerminalNode *DOUBLE_COLON();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Class_scopeContext* class_scope();

  class  First_range_identifierContext : public antlr4::ParserRuleContext {
  public:
    First_range_identifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  First_range_identifierContext* first_range_identifier();

  class  Second_range_identifierContext : public antlr4::ParserRuleContext {
  public:
    Second_range_identifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Second_range_identifierContext* second_range_identifier();

  class  Range_expressionContext : public antlr4::ParserRuleContext {
  public:
    Range_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    First_range_identifierContext *first_range_identifier();
    antlr4::tree::TerminalNode *COLON();
    Second_range_identifierContext *second_range_identifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Range_expressionContext* range_expression();

  class  Constant_range_expressionContext : public antlr4::ParserRuleContext {
  public:
    Constant_range_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Range_expressionContext *range_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Constant_range_expressionContext* constant_range_expression();

  class  Constant_mintypmax_expressionContext : public antlr4::ParserRuleContext {
  public:
    Constant_mintypmax_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Mintypmax_expressionContext *mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Constant_mintypmax_expressionContext* constant_mintypmax_expression();

  class  Mintypmax_expressionContext : public antlr4::ParserRuleContext {
  public:
    Mintypmax_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COLON();
    antlr4::tree::TerminalNode* COLON(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Mintypmax_expressionContext* mintypmax_expression();

  class  Named_parameter_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Named_parameter_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DOT();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    Param_expressionContext *param_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Named_parameter_assignmentContext* named_parameter_assignment();

  class  PrimaryContext : public antlr4::ParserRuleContext {
  public:
    PrimaryContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    PrimaryContext() = default;
    void copyFrom(PrimaryContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  PrimaryLitContext : public PrimaryContext {
  public:
    PrimaryLitContext(PrimaryContext *ctx);

    Primary_literalContext *primary_literal();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PrimaryReplContext : public PrimaryContext {
  public:
    PrimaryReplContext(PrimaryContext *ctx);

    ReplicationContext *replication();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PrimaryRandomizeContext : public PrimaryContext {
  public:
    PrimaryRandomizeContext(PrimaryContext *ctx);

    Randomize_callContext *randomize_call();
    antlr4::tree::TerminalNode *KW_STD();
    antlr4::tree::TerminalNode *DOUBLE_COLON();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PrimaryAssigContext : public PrimaryContext {
  public:
    PrimaryAssigContext(PrimaryContext *ctx);

    Assignment_pattern_expressionContext *assignment_pattern_expression();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PrimaryBitSelectContext : public PrimaryContext {
  public:
    PrimaryBitSelectContext(PrimaryContext *ctx);

    PrimaryContext *primary();
    Bit_selectContext *bit_select();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PrimaryTfCallContext : public PrimaryContext {
  public:
    PrimaryTfCallContext(PrimaryContext *ctx);

    Any_system_tf_identifierContext *any_system_tf_identifier();
    antlr4::tree::TerminalNode *LPAREN();
    Data_typeContext *data_type();
    antlr4::tree::TerminalNode *RPAREN();
    List_of_argumentsContext *list_of_arguments();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    Clocking_eventContext *clocking_event();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PrimaryTypeRefContext : public PrimaryContext {
  public:
    PrimaryTypeRefContext(PrimaryContext *ctx);

    Type_referenceContext *type_reference();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PrimaryCallArrayMethodNoArgsContext : public PrimaryContext {
  public:
    PrimaryCallArrayMethodNoArgsContext(PrimaryContext *ctx);

    PrimaryContext *primary();
    antlr4::tree::TerminalNode *DOT();
    Array_method_nameContext *array_method_name();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PrimaryCastContext : public PrimaryContext {
  public:
    PrimaryCastContext(PrimaryContext *ctx);

    antlr4::tree::TerminalNode *APOSTROPHE();
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *KW_STRING();
    antlr4::tree::TerminalNode *KW_CONST();
    Integer_typeContext *integer_type();
    Non_integer_typeContext *non_integer_type();
    SigningContext *signing();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PrimaryParContext : public PrimaryContext {
  public:
    PrimaryParContext(PrimaryContext *ctx);

    antlr4::tree::TerminalNode *LPAREN();
    Mintypmax_expressionContext *mintypmax_expression();
    antlr4::tree::TerminalNode *RPAREN();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PrimaryCallContext : public PrimaryContext {
  public:
    PrimaryCallContext(PrimaryContext *ctx);

    PrimaryContext *primary();
    std::vector<antlr4::tree::TerminalNode *> LPAREN();
    antlr4::tree::TerminalNode* LPAREN(size_t i);
    std::vector<antlr4::tree::TerminalNode *> RPAREN();
    antlr4::tree::TerminalNode* RPAREN(size_t i);
    antlr4::tree::TerminalNode *DOT();
    Array_method_nameContext *array_method_name();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    List_of_argumentsContext *list_of_arguments();
    antlr4::tree::TerminalNode *KW_WITH();
    ExpressionContext *expression();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PrimaryRandomize2Context : public PrimaryContext {
  public:
    PrimaryRandomize2Context(PrimaryContext *ctx);

    PrimaryContext *primary();
    antlr4::tree::TerminalNode *DOT();
    Randomize_callContext *randomize_call();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PrimaryDotContext : public PrimaryContext {
  public:
    PrimaryDotContext(PrimaryContext *ctx);

    PrimaryContext *primary();
    antlr4::tree::TerminalNode *DOT();
    IdentifierContext *identifier();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PrimaryStreaming_concatenationContext : public PrimaryContext {
  public:
    PrimaryStreaming_concatenationContext(PrimaryContext *ctx);

    Streaming_concatenationContext *streaming_concatenation();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PrimaryPathContext : public PrimaryContext {
  public:
    PrimaryPathContext(PrimaryContext *ctx);

    Package_or_class_scoped_pathContext *package_or_class_scoped_path();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PrimaryIndexContext : public PrimaryContext {
  public:
    PrimaryIndexContext(PrimaryContext *ctx);

    PrimaryContext *primary();
    antlr4::tree::TerminalNode *LSQUARE_BR();
    Array_range_expressionContext *array_range_expression();
    antlr4::tree::TerminalNode *RSQUARE_BR();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PrimaryCallWithContext : public PrimaryContext {
  public:
    PrimaryCallWithContext(PrimaryContext *ctx);

    PrimaryContext *primary();
    antlr4::tree::TerminalNode *KW_WITH();
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *DOT();
    Array_method_nameContext *array_method_name();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PrimaryConcatContext : public PrimaryContext {
  public:
    PrimaryConcatContext(PrimaryContext *ctx);

    ConcatenationContext *concatenation();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PrimaryCast2Context : public PrimaryContext {
  public:
    PrimaryCast2Context(PrimaryContext *ctx);

    PrimaryContext *primary();
    Cast_separatorContext *cast_separator();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  PrimaryContext* primary();
  PrimaryContext* primary(int precedence);
  class  Cast_separatorContext : public antlr4::ParserRuleContext {
  public:
    Cast_separatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *APOSTROPHE();
    antlr4::tree::TerminalNode *LPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Cast_separatorContext* cast_separator();

  class  Constant_expressionContext : public antlr4::ParserRuleContext {
  public:
    Constant_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Constant_expressionContext* constant_expression();

  class  Inc_or_dec_expressionContext : public antlr4::ParserRuleContext {
  public:
    Inc_or_dec_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    Inc_or_dec_expressionContext() = default;
    void copyFrom(Inc_or_dec_expressionContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  Inc_or_dec_expressionPreContext : public Inc_or_dec_expressionContext {
  public:
    Inc_or_dec_expressionPreContext(Inc_or_dec_expressionContext *ctx);

    Inc_or_dec_operatorContext *inc_or_dec_operator();
    Variable_lvalueContext *variable_lvalue();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  Inc_or_dec_expressionPostContext : public Inc_or_dec_expressionContext {
  public:
    Inc_or_dec_expressionPostContext(Inc_or_dec_expressionContext *ctx);

    Variable_lvalueContext *variable_lvalue();
    Inc_or_dec_operatorContext *inc_or_dec_operator();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  Inc_or_dec_expressionContext* inc_or_dec_expression();

  class  ExpressionContext : public antlr4::ParserRuleContext {
  public:
    ExpressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    PrimaryContext *primary();
    antlr4::tree::TerminalNode *LPAREN();
    Operator_assignmentContext *operator_assignment();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *KW_TAGGED();
    IdentifierContext *identifier();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    Unary_operatorContext *unary_operator();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    Inc_or_dec_expressionContext *inc_or_dec_expression();
    Operator_powerContext *operator_power();
    Operator_mul_div_modContext *operator_mul_div_mod();
    Operator_plus_minusContext *operator_plus_minus();
    Operator_shiftContext *operator_shift();
    Operator_cmpContext *operator_cmp();
    Operator_eq_neqContext *operator_eq_neq();
    Operator_bitwise_andContext *operator_bitwise_and();
    Operator_xorContext *operator_xor();
    Operator_bitwise_orContext *operator_bitwise_or();
    Operator_log_andContext *operator_log_and();
    Operator_log_orContext *operator_log_or();
    antlr4::tree::TerminalNode *QUESTIONMARK();
    antlr4::tree::TerminalNode *COLON();
    std::vector<antlr4::tree::TerminalNode *> KW_MATCHES();
    antlr4::tree::TerminalNode* KW_MATCHES(size_t i);
    std::vector<PatternContext *> pattern();
    PatternContext* pattern(size_t i);
    Operator_implContext *operator_impl();
    antlr4::tree::TerminalNode *KW_INSIDE();
    antlr4::tree::TerminalNode *LBRACE();
    Open_range_listContext *open_range_list();
    antlr4::tree::TerminalNode *RBRACE();
    antlr4::tree::TerminalNode *TRIPLE_AND();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExpressionContext* expression();
  ExpressionContext* expression(int precedence);
  class  Operator_log_andContext : public antlr4::ParserRuleContext {
  public:
    Operator_log_andContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *AND_LOG();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Operator_log_andContext* operator_log_and();

  class  Operator_log_orContext : public antlr4::ParserRuleContext {
  public:
    Operator_log_orContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OR_LOG();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Operator_log_orContext* operator_log_or();

  class  Operator_powerContext : public antlr4::ParserRuleContext {
  public:
    Operator_powerContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DOUBLESTAR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Operator_powerContext* operator_power();

  class  Operator_bitwise_andContext : public antlr4::ParserRuleContext {
  public:
    Operator_bitwise_andContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *AMPERSAND();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Operator_bitwise_andContext* operator_bitwise_and();

  class  Operator_bitwise_orContext : public antlr4::ParserRuleContext {
  public:
    Operator_bitwise_orContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *BAR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Operator_bitwise_orContext* operator_bitwise_or();

  class  ReplicationContext : public antlr4::ParserRuleContext {
  public:
    ReplicationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> LBRACE();
    antlr4::tree::TerminalNode* LBRACE(size_t i);
    Replication_sizeContext *replication_size();
    Replication_valueContext *replication_value();
    std::vector<antlr4::tree::TerminalNode *> RBRACE();
    antlr4::tree::TerminalNode* RBRACE(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ReplicationContext* replication();

  class  Replication_sizeContext : public antlr4::ParserRuleContext {
  public:
    Replication_sizeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Replication_sizeContext* replication_size();

  class  Replication_valueContext : public antlr4::ParserRuleContext {
  public:
    Replication_valueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Replication_valueContext* replication_value();

  class  ConcatenationContext : public antlr4::ParserRuleContext {
  public:
    ConcatenationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    ConcatenationContext *concatenation();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ConcatenationContext* concatenation();

  class  Dynamic_array_newContext : public antlr4::ParserRuleContext {
  public:
    Dynamic_array_newContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_NEW();
    antlr4::tree::TerminalNode *LSQUARE_BR();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    antlr4::tree::TerminalNode *RSQUARE_BR();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Dynamic_array_newContext* dynamic_array_new();

  class  Const_or_range_expressionContext : public antlr4::ParserRuleContext {
  public:
    Const_or_range_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *DOLAR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Const_or_range_expressionContext* const_or_range_expression();

  class  Variable_decl_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Variable_decl_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *ASSIGN();
    ExpressionContext *expression();
    Class_newContext *class_new();
    std::vector<Variable_dimensionContext *> variable_dimension();
    Variable_dimensionContext* variable_dimension(size_t i);
    Dynamic_array_newContext *dynamic_array_new();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Variable_decl_assignmentContext* variable_decl_assignment();

  class  Assignment_pattern_variable_lvalueContext : public antlr4::ParserRuleContext {
  public:
    Assignment_pattern_variable_lvalueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *APOSTROPHE_LBRACE();
    std::vector<Variable_lvalueContext *> variable_lvalue();
    Variable_lvalueContext* variable_lvalue(size_t i);
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Assignment_pattern_variable_lvalueContext* assignment_pattern_variable_lvalue();

  class  Stream_operatorContext : public antlr4::ParserRuleContext {
  public:
    Stream_operatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SHIFT_RIGHT();
    antlr4::tree::TerminalNode *SHIFT_LEFT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Stream_operatorContext* stream_operator();

  class  Slice_sizeContext : public antlr4::ParserRuleContext {
  public:
    Slice_sizeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Integer_typeContext *integer_type();
    Non_integer_typeContext *non_integer_type();
    Package_or_class_scoped_pathContext *package_or_class_scoped_path();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Slice_sizeContext* slice_size();

  class  Streaming_concatenationContext : public antlr4::ParserRuleContext {
  public:
    Streaming_concatenationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACE();
    Stream_operatorContext *stream_operator();
    Stream_concatenationContext *stream_concatenation();
    antlr4::tree::TerminalNode *RBRACE();
    Slice_sizeContext *slice_size();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Streaming_concatenationContext* streaming_concatenation();

  class  Stream_concatenationContext : public antlr4::ParserRuleContext {
  public:
    Stream_concatenationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACE();
    std::vector<Stream_expressionContext *> stream_expression();
    Stream_expressionContext* stream_expression(size_t i);
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Stream_concatenationContext* stream_concatenation();

  class  Stream_expressionContext : public antlr4::ParserRuleContext {
  public:
    Stream_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *KW_WITH();
    antlr4::tree::TerminalNode *LSQUARE_BR();
    Array_range_expressionContext *array_range_expression();
    antlr4::tree::TerminalNode *RSQUARE_BR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Stream_expressionContext* stream_expression();

  class  Array_range_expressionContext : public antlr4::ParserRuleContext {
  public:
    Array_range_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    Range_separatorContext *range_separator();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Array_range_expressionContext* array_range_expression();

  class  Range_separatorContext : public antlr4::ParserRuleContext {
  public:
    Range_separatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *PLUS();
    antlr4::tree::TerminalNode *MINUS();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Range_separatorContext* range_separator();

  class  Open_range_listContext : public antlr4::ParserRuleContext {
  public:
    Open_range_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Value_rangeContext *> value_range();
    Value_rangeContext* value_range(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Open_range_listContext* open_range_list();

  class  PatternContext : public antlr4::ParserRuleContext {
  public:
    PatternContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *MUL();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *KW_TAGGED();
    std::vector<PatternContext *> pattern();
    PatternContext* pattern(size_t i);
    antlr4::tree::TerminalNode *APOSTROPHE_LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<antlr4::tree::TerminalNode *> COLON();
    antlr4::tree::TerminalNode* COLON(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PatternContext* pattern();

  class  Assignment_patternContext : public antlr4::ParserRuleContext {
  public:
    Assignment_patternContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *APOSTROPHE_LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    std::vector<Structure_pattern_keyContext *> structure_pattern_key();
    Structure_pattern_keyContext* structure_pattern_key(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COLON();
    antlr4::tree::TerminalNode* COLON(size_t i);
    std::vector<Array_pattern_keyContext *> array_pattern_key();
    Array_pattern_keyContext* array_pattern_key(size_t i);
    Replication_assignmentContext *replication_assignment();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Assignment_patternContext* assignment_pattern();

  class  Replication_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Replication_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Replication_sizeContext *replication_size();
    antlr4::tree::TerminalNode *LBRACE();
    Replication_valueContext *replication_value();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Replication_assignmentContext* replication_assignment();

  class  Structure_pattern_keyContext : public antlr4::ParserRuleContext {
  public:
    Structure_pattern_keyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    Assignment_pattern_keyContext *assignment_pattern_key();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Structure_pattern_keyContext* structure_pattern_key();

  class  Array_pattern_keyContext : public antlr4::ParserRuleContext {
  public:
    Array_pattern_keyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Constant_expressionContext *constant_expression();
    Assignment_pattern_keyContext *assignment_pattern_key();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Array_pattern_keyContext* array_pattern_key();

  class  Assignment_pattern_keyContext : public antlr4::ParserRuleContext {
  public:
    Assignment_pattern_keyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_DEFAULT();
    Integer_typeContext *integer_type();
    Non_integer_typeContext *non_integer_type();
    Package_or_class_scoped_pathContext *package_or_class_scoped_path();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Assignment_pattern_keyContext* assignment_pattern_key();

  class  Struct_union_memberContext : public antlr4::ParserRuleContext {
  public:
    Struct_union_memberContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Data_type_or_voidContext *data_type_or_void();
    List_of_variable_decl_assignmentsContext *list_of_variable_decl_assignments();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    Random_qualifierContext *random_qualifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Struct_union_memberContext* struct_union_member();

  class  Data_type_or_voidContext : public antlr4::ParserRuleContext {
  public:
    Data_type_or_voidContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_VOID();
    Data_typeContext *data_type();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Data_type_or_voidContext* data_type_or_void();

  class  Enum_name_declarationContext : public antlr4::ParserRuleContext {
  public:
    Enum_name_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *LSQUARE_BR();
    std::vector<Integral_numberContext *> integral_number();
    Integral_numberContext* integral_number(size_t i);
    antlr4::tree::TerminalNode *RSQUARE_BR();
    antlr4::tree::TerminalNode *ASSIGN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *COLON();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Enum_name_declarationContext* enum_name_declaration();

  class  Assignment_pattern_expressionContext : public antlr4::ParserRuleContext {
  public:
    Assignment_pattern_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Assignment_patternContext *assignment_pattern();
    Assignment_pattern_expression_typeContext *assignment_pattern_expression_type();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Assignment_pattern_expressionContext* assignment_pattern_expression();

  class  Assignment_pattern_expression_typeContext : public antlr4::ParserRuleContext {
  public:
    Assignment_pattern_expression_typeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Package_or_class_scoped_pathContext *package_or_class_scoped_path();
    Integer_atom_typeContext *integer_atom_type();
    Type_referenceContext *type_reference();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Assignment_pattern_expression_typeContext* assignment_pattern_expression_type();

  class  Net_lvalueContext : public antlr4::ParserRuleContext {
  public:
    Net_lvalueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Variable_lvalueContext *variable_lvalue();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Net_lvalueContext* net_lvalue();

  class  Variable_lvalueContext : public antlr4::ParserRuleContext {
  public:
    Variable_lvalueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACE();
    std::vector<Variable_lvalueContext *> variable_lvalue();
    Variable_lvalueContext* variable_lvalue(size_t i);
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    Package_or_class_scoped_hier_id_with_selectContext *package_or_class_scoped_hier_id_with_select();
    Assignment_pattern_variable_lvalueContext *assignment_pattern_variable_lvalue();
    Assignment_pattern_expression_typeContext *assignment_pattern_expression_type();
    Streaming_concatenationContext *streaming_concatenation();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Variable_lvalueContext* variable_lvalue();

  class  Solve_before_listContext : public antlr4::ParserRuleContext {
  public:
    Solve_before_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<PrimaryContext *> primary();
    PrimaryContext* primary(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Solve_before_listContext* solve_before_list();

  class  Constraint_block_itemContext : public antlr4::ParserRuleContext {
  public:
    Constraint_block_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_SOLVE();
    std::vector<Solve_before_listContext *> solve_before_list();
    Solve_before_listContext* solve_before_list(size_t i);
    antlr4::tree::TerminalNode *KW_BEFORE();
    antlr4::tree::TerminalNode *SEMI();
    Constraint_expressionContext *constraint_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Constraint_block_itemContext* constraint_block_item();

  class  Constraint_expressionContext : public antlr4::ParserRuleContext {
  public:
    Constraint_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_IF();
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<Constraint_setContext *> constraint_set();
    Constraint_setContext* constraint_set(size_t i);
    antlr4::tree::TerminalNode *KW_ELSE();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_DISABLE();
    antlr4::tree::TerminalNode *KW_SOFT();
    PrimaryContext *primary();
    Expression_or_distContext *expression_or_dist();
    Uniqueness_constraintContext *uniqueness_constraint();
    antlr4::tree::TerminalNode *KW_FOREACH();
    antlr4::tree::TerminalNode *LSQUARE_BR();
    Loop_variablesContext *loop_variables();
    antlr4::tree::TerminalNode *RSQUARE_BR();
    antlr4::tree::TerminalNode *ARROW();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Constraint_expressionContext* constraint_expression();

  class  Uniqueness_constraintContext : public antlr4::ParserRuleContext {
  public:
    Uniqueness_constraintContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_UNIQUE();
    antlr4::tree::TerminalNode *LBRACE();
    Open_range_listContext *open_range_list();
    antlr4::tree::TerminalNode *RBRACE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Uniqueness_constraintContext* uniqueness_constraint();

  class  Constraint_setContext : public antlr4::ParserRuleContext {
  public:
    Constraint_setContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<Constraint_expressionContext *> constraint_expression();
    Constraint_expressionContext* constraint_expression(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Constraint_setContext* constraint_set();

  class  Randomize_callContext : public antlr4::ParserRuleContext {
  public:
    Randomize_callContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_RANDOMIZE();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    std::vector<antlr4::tree::TerminalNode *> LPAREN();
    antlr4::tree::TerminalNode* LPAREN(size_t i);
    std::vector<antlr4::tree::TerminalNode *> RPAREN();
    antlr4::tree::TerminalNode* RPAREN(size_t i);
    antlr4::tree::TerminalNode *KW_WITH();
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    antlr4::tree::TerminalNode *KW_NULL();
    std::vector<List_of_argumentsContext *> list_of_arguments();
    List_of_argumentsContext* list_of_arguments(size_t i);
    std::vector<Constraint_block_itemContext *> constraint_block_item();
    Constraint_block_itemContext* constraint_block_item(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Randomize_callContext* randomize_call();

  class  Module_header_commonContext : public antlr4::ParserRuleContext {
  public:
    Module_header_commonContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Module_keywordContext *module_keyword();
    IdentifierContext *identifier();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    LifetimeContext *lifetime();
    std::vector<Package_import_declarationContext *> package_import_declaration();
    Package_import_declarationContext* package_import_declaration(size_t i);
    Parameter_port_listContext *parameter_port_list();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Module_header_commonContext* module_header_common();

  class  Module_declarationContext : public antlr4::ParserRuleContext {
  public:
    Module_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_EXTERN();
    Module_header_commonContext *module_header_common();
    antlr4::tree::TerminalNode *SEMI();
    List_of_port_declarationsContext *list_of_port_declarations();
    antlr4::tree::TerminalNode *KW_ENDMODULE();
    antlr4::tree::TerminalNode *COLON();
    IdentifierContext *identifier();
    Timeunits_declarationContext *timeunits_declaration();
    std::vector<Module_itemContext *> module_item();
    Module_itemContext* module_item(size_t i);
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *MUL();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Module_declarationContext* module_declaration();

  class  Module_keywordContext : public antlr4::ParserRuleContext {
  public:
    Module_keywordContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_MODULE();
    antlr4::tree::TerminalNode *KW_MACROMODULE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Module_keywordContext* module_keyword();

  class  Net_port_typeContext : public antlr4::ParserRuleContext {
  public:
    Net_port_typeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_INTERCONNECT();
    Implicit_data_typeContext *implicit_data_type();
    Net_typeContext *net_type();
    Data_type_or_implicitContext *data_type_or_implicit();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Net_port_typeContext* net_port_type();

  class  Var_data_typeContext : public antlr4::ParserRuleContext {
  public:
    Var_data_typeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_VAR();
    Data_type_or_implicitContext *data_type_or_implicit();
    Data_typeContext *data_type();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Var_data_typeContext* var_data_type();

  class  Net_or_var_data_typeContext : public antlr4::ParserRuleContext {
  public:
    Net_or_var_data_typeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_INTERCONNECT();
    Implicit_data_typeContext *implicit_data_type();
    antlr4::tree::TerminalNode *KW_VAR();
    Data_type_or_implicitContext *data_type_or_implicit();
    Net_typeContext *net_type();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Net_or_var_data_typeContext* net_or_var_data_type();

  class  List_of_defparam_assignmentsContext : public antlr4::ParserRuleContext {
  public:
    List_of_defparam_assignmentsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Defparam_assignmentContext *> defparam_assignment();
    Defparam_assignmentContext* defparam_assignment(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  List_of_defparam_assignmentsContext* list_of_defparam_assignments();

  class  List_of_net_decl_assignmentsContext : public antlr4::ParserRuleContext {
  public:
    List_of_net_decl_assignmentsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Net_decl_assignmentContext *> net_decl_assignment();
    Net_decl_assignmentContext* net_decl_assignment(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  List_of_net_decl_assignmentsContext* list_of_net_decl_assignments();

  class  List_of_specparam_assignmentsContext : public antlr4::ParserRuleContext {
  public:
    List_of_specparam_assignmentsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Specparam_assignmentContext *> specparam_assignment();
    Specparam_assignmentContext* specparam_assignment(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  List_of_specparam_assignmentsContext* list_of_specparam_assignments();

  class  List_of_variable_decl_assignmentsContext : public antlr4::ParserRuleContext {
  public:
    List_of_variable_decl_assignmentsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Variable_decl_assignmentContext *> variable_decl_assignment();
    Variable_decl_assignmentContext* variable_decl_assignment(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  List_of_variable_decl_assignmentsContext* list_of_variable_decl_assignments();

  class  List_of_variable_identifiers_itemContext : public antlr4::ParserRuleContext {
  public:
    List_of_variable_identifiers_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    std::vector<Variable_dimensionContext *> variable_dimension();
    Variable_dimensionContext* variable_dimension(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  List_of_variable_identifiers_itemContext* list_of_variable_identifiers_item();

  class  List_of_variable_identifiersContext : public antlr4::ParserRuleContext {
  public:
    List_of_variable_identifiersContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<List_of_variable_identifiers_itemContext *> list_of_variable_identifiers_item();
    List_of_variable_identifiers_itemContext* list_of_variable_identifiers_item(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  List_of_variable_identifiersContext* list_of_variable_identifiers();

  class  List_of_variable_port_identifiersContext : public antlr4::ParserRuleContext {
  public:
    List_of_variable_port_identifiersContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    List_of_tf_variable_identifiersContext *list_of_tf_variable_identifiers();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  List_of_variable_port_identifiersContext* list_of_variable_port_identifiers();

  class  Defparam_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Defparam_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Hierarchical_identifierContext *hierarchical_identifier();
    antlr4::tree::TerminalNode *ASSIGN();
    Mintypmax_expressionContext *mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Defparam_assignmentContext* defparam_assignment();

  class  Net_decl_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Net_decl_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    std::vector<Unpacked_dimensionContext *> unpacked_dimension();
    Unpacked_dimensionContext* unpacked_dimension(size_t i);
    antlr4::tree::TerminalNode *ASSIGN();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Net_decl_assignmentContext* net_decl_assignment();

  class  Specparam_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Specparam_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *ASSIGN();
    Mintypmax_expressionContext *mintypmax_expression();
    Pulse_control_specparamContext *pulse_control_specparam();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Specparam_assignmentContext* specparam_assignment();

  class  Error_limit_valueContext : public antlr4::ParserRuleContext {
  public:
    Error_limit_valueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Mintypmax_expressionContext *mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Error_limit_valueContext* error_limit_value();

  class  Reject_limit_valueContext : public antlr4::ParserRuleContext {
  public:
    Reject_limit_valueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Mintypmax_expressionContext *mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Reject_limit_valueContext* reject_limit_value();

  class  Pulse_control_specparamContext : public antlr4::ParserRuleContext {
  public:
    Pulse_control_specparamContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_PATHPULSE_DOLAR();
    antlr4::tree::TerminalNode *ASSIGN();
    antlr4::tree::TerminalNode *LPAREN();
    Reject_limit_valueContext *reject_limit_value();
    antlr4::tree::TerminalNode *RPAREN();
    Specify_input_terminal_descriptorContext *specify_input_terminal_descriptor();
    antlr4::tree::TerminalNode *DOLAR();
    Specify_output_terminal_descriptorContext *specify_output_terminal_descriptor();
    antlr4::tree::TerminalNode *COMMA();
    Error_limit_valueContext *error_limit_value();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Pulse_control_specparamContext* pulse_control_specparam();

  class  Identifier_doted_index_at_endContext : public antlr4::ParserRuleContext {
  public:
    Identifier_doted_index_at_endContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *DOT();
    std::vector<antlr4::tree::TerminalNode *> LSQUARE_BR();
    antlr4::tree::TerminalNode* LSQUARE_BR(size_t i);
    std::vector<Range_expressionContext *> range_expression();
    Range_expressionContext* range_expression(size_t i);
    std::vector<antlr4::tree::TerminalNode *> RSQUARE_BR();
    antlr4::tree::TerminalNode* RSQUARE_BR(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Identifier_doted_index_at_endContext* identifier_doted_index_at_end();

  class  Specify_terminal_descriptorContext : public antlr4::ParserRuleContext {
  public:
    Specify_terminal_descriptorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Identifier_doted_index_at_endContext *identifier_doted_index_at_end();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Specify_terminal_descriptorContext* specify_terminal_descriptor();

  class  Specify_input_terminal_descriptorContext : public antlr4::ParserRuleContext {
  public:
    Specify_input_terminal_descriptorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Identifier_doted_index_at_endContext *identifier_doted_index_at_end();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Specify_input_terminal_descriptorContext* specify_input_terminal_descriptor();

  class  Specify_output_terminal_descriptorContext : public antlr4::ParserRuleContext {
  public:
    Specify_output_terminal_descriptorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Identifier_doted_index_at_endContext *identifier_doted_index_at_end();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Specify_output_terminal_descriptorContext* specify_output_terminal_descriptor();

  class  Specify_itemContext : public antlr4::ParserRuleContext {
  public:
    Specify_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Specparam_declarationContext *specparam_declaration();
    Pulsestyle_declarationContext *pulsestyle_declaration();
    Showcancelled_declarationContext *showcancelled_declaration();
    Path_declarationContext *path_declaration();
    System_timing_checkContext *system_timing_check();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Specify_itemContext* specify_item();

  class  Pulsestyle_declarationContext : public antlr4::ParserRuleContext {
  public:
    Pulsestyle_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    List_of_path_outputsContext *list_of_path_outputs();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_PULSESTYLE_ONEVENT();
    antlr4::tree::TerminalNode *KW_PULSESTYLE_ONDETECT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Pulsestyle_declarationContext* pulsestyle_declaration();

  class  Showcancelled_declarationContext : public antlr4::ParserRuleContext {
  public:
    Showcancelled_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    List_of_path_outputsContext *list_of_path_outputs();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_SHOWCANCELLED();
    antlr4::tree::TerminalNode *KW_NOSHOWCANCELLED();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Showcancelled_declarationContext* showcancelled_declaration();

  class  Path_declarationContext : public antlr4::ParserRuleContext {
  public:
    Path_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SEMI();
    Simple_path_declarationContext *simple_path_declaration();
    Edge_sensitive_path_declarationContext *edge_sensitive_path_declaration();
    State_dependent_path_declarationContext *state_dependent_path_declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Path_declarationContext* path_declaration();

  class  Simple_path_declarationContext : public antlr4::ParserRuleContext {
  public:
    Simple_path_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ASSIGN();
    Path_delay_valueContext *path_delay_value();
    Parallel_path_descriptionContext *parallel_path_description();
    Full_path_descriptionContext *full_path_description();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Simple_path_declarationContext* simple_path_declaration();

  class  Path_delay_valueContext : public antlr4::ParserRuleContext {
  public:
    Path_delay_valueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    List_of_path_delay_expressionsContext *list_of_path_delay_expressions();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Path_delay_valueContext* path_delay_value();

  class  List_of_path_outputsContext : public antlr4::ParserRuleContext {
  public:
    List_of_path_outputsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    List_of_pathsContext *list_of_paths();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  List_of_path_outputsContext* list_of_path_outputs();

  class  List_of_path_inputsContext : public antlr4::ParserRuleContext {
  public:
    List_of_path_inputsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    List_of_pathsContext *list_of_paths();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  List_of_path_inputsContext* list_of_path_inputs();

  class  List_of_pathsContext : public antlr4::ParserRuleContext {
  public:
    List_of_pathsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Identifier_doted_index_at_endContext *> identifier_doted_index_at_end();
    Identifier_doted_index_at_endContext* identifier_doted_index_at_end(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  List_of_pathsContext* list_of_paths();

  class  List_of_path_delay_expressionsContext : public antlr4::ParserRuleContext {
  public:
    List_of_path_delay_expressionsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    T_path_delay_expressionContext *t_path_delay_expression();
    Trise_path_delay_expressionContext *trise_path_delay_expression();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    Tfall_path_delay_expressionContext *tfall_path_delay_expression();
    Tz_path_delay_expressionContext *tz_path_delay_expression();
    T01_path_delay_expressionContext *t01_path_delay_expression();
    T10_path_delay_expressionContext *t10_path_delay_expression();
    T0z_path_delay_expressionContext *t0z_path_delay_expression();
    Tz1_path_delay_expressionContext *tz1_path_delay_expression();
    T1z_path_delay_expressionContext *t1z_path_delay_expression();
    Tz0_path_delay_expressionContext *tz0_path_delay_expression();
    T0x_path_delay_expressionContext *t0x_path_delay_expression();
    Tx1_path_delay_expressionContext *tx1_path_delay_expression();
    T1x_path_delay_expressionContext *t1x_path_delay_expression();
    Tx0_path_delay_expressionContext *tx0_path_delay_expression();
    Txz_path_delay_expressionContext *txz_path_delay_expression();
    Tzx_path_delay_expressionContext *tzx_path_delay_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  List_of_path_delay_expressionsContext* list_of_path_delay_expressions();

  class  T_path_delay_expressionContext : public antlr4::ParserRuleContext {
  public:
    T_path_delay_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Constant_mintypmax_expressionContext *constant_mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  T_path_delay_expressionContext* t_path_delay_expression();

  class  Trise_path_delay_expressionContext : public antlr4::ParserRuleContext {
  public:
    Trise_path_delay_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Constant_mintypmax_expressionContext *constant_mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Trise_path_delay_expressionContext* trise_path_delay_expression();

  class  Tfall_path_delay_expressionContext : public antlr4::ParserRuleContext {
  public:
    Tfall_path_delay_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Constant_mintypmax_expressionContext *constant_mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Tfall_path_delay_expressionContext* tfall_path_delay_expression();

  class  Tz_path_delay_expressionContext : public antlr4::ParserRuleContext {
  public:
    Tz_path_delay_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Constant_mintypmax_expressionContext *constant_mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Tz_path_delay_expressionContext* tz_path_delay_expression();

  class  T01_path_delay_expressionContext : public antlr4::ParserRuleContext {
  public:
    T01_path_delay_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Constant_mintypmax_expressionContext *constant_mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  T01_path_delay_expressionContext* t01_path_delay_expression();

  class  T10_path_delay_expressionContext : public antlr4::ParserRuleContext {
  public:
    T10_path_delay_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Constant_mintypmax_expressionContext *constant_mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  T10_path_delay_expressionContext* t10_path_delay_expression();

  class  T0z_path_delay_expressionContext : public antlr4::ParserRuleContext {
  public:
    T0z_path_delay_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Constant_mintypmax_expressionContext *constant_mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  T0z_path_delay_expressionContext* t0z_path_delay_expression();

  class  Tz1_path_delay_expressionContext : public antlr4::ParserRuleContext {
  public:
    Tz1_path_delay_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Constant_mintypmax_expressionContext *constant_mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Tz1_path_delay_expressionContext* tz1_path_delay_expression();

  class  T1z_path_delay_expressionContext : public antlr4::ParserRuleContext {
  public:
    T1z_path_delay_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Constant_mintypmax_expressionContext *constant_mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  T1z_path_delay_expressionContext* t1z_path_delay_expression();

  class  Tz0_path_delay_expressionContext : public antlr4::ParserRuleContext {
  public:
    Tz0_path_delay_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Constant_mintypmax_expressionContext *constant_mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Tz0_path_delay_expressionContext* tz0_path_delay_expression();

  class  T0x_path_delay_expressionContext : public antlr4::ParserRuleContext {
  public:
    T0x_path_delay_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Constant_mintypmax_expressionContext *constant_mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  T0x_path_delay_expressionContext* t0x_path_delay_expression();

  class  Tx1_path_delay_expressionContext : public antlr4::ParserRuleContext {
  public:
    Tx1_path_delay_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Constant_mintypmax_expressionContext *constant_mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Tx1_path_delay_expressionContext* tx1_path_delay_expression();

  class  T1x_path_delay_expressionContext : public antlr4::ParserRuleContext {
  public:
    T1x_path_delay_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Constant_mintypmax_expressionContext *constant_mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  T1x_path_delay_expressionContext* t1x_path_delay_expression();

  class  Tx0_path_delay_expressionContext : public antlr4::ParserRuleContext {
  public:
    Tx0_path_delay_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Constant_mintypmax_expressionContext *constant_mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Tx0_path_delay_expressionContext* tx0_path_delay_expression();

  class  Txz_path_delay_expressionContext : public antlr4::ParserRuleContext {
  public:
    Txz_path_delay_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Constant_mintypmax_expressionContext *constant_mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Txz_path_delay_expressionContext* txz_path_delay_expression();

  class  Tzx_path_delay_expressionContext : public antlr4::ParserRuleContext {
  public:
    Tzx_path_delay_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Constant_mintypmax_expressionContext *constant_mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Tzx_path_delay_expressionContext* tzx_path_delay_expression();

  class  Parallel_path_descriptionContext : public antlr4::ParserRuleContext {
  public:
    Parallel_path_descriptionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    Specify_input_terminal_descriptorContext *specify_input_terminal_descriptor();
    Any_implicationContext *any_implication();
    Specify_output_terminal_descriptorContext *specify_output_terminal_descriptor();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Parallel_path_descriptionContext* parallel_path_description();

  class  Full_path_descriptionContext : public antlr4::ParserRuleContext {
  public:
    Full_path_descriptionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    List_of_path_inputsContext *list_of_path_inputs();
    antlr4::tree::TerminalNode *PATH_FULL();
    List_of_path_outputsContext *list_of_path_outputs();
    antlr4::tree::TerminalNode *RPAREN();
    Operator_plus_minusContext *operator_plus_minus();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Full_path_descriptionContext* full_path_description();

  class  Identifier_listContext : public antlr4::ParserRuleContext {
  public:
    Identifier_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Identifier_listContext* identifier_list();

  class  Specparam_declarationContext : public antlr4::ParserRuleContext {
  public:
    Specparam_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_SPECPARAM();
    List_of_specparam_assignmentsContext *list_of_specparam_assignments();
    antlr4::tree::TerminalNode *SEMI();
    Packed_dimensionContext *packed_dimension();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Specparam_declarationContext* specparam_declaration();

  class  Edge_sensitive_path_declarationContext : public antlr4::ParserRuleContext {
  public:
    Edge_sensitive_path_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ASSIGN();
    Path_delay_valueContext *path_delay_value();
    Parallel_edge_sensitive_path_descriptionContext *parallel_edge_sensitive_path_description();
    Full_edge_sensitive_path_descriptionContext *full_edge_sensitive_path_description();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Edge_sensitive_path_declarationContext* edge_sensitive_path_declaration();

  class  Parallel_edge_sensitive_path_descriptionContext : public antlr4::ParserRuleContext {
  public:
    Parallel_edge_sensitive_path_descriptionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> LPAREN();
    antlr4::tree::TerminalNode* LPAREN(size_t i);
    Specify_input_terminal_descriptorContext *specify_input_terminal_descriptor();
    Any_implicationContext *any_implication();
    Specify_output_terminal_descriptorContext *specify_output_terminal_descriptor();
    antlr4::tree::TerminalNode *COLON();
    Data_source_expressionContext *data_source_expression();
    std::vector<antlr4::tree::TerminalNode *> RPAREN();
    antlr4::tree::TerminalNode* RPAREN(size_t i);
    Edge_identifierContext *edge_identifier();
    Operator_plus_minusContext *operator_plus_minus();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Parallel_edge_sensitive_path_descriptionContext* parallel_edge_sensitive_path_description();

  class  Full_edge_sensitive_path_descriptionContext : public antlr4::ParserRuleContext {
  public:
    Full_edge_sensitive_path_descriptionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> LPAREN();
    antlr4::tree::TerminalNode* LPAREN(size_t i);
    List_of_path_inputsContext *list_of_path_inputs();
    antlr4::tree::TerminalNode *PATH_FULL();
    List_of_path_outputsContext *list_of_path_outputs();
    antlr4::tree::TerminalNode *COLON();
    Data_source_expressionContext *data_source_expression();
    std::vector<antlr4::tree::TerminalNode *> RPAREN();
    antlr4::tree::TerminalNode* RPAREN(size_t i);
    Edge_identifierContext *edge_identifier();
    std::vector<Operator_plus_minusContext *> operator_plus_minus();
    Operator_plus_minusContext* operator_plus_minus(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Full_edge_sensitive_path_descriptionContext* full_edge_sensitive_path_description();

  class  Data_source_expressionContext : public antlr4::ParserRuleContext {
  public:
    Data_source_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Data_source_expressionContext* data_source_expression();

  class  Data_declarationContext : public antlr4::ParserRuleContext {
  public:
    Data_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    List_of_variable_decl_assignmentsContext *list_of_variable_decl_assignments();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_VAR();
    Data_type_or_implicitContext *data_type_or_implicit();
    antlr4::tree::TerminalNode *KW_CONST();
    LifetimeContext *lifetime();
    Type_declarationContext *type_declaration();
    Package_import_declarationContext *package_import_declaration();
    Net_type_declarationContext *net_type_declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Data_declarationContext* data_declaration();

  class  Module_path_expressionContext : public antlr4::ParserRuleContext {
  public:
    Module_path_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Module_path_expressionContext* module_path_expression();

  class  State_dependent_path_declarationContext : public antlr4::ParserRuleContext {
  public:
    State_dependent_path_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_IF();
    antlr4::tree::TerminalNode *LPAREN();
    Module_path_expressionContext *module_path_expression();
    antlr4::tree::TerminalNode *RPAREN();
    Simple_path_declarationContext *simple_path_declaration();
    Edge_sensitive_path_declarationContext *edge_sensitive_path_declaration();
    antlr4::tree::TerminalNode *KW_IFNONE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  State_dependent_path_declarationContext* state_dependent_path_declaration();

  class  Package_export_declarationContext : public antlr4::ParserRuleContext {
  public:
    Package_export_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_EXPORT();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<antlr4::tree::TerminalNode *> MUL();
    antlr4::tree::TerminalNode* MUL(size_t i);
    antlr4::tree::TerminalNode *DOUBLE_COLON();
    std::vector<Package_import_itemContext *> package_import_item();
    Package_import_itemContext* package_import_item(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Package_export_declarationContext* package_export_declaration();

  class  Genvar_declarationContext : public antlr4::ParserRuleContext {
  public:
    Genvar_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_GENVAR();
    Identifier_listContext *identifier_list();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Genvar_declarationContext* genvar_declaration();

  class  Net_declarationContext : public antlr4::ParserRuleContext {
  public:
    Net_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_INTERCONNECT();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    List_of_net_decl_assignmentsContext *list_of_net_decl_assignments();
    Net_typeContext *net_type();
    Implicit_data_typeContext *implicit_data_type();
    antlr4::tree::TerminalNode *HASH();
    Delay_valueContext *delay_value();
    std::vector<Unpacked_dimensionContext *> unpacked_dimension();
    Unpacked_dimensionContext* unpacked_dimension(size_t i);
    antlr4::tree::TerminalNode *COMMA();
    Drive_strengthContext *drive_strength();
    Charge_strengthContext *charge_strength();
    Data_type_or_implicitContext *data_type_or_implicit();
    Delay3Context *delay3();
    Delay_controlContext *delay_control();
    antlr4::tree::TerminalNode *KW_VECTORED();
    antlr4::tree::TerminalNode *KW_SCALARED();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Net_declarationContext* net_declaration();

  class  Parameter_port_listContext : public antlr4::ParserRuleContext {
  public:
    Parameter_port_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *HASH();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    List_of_param_assignmentsContext *list_of_param_assignments();
    std::vector<Parameter_port_declarationContext *> parameter_port_declaration();
    Parameter_port_declarationContext* parameter_port_declaration(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Parameter_port_listContext* parameter_port_list();

  class  Parameter_port_declarationContext : public antlr4::ParserRuleContext {
  public:
    Parameter_port_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_TYPE();
    List_of_type_assignmentsContext *list_of_type_assignments();
    Parameter_declarationContext *parameter_declaration();
    Local_parameter_declarationContext *local_parameter_declaration();
    Data_typeContext *data_type();
    List_of_param_assignmentsContext *list_of_param_assignments();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Parameter_port_declarationContext* parameter_port_declaration();

  class  List_of_port_declarations_ansi_itemContext : public antlr4::ParserRuleContext {
  public:
    List_of_port_declarations_ansi_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Ansi_port_declarationContext *ansi_port_declaration();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  List_of_port_declarations_ansi_itemContext* list_of_port_declarations_ansi_item();

  class  List_of_port_declarationsContext : public antlr4::ParserRuleContext {
  public:
    List_of_port_declarationsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<Nonansi_portContext *> nonansi_port();
    Nonansi_portContext* nonansi_port(size_t i);
    std::vector<List_of_port_declarations_ansi_itemContext *> list_of_port_declarations_ansi_item();
    List_of_port_declarations_ansi_itemContext* list_of_port_declarations_ansi_item(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  List_of_port_declarationsContext* list_of_port_declarations();

  class  Nonansi_port_declarationContext : public antlr4::ParserRuleContext {
  public:
    Nonansi_port_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_INOUT();
    List_of_variable_identifiersContext *list_of_variable_identifiers();
    antlr4::tree::TerminalNode *KW_INPUT();
    antlr4::tree::TerminalNode *KW_OUTPUT();
    List_of_variable_port_identifiersContext *list_of_variable_port_identifiers();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *KW_REF();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    Net_port_typeContext *net_port_type();
    Net_or_var_data_typeContext *net_or_var_data_type();
    antlr4::tree::TerminalNode *DOT();
    Var_data_typeContext *var_data_type();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Nonansi_port_declarationContext* nonansi_port_declaration();

  class  Nonansi_portContext : public antlr4::ParserRuleContext {
  public:
    Nonansi_portContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Nonansi_port__exprContext *nonansi_port__expr();
    antlr4::tree::TerminalNode *DOT();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Nonansi_portContext* nonansi_port();

  class  Nonansi_port__exprContext : public antlr4::ParserRuleContext {
  public:
    Nonansi_port__exprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Identifier_doted_index_at_endContext *> identifier_doted_index_at_end();
    Identifier_doted_index_at_endContext* identifier_doted_index_at_end(size_t i);
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Nonansi_port__exprContext* nonansi_port__expr();

  class  Port_identifierContext : public antlr4::ParserRuleContext {
  public:
    Port_identifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Port_identifierContext* port_identifier();

  class  Ansi_port_declarationContext : public antlr4::ParserRuleContext {
  public:
    Ansi_port_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Port_identifierContext *port_identifier();
    Port_directionContext *port_direction();
    Net_or_var_data_typeContext *net_or_var_data_type();
    std::vector<Variable_dimensionContext *> variable_dimension();
    Variable_dimensionContext* variable_dimension(size_t i);
    antlr4::tree::TerminalNode *ASSIGN();
    Constant_expressionContext *constant_expression();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *KW_INTERFACE();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Ansi_port_declarationContext* ansi_port_declaration();

  class  System_timing_checkContext : public antlr4::ParserRuleContext {
  public:
    System_timing_checkContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Dolar_setup_timing_checkContext *dolar_setup_timing_check();
    Dolar_hold_timing_checkContext *dolar_hold_timing_check();
    Dolar_setuphold_timing_checkContext *dolar_setuphold_timing_check();
    Dolar_recovery_timing_checkContext *dolar_recovery_timing_check();
    Dolar_removal_timing_checkContext *dolar_removal_timing_check();
    Dolar_recrem_timing_checkContext *dolar_recrem_timing_check();
    Dolar_skew_timing_checkContext *dolar_skew_timing_check();
    Dolar_timeskew_timing_checkContext *dolar_timeskew_timing_check();
    Dolar_fullskew_timing_checkContext *dolar_fullskew_timing_check();
    Dolar_period_timing_checkContext *dolar_period_timing_check();
    Dolar_width_timing_checkContext *dolar_width_timing_check();
    Dolar_nochange_timing_checkContext *dolar_nochange_timing_check();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  System_timing_checkContext* system_timing_check();

  class  Dolar_setup_timing_checkContext : public antlr4::ParserRuleContext {
  public:
    Dolar_setup_timing_checkContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_DOLAR_SETUP();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Timing_check_eventContext *> timing_check_event();
    Timing_check_eventContext* timing_check_event(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    Timing_check_limitContext *timing_check_limit();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *SEMI();
    NotifierContext *notifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Dolar_setup_timing_checkContext* dolar_setup_timing_check();

  class  Dolar_hold_timing_checkContext : public antlr4::ParserRuleContext {
  public:
    Dolar_hold_timing_checkContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_DOLAR_HOLD();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Timing_check_eventContext *> timing_check_event();
    Timing_check_eventContext* timing_check_event(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    Timing_check_limitContext *timing_check_limit();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *SEMI();
    NotifierContext *notifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Dolar_hold_timing_checkContext* dolar_hold_timing_check();

  class  Dolar_setuphold_timing_checkContext : public antlr4::ParserRuleContext {
  public:
    Dolar_setuphold_timing_checkContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_DOLAR_SETUPHOLD();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Timing_check_eventContext *> timing_check_event();
    Timing_check_eventContext* timing_check_event(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<Timing_check_limitContext *> timing_check_limit();
    Timing_check_limitContext* timing_check_limit(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *SEMI();
    NotifierContext *notifier();
    Timestamp_conditionContext *timestamp_condition();
    Timecheck_conditionContext *timecheck_condition();
    std::vector<Delayed_referenceContext *> delayed_reference();
    Delayed_referenceContext* delayed_reference(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Dolar_setuphold_timing_checkContext* dolar_setuphold_timing_check();

  class  Dolar_recovery_timing_checkContext : public antlr4::ParserRuleContext {
  public:
    Dolar_recovery_timing_checkContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_DOLAR_RECOVERY();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Timing_check_eventContext *> timing_check_event();
    Timing_check_eventContext* timing_check_event(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    Timing_check_limitContext *timing_check_limit();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *SEMI();
    NotifierContext *notifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Dolar_recovery_timing_checkContext* dolar_recovery_timing_check();

  class  Dolar_removal_timing_checkContext : public antlr4::ParserRuleContext {
  public:
    Dolar_removal_timing_checkContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_DOLAR_REMOVAL();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Timing_check_eventContext *> timing_check_event();
    Timing_check_eventContext* timing_check_event(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    Timing_check_limitContext *timing_check_limit();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *SEMI();
    NotifierContext *notifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Dolar_removal_timing_checkContext* dolar_removal_timing_check();

  class  Dolar_recrem_timing_checkContext : public antlr4::ParserRuleContext {
  public:
    Dolar_recrem_timing_checkContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_DOLAR_RECREM();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Timing_check_eventContext *> timing_check_event();
    Timing_check_eventContext* timing_check_event(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<Timing_check_limitContext *> timing_check_limit();
    Timing_check_limitContext* timing_check_limit(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *SEMI();
    NotifierContext *notifier();
    Timestamp_conditionContext *timestamp_condition();
    Timecheck_conditionContext *timecheck_condition();
    std::vector<Delayed_referenceContext *> delayed_reference();
    Delayed_referenceContext* delayed_reference(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Dolar_recrem_timing_checkContext* dolar_recrem_timing_check();

  class  Dolar_skew_timing_checkContext : public antlr4::ParserRuleContext {
  public:
    Dolar_skew_timing_checkContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_DOLAR_SKEW();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Timing_check_eventContext *> timing_check_event();
    Timing_check_eventContext* timing_check_event(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    Timing_check_limitContext *timing_check_limit();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *SEMI();
    NotifierContext *notifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Dolar_skew_timing_checkContext* dolar_skew_timing_check();

  class  Dolar_timeskew_timing_checkContext : public antlr4::ParserRuleContext {
  public:
    Dolar_timeskew_timing_checkContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_DOLAR_TIMESKEW();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Timing_check_eventContext *> timing_check_event();
    Timing_check_eventContext* timing_check_event(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    Timing_check_limitContext *timing_check_limit();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *SEMI();
    NotifierContext *notifier();
    Event_based_flagContext *event_based_flag();
    Remain_active_flagContext *remain_active_flag();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Dolar_timeskew_timing_checkContext* dolar_timeskew_timing_check();

  class  Dolar_fullskew_timing_checkContext : public antlr4::ParserRuleContext {
  public:
    Dolar_fullskew_timing_checkContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_DOLAR_FULLSKEW();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Timing_check_eventContext *> timing_check_event();
    Timing_check_eventContext* timing_check_event(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<Timing_check_limitContext *> timing_check_limit();
    Timing_check_limitContext* timing_check_limit(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *SEMI();
    NotifierContext *notifier();
    Event_based_flagContext *event_based_flag();
    Remain_active_flagContext *remain_active_flag();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Dolar_fullskew_timing_checkContext* dolar_fullskew_timing_check();

  class  Dolar_period_timing_checkContext : public antlr4::ParserRuleContext {
  public:
    Dolar_period_timing_checkContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_DOLAR_PERIOD();
    antlr4::tree::TerminalNode *LPAREN();
    Controlled_reference_eventContext *controlled_reference_event();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    Timing_check_limitContext *timing_check_limit();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *SEMI();
    NotifierContext *notifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Dolar_period_timing_checkContext* dolar_period_timing_check();

  class  Dolar_width_timing_checkContext : public antlr4::ParserRuleContext {
  public:
    Dolar_width_timing_checkContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_DOLAR_WIDTH();
    antlr4::tree::TerminalNode *LPAREN();
    Controlled_reference_eventContext *controlled_reference_event();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    Timing_check_limitContext *timing_check_limit();
    ThresholdContext *threshold();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *SEMI();
    NotifierContext *notifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Dolar_width_timing_checkContext* dolar_width_timing_check();

  class  Dolar_nochange_timing_checkContext : public antlr4::ParserRuleContext {
  public:
    Dolar_nochange_timing_checkContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_DOLAR_NOCHANGE();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Timing_check_eventContext *> timing_check_event();
    Timing_check_eventContext* timing_check_event(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    Start_edge_offsetContext *start_edge_offset();
    End_edge_offsetContext *end_edge_offset();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *SEMI();
    NotifierContext *notifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Dolar_nochange_timing_checkContext* dolar_nochange_timing_check();

  class  Timecheck_conditionContext : public antlr4::ParserRuleContext {
  public:
    Timecheck_conditionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Mintypmax_expressionContext *mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Timecheck_conditionContext* timecheck_condition();

  class  Controlled_reference_eventContext : public antlr4::ParserRuleContext {
  public:
    Controlled_reference_eventContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Controlled_timing_check_eventContext *controlled_timing_check_event();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Controlled_reference_eventContext* controlled_reference_event();

  class  Delayed_referenceContext : public antlr4::ParserRuleContext {
  public:
    Delayed_referenceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *LSQUARE_BR();
    Constant_mintypmax_expressionContext *constant_mintypmax_expression();
    antlr4::tree::TerminalNode *RSQUARE_BR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Delayed_referenceContext* delayed_reference();

  class  End_edge_offsetContext : public antlr4::ParserRuleContext {
  public:
    End_edge_offsetContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Mintypmax_expressionContext *mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  End_edge_offsetContext* end_edge_offset();

  class  Event_based_flagContext : public antlr4::ParserRuleContext {
  public:
    Event_based_flagContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Constant_expressionContext *constant_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Event_based_flagContext* event_based_flag();

  class  NotifierContext : public antlr4::ParserRuleContext {
  public:
    NotifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  NotifierContext* notifier();

  class  Remain_active_flagContext : public antlr4::ParserRuleContext {
  public:
    Remain_active_flagContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Constant_mintypmax_expressionContext *constant_mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Remain_active_flagContext* remain_active_flag();

  class  Timestamp_conditionContext : public antlr4::ParserRuleContext {
  public:
    Timestamp_conditionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Mintypmax_expressionContext *mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Timestamp_conditionContext* timestamp_condition();

  class  Start_edge_offsetContext : public antlr4::ParserRuleContext {
  public:
    Start_edge_offsetContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Mintypmax_expressionContext *mintypmax_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Start_edge_offsetContext* start_edge_offset();

  class  ThresholdContext : public antlr4::ParserRuleContext {
  public:
    ThresholdContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Constant_expressionContext *constant_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ThresholdContext* threshold();

  class  Timing_check_limitContext : public antlr4::ParserRuleContext {
  public:
    Timing_check_limitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Timing_check_limitContext* timing_check_limit();

  class  Timing_check_eventContext : public antlr4::ParserRuleContext {
  public:
    Timing_check_eventContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Specify_terminal_descriptorContext *specify_terminal_descriptor();
    Timing_check_event_controlContext *timing_check_event_control();
    antlr4::tree::TerminalNode *TRIPLE_AND();
    Timing_check_conditionContext *timing_check_condition();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Timing_check_eventContext* timing_check_event();

  class  Timing_check_conditionContext : public antlr4::ParserRuleContext {
  public:
    Timing_check_conditionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    Scalar_timing_check_conditionContext *scalar_timing_check_condition();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Timing_check_conditionContext* timing_check_condition();

  class  Scalar_timing_check_conditionContext : public antlr4::ParserRuleContext {
  public:
    Scalar_timing_check_conditionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Scalar_timing_check_conditionContext* scalar_timing_check_condition();

  class  Controlled_timing_check_eventContext : public antlr4::ParserRuleContext {
  public:
    Controlled_timing_check_eventContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Timing_check_event_controlContext *timing_check_event_control();
    Specify_terminal_descriptorContext *specify_terminal_descriptor();
    antlr4::tree::TerminalNode *TRIPLE_AND();
    Timing_check_conditionContext *timing_check_condition();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Controlled_timing_check_eventContext* controlled_timing_check_event();

  class  Function_data_type_or_implicitContext : public antlr4::ParserRuleContext {
  public:
    Function_data_type_or_implicitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Data_type_or_voidContext *data_type_or_void();
    Implicit_data_typeContext *implicit_data_type();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Function_data_type_or_implicitContext* function_data_type_or_implicit();

  class  Extern_tf_declarationContext : public antlr4::ParserRuleContext {
  public:
    Extern_tf_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_EXTERN();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_FORKJOIN();
    Task_prototypeContext *task_prototype();
    Method_prototypeContext *method_prototype();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Extern_tf_declarationContext* extern_tf_declaration();

  class  Function_declarationContext : public antlr4::ParserRuleContext {
  public:
    Function_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_FUNCTION();
    Untyped_function_declarationContext *untyped_function_declaration();
    antlr4::tree::TerminalNode *KW_ENDFUNCTION();
    antlr4::tree::TerminalNode *COLON();
    IdentifierContext *identifier();
    LifetimeContext *lifetime();
    Function_data_type_or_implicitContext *function_data_type_or_implicit();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Function_declarationContext* function_declaration();

  class  Untyped_function_declarationContext : public antlr4::ParserRuleContext {
  public:
    Untyped_function_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Task_and_function_declaration_commonContext *task_and_function_declaration_common();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Untyped_function_declarationContext* untyped_function_declaration();

  class  Task_prototypeContext : public antlr4::ParserRuleContext {
  public:
    Task_prototypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_TASK();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *LPAREN();
    Tf_port_listContext *tf_port_list();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Task_prototypeContext* task_prototype();

  class  Function_prototypeContext : public antlr4::ParserRuleContext {
  public:
    Function_prototypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_FUNCTION();
    Data_type_or_voidContext *data_type_or_void();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *LPAREN();
    Tf_port_listContext *tf_port_list();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Function_prototypeContext* function_prototype();

  class  Dpi_import_exportContext : public antlr4::ParserRuleContext {
  public:
    Dpi_import_exportContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_IMPORT();
    antlr4::tree::TerminalNode *STRING_LITERAL();
    antlr4::tree::TerminalNode *KW_EXPORT();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *KW_FUNCTION();
    antlr4::tree::TerminalNode *KW_TASK();
    Function_prototypeContext *function_prototype();
    Task_prototypeContext *task_prototype();
    antlr4::tree::TerminalNode *ASSIGN();
    antlr4::tree::TerminalNode *C_IDENTIFIER();
    antlr4::tree::TerminalNode *ESCAPED_IDENTIFIER();
    Dpi_function_import_propertyContext *dpi_function_import_property();
    Dpi_task_import_propertyContext *dpi_task_import_property();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Dpi_import_exportContext* dpi_import_export();

  class  Dpi_function_import_propertyContext : public antlr4::ParserRuleContext {
  public:
    Dpi_function_import_propertyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_CONTEXT();
    antlr4::tree::TerminalNode *KW_PURE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Dpi_function_import_propertyContext* dpi_function_import_property();

  class  Dpi_task_import_propertyContext : public antlr4::ParserRuleContext {
  public:
    Dpi_task_import_propertyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_CONTEXT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Dpi_task_import_propertyContext* dpi_task_import_property();

  class  Task_and_function_declaration_commonContext : public antlr4::ParserRuleContext {
  public:
    Task_and_function_declaration_commonContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *LPAREN();
    Tf_port_listContext *tf_port_list();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *DOT();
    Class_scopeContext *class_scope();
    std::vector<Statement_or_nullContext *> statement_or_null();
    Statement_or_nullContext* statement_or_null(size_t i);
    std::vector<Tf_item_declarationContext *> tf_item_declaration();
    Tf_item_declarationContext* tf_item_declaration(size_t i);
    std::vector<Block_item_declarationContext *> block_item_declaration();
    Block_item_declarationContext* block_item_declaration(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Task_and_function_declaration_commonContext* task_and_function_declaration_common();

  class  Task_declarationContext : public antlr4::ParserRuleContext {
  public:
    Task_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_TASK();
    Task_and_function_declaration_commonContext *task_and_function_declaration_common();
    antlr4::tree::TerminalNode *KW_ENDTASK();
    antlr4::tree::TerminalNode *COLON();
    IdentifierContext *identifier();
    LifetimeContext *lifetime();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Task_declarationContext* task_declaration();

  class  Method_prototypeContext : public antlr4::ParserRuleContext {
  public:
    Method_prototypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Task_prototypeContext *task_prototype();
    Function_prototypeContext *function_prototype();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Method_prototypeContext* method_prototype();

  class  Extern_constraint_declarationContext : public antlr4::ParserRuleContext {
  public:
    Extern_constraint_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_CONSTRAINT();
    Class_scopeContext *class_scope();
    IdentifierContext *identifier();
    Constraint_blockContext *constraint_block();
    antlr4::tree::TerminalNode *KW_STATIC();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Extern_constraint_declarationContext* extern_constraint_declaration();

  class  Constraint_blockContext : public antlr4::ParserRuleContext {
  public:
    Constraint_blockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<Constraint_block_itemContext *> constraint_block_item();
    Constraint_block_itemContext* constraint_block_item(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Constraint_blockContext* constraint_block();

  class  Checker_port_listContext : public antlr4::ParserRuleContext {
  public:
    Checker_port_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Checker_port_itemContext *> checker_port_item();
    Checker_port_itemContext* checker_port_item(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Checker_port_listContext* checker_port_list();

  class  Checker_port_itemContext : public antlr4::ParserRuleContext {
  public:
    Checker_port_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    Checker_port_directionContext *checker_port_direction();
    Property_formal_typeContext *property_formal_type();
    std::vector<Variable_dimensionContext *> variable_dimension();
    Variable_dimensionContext* variable_dimension(size_t i);
    antlr4::tree::TerminalNode *ASSIGN();
    Property_actual_argContext *property_actual_arg();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Checker_port_itemContext* checker_port_item();

  class  Checker_port_directionContext : public antlr4::ParserRuleContext {
  public:
    Checker_port_directionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_INPUT();
    antlr4::tree::TerminalNode *KW_OUTPUT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Checker_port_directionContext* checker_port_direction();

  class  Checker_declarationContext : public antlr4::ParserRuleContext {
  public:
    Checker_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_CHECKER();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_ENDCHECKER();
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<Checker_or_generate_itemContext *> checker_or_generate_item();
    Checker_or_generate_itemContext* checker_or_generate_item(size_t i);
    Checker_port_listContext *checker_port_list();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Checker_declarationContext* checker_declaration();

  class  Class_declarationContext : public antlr4::ParserRuleContext {
  public:
    Class_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_CLASS();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_ENDCLASS();
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *KW_VIRTUAL();
    LifetimeContext *lifetime();
    Parameter_port_listContext *parameter_port_list();
    antlr4::tree::TerminalNode *KW_EXTENDS();
    Class_typeContext *class_type();
    antlr4::tree::TerminalNode *KW_IMPLEMENTS();
    std::vector<Interface_class_typeContext *> interface_class_type();
    Interface_class_typeContext* interface_class_type(size_t i);
    std::vector<Class_itemContext *> class_item();
    Class_itemContext* class_item(size_t i);
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    List_of_argumentsContext *list_of_arguments();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Class_declarationContext* class_declaration();

  class  Always_constructContext : public antlr4::ParserRuleContext {
  public:
    Always_constructContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Always_keywordContext *always_keyword();
    StatementContext *statement();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Always_constructContext* always_construct();

  class  Interface_class_typeContext : public antlr4::ParserRuleContext {
  public:
    Interface_class_typeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Ps_identifierContext *ps_identifier();
    Parameter_value_assignmentContext *parameter_value_assignment();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interface_class_typeContext* interface_class_type();

  class  Interface_class_declarationContext : public antlr4::ParserRuleContext {
  public:
    Interface_class_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_INTERFACE();
    antlr4::tree::TerminalNode *KW_CLASS();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_ENDCLASS();
    antlr4::tree::TerminalNode *COLON();
    Parameter_port_listContext *parameter_port_list();
    antlr4::tree::TerminalNode *KW_EXTENDS();
    std::vector<Interface_class_typeContext *> interface_class_type();
    Interface_class_typeContext* interface_class_type(size_t i);
    std::vector<Interface_class_itemContext *> interface_class_item();
    Interface_class_itemContext* interface_class_item(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interface_class_declarationContext* interface_class_declaration();

  class  Interface_class_itemContext : public antlr4::ParserRuleContext {
  public:
    Interface_class_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Type_declarationContext *type_declaration();
    Interface_class_methodContext *interface_class_method();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    antlr4::tree::TerminalNode *SEMI();
    Local_parameter_declarationContext *local_parameter_declaration();
    Parameter_declarationContext *parameter_declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interface_class_itemContext* interface_class_item();

  class  Interface_class_methodContext : public antlr4::ParserRuleContext {
  public:
    Interface_class_methodContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_PURE();
    antlr4::tree::TerminalNode *KW_VIRTUAL();
    Method_prototypeContext *method_prototype();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interface_class_methodContext* interface_class_method();

  class  Package_declarationContext : public antlr4::ParserRuleContext {
  public:
    Package_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_PACKAGE();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_ENDPACKAGE();
    antlr4::tree::TerminalNode *COLON();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    LifetimeContext *lifetime();
    Timeunits_declarationContext *timeunits_declaration();
    std::vector<Package_itemContext *> package_item();
    Package_itemContext* package_item(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Package_declarationContext* package_declaration();

  class  Package_itemContext : public antlr4::ParserRuleContext {
  public:
    Package_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Net_declarationContext *net_declaration();
    Data_declarationContext *data_declaration();
    Task_declarationContext *task_declaration();
    Function_declarationContext *function_declaration();
    Checker_declarationContext *checker_declaration();
    Dpi_import_exportContext *dpi_import_export();
    Extern_constraint_declarationContext *extern_constraint_declaration();
    Class_declarationContext *class_declaration();
    Interface_class_declarationContext *interface_class_declaration();
    Class_constructor_declarationContext *class_constructor_declaration();
    antlr4::tree::TerminalNode *SEMI();
    Local_parameter_declarationContext *local_parameter_declaration();
    Parameter_declarationContext *parameter_declaration();
    Covergroup_declarationContext *covergroup_declaration();
    Property_declarationContext *property_declaration();
    Sequence_declarationContext *sequence_declaration();
    Let_declarationContext *let_declaration();
    Anonymous_programContext *anonymous_program();
    Package_export_declarationContext *package_export_declaration();
    Timeunits_declarationContext *timeunits_declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Package_itemContext* package_item();

  class  Program_declarationContext : public antlr4::ParserRuleContext {
  public:
    Program_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_EXTERN();
    Program_headerContext *program_header();
    antlr4::tree::TerminalNode *KW_ENDPROGRAM();
    antlr4::tree::TerminalNode *COLON();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *KW_PROGRAM();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *MUL();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *SEMI();
    Timeunits_declarationContext *timeunits_declaration();
    std::vector<Program_itemContext *> program_item();
    Program_itemContext* program_item(size_t i);
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Program_declarationContext* program_declaration();

  class  Program_headerContext : public antlr4::ParserRuleContext {
  public:
    Program_headerContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_PROGRAM();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    LifetimeContext *lifetime();
    std::vector<Package_import_declarationContext *> package_import_declaration();
    Package_import_declarationContext* package_import_declaration(size_t i);
    Parameter_port_listContext *parameter_port_list();
    List_of_port_declarationsContext *list_of_port_declarations();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Program_headerContext* program_header();

  class  Program_itemContext : public antlr4::ParserRuleContext {
  public:
    Program_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Nonansi_port_declarationContext *nonansi_port_declaration();
    antlr4::tree::TerminalNode *SEMI();
    Non_port_program_itemContext *non_port_program_item();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Program_itemContext* program_item();

  class  Non_port_program_itemContext : public antlr4::ParserRuleContext {
  public:
    Non_port_program_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Continuous_assignContext *continuous_assign();
    antlr4::tree::TerminalNode *SEMI();
    Net_declarationContext *net_declaration();
    Data_declarationContext *data_declaration();
    Task_declarationContext *task_declaration();
    Function_declarationContext *function_declaration();
    Checker_declarationContext *checker_declaration();
    Dpi_import_exportContext *dpi_import_export();
    Extern_constraint_declarationContext *extern_constraint_declaration();
    Class_declarationContext *class_declaration();
    Interface_class_declarationContext *interface_class_declaration();
    Class_constructor_declarationContext *class_constructor_declaration();
    Covergroup_declarationContext *covergroup_declaration();
    Property_declarationContext *property_declaration();
    Sequence_declarationContext *sequence_declaration();
    Let_declarationContext *let_declaration();
    Genvar_declarationContext *genvar_declaration();
    Clocking_declarationContext *clocking_declaration();
    Initial_constructContext *initial_construct();
    Final_constructContext *final_construct();
    Concurrent_assertion_itemContext *concurrent_assertion_item();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    Default_clocking_or_dissable_constructContext *default_clocking_or_dissable_construct();
    Local_parameter_declarationContext *local_parameter_declaration();
    Parameter_declarationContext *parameter_declaration();
    Timeunits_declarationContext *timeunits_declaration();
    Program_generate_itemContext *program_generate_item();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Non_port_program_itemContext* non_port_program_item();

  class  Anonymous_programContext : public antlr4::ParserRuleContext {
  public:
    Anonymous_programContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_PROGRAM();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_ENDPROGRAM();
    std::vector<Anonymous_program_itemContext *> anonymous_program_item();
    Anonymous_program_itemContext* anonymous_program_item(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Anonymous_programContext* anonymous_program();

  class  Anonymous_program_itemContext : public antlr4::ParserRuleContext {
  public:
    Anonymous_program_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SEMI();
    Task_declarationContext *task_declaration();
    Function_declarationContext *function_declaration();
    Class_declarationContext *class_declaration();
    Interface_class_declarationContext *interface_class_declaration();
    Covergroup_declarationContext *covergroup_declaration();
    Class_constructor_declarationContext *class_constructor_declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Anonymous_program_itemContext* anonymous_program_item();

  class  Sequence_declarationContext : public antlr4::ParserRuleContext {
  public:
    Sequence_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_SEQUENCE();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    std::vector<antlr4::tree::TerminalNode *> SEMI();
    antlr4::tree::TerminalNode* SEMI(size_t i);
    Sequence_exprContext *sequence_expr();
    antlr4::tree::TerminalNode *KW_ENDSEQUENCE();
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<Assertion_variable_declarationContext *> assertion_variable_declaration();
    Assertion_variable_declarationContext* assertion_variable_declaration(size_t i);
    Sequence_port_listContext *sequence_port_list();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sequence_declarationContext* sequence_declaration();

  class  Sequence_port_listContext : public antlr4::ParserRuleContext {
  public:
    Sequence_port_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Sequence_port_itemContext *> sequence_port_item();
    Sequence_port_itemContext* sequence_port_item(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sequence_port_listContext* sequence_port_list();

  class  Sequence_port_itemContext : public antlr4::ParserRuleContext {
  public:
    Sequence_port_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    antlr4::tree::TerminalNode *KW_LOCAL();
    Sequence_formal_typeContext *sequence_formal_type();
    std::vector<Variable_dimensionContext *> variable_dimension();
    Variable_dimensionContext* variable_dimension(size_t i);
    antlr4::tree::TerminalNode *ASSIGN();
    Sequence_actual_argContext *sequence_actual_arg();
    Sequence_lvar_port_directionContext *sequence_lvar_port_direction();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sequence_port_itemContext* sequence_port_item();

  class  Property_declarationContext : public antlr4::ParserRuleContext {
  public:
    Property_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_PROPERTY();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    std::vector<antlr4::tree::TerminalNode *> SEMI();
    antlr4::tree::TerminalNode* SEMI(size_t i);
    Property_specContext *property_spec();
    antlr4::tree::TerminalNode *KW_ENDPROPERTY();
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<Assertion_variable_declarationContext *> assertion_variable_declaration();
    Assertion_variable_declarationContext* assertion_variable_declaration(size_t i);
    Property_port_listContext *property_port_list();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Property_declarationContext* property_declaration();

  class  Property_port_listContext : public antlr4::ParserRuleContext {
  public:
    Property_port_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Property_port_itemContext *> property_port_item();
    Property_port_itemContext* property_port_item(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Property_port_listContext* property_port_list();

  class  Property_port_itemContext : public antlr4::ParserRuleContext {
  public:
    Property_port_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    antlr4::tree::TerminalNode *KW_LOCAL();
    Property_formal_typeContext *property_formal_type();
    std::vector<Variable_dimensionContext *> variable_dimension();
    Variable_dimensionContext* variable_dimension(size_t i);
    antlr4::tree::TerminalNode *ASSIGN();
    Property_actual_argContext *property_actual_arg();
    antlr4::tree::TerminalNode *KW_INPUT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Property_port_itemContext* property_port_item();

  class  Continuous_assignContext : public antlr4::ParserRuleContext {
  public:
    Continuous_assignContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_ASSIGN();
    antlr4::tree::TerminalNode *SEMI();
    List_of_variable_assignmentsContext *list_of_variable_assignments();
    Delay_controlContext *delay_control();
    Drive_strengthContext *drive_strength();
    Delay3Context *delay3();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Continuous_assignContext* continuous_assign();

  class  Checker_or_generate_itemContext : public antlr4::ParserRuleContext {
  public:
    Checker_or_generate_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Data_declarationContext *data_declaration();
    antlr4::tree::TerminalNode *KW_RAND();
    Module_or_generate_or_interface_or_checker_itemContext *module_or_generate_or_interface_or_checker_item();
    antlr4::tree::TerminalNode *SEMI();
    Default_clocking_or_dissable_constructContext *default_clocking_or_dissable_construct();
    Program_generate_itemContext *program_generate_item();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Checker_or_generate_itemContext* checker_or_generate_item();

  class  Constraint_prototypeContext : public antlr4::ParserRuleContext {
  public:
    Constraint_prototypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_CONSTRAINT();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *SEMI();
    Constraint_prototype_qualifierContext *constraint_prototype_qualifier();
    antlr4::tree::TerminalNode *KW_STATIC();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Constraint_prototypeContext* constraint_prototype();

  class  Class_constraintContext : public antlr4::ParserRuleContext {
  public:
    Class_constraintContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Constraint_prototypeContext *constraint_prototype();
    Constraint_declarationContext *constraint_declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Class_constraintContext* class_constraint();

  class  Constraint_declarationContext : public antlr4::ParserRuleContext {
  public:
    Constraint_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_CONSTRAINT();
    IdentifierContext *identifier();
    Constraint_blockContext *constraint_block();
    antlr4::tree::TerminalNode *KW_STATIC();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Constraint_declarationContext* constraint_declaration();

  class  Class_constructor_declarationContext : public antlr4::ParserRuleContext {
  public:
    Class_constructor_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_FUNCTION();
    std::vector<antlr4::tree::TerminalNode *> KW_NEW();
    antlr4::tree::TerminalNode* KW_NEW(size_t i);
    std::vector<antlr4::tree::TerminalNode *> SEMI();
    antlr4::tree::TerminalNode* SEMI(size_t i);
    antlr4::tree::TerminalNode *KW_ENDFUNCTION();
    Class_scopeContext *class_scope();
    std::vector<antlr4::tree::TerminalNode *> LPAREN();
    antlr4::tree::TerminalNode* LPAREN(size_t i);
    Tf_port_listContext *tf_port_list();
    std::vector<antlr4::tree::TerminalNode *> RPAREN();
    antlr4::tree::TerminalNode* RPAREN(size_t i);
    std::vector<Block_item_declarationContext *> block_item_declaration();
    Block_item_declarationContext* block_item_declaration(size_t i);
    antlr4::tree::TerminalNode *KW_SUPER();
    antlr4::tree::TerminalNode *DOT();
    std::vector<Statement_or_nullContext *> statement_or_null();
    Statement_or_nullContext* statement_or_null(size_t i);
    antlr4::tree::TerminalNode *COLON();
    List_of_argumentsContext *list_of_arguments();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Class_constructor_declarationContext* class_constructor_declaration();

  class  Class_propertyContext : public antlr4::ParserRuleContext {
  public:
    Class_propertyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_CONST();
    Data_typeContext *data_type();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<Class_item_qualifierContext *> class_item_qualifier();
    Class_item_qualifierContext* class_item_qualifier(size_t i);
    antlr4::tree::TerminalNode *ASSIGN();
    Constant_expressionContext *constant_expression();
    Data_declarationContext *data_declaration();
    std::vector<Property_qualifierContext *> property_qualifier();
    Property_qualifierContext* property_qualifier(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Class_propertyContext* class_property();

  class  Class_methodContext : public antlr4::ParserRuleContext {
  public:
    Class_methodContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_PURE();
    antlr4::tree::TerminalNode *KW_VIRTUAL();
    Method_prototypeContext *method_prototype();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<Class_item_qualifierContext *> class_item_qualifier();
    Class_item_qualifierContext* class_item_qualifier(size_t i);
    antlr4::tree::TerminalNode *KW_EXTERN();
    Class_constructor_prototypeContext *class_constructor_prototype();
    std::vector<Method_qualifierContext *> method_qualifier();
    Method_qualifierContext* method_qualifier(size_t i);
    Task_declarationContext *task_declaration();
    Function_declarationContext *function_declaration();
    Class_constructor_declarationContext *class_constructor_declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Class_methodContext* class_method();

  class  Class_constructor_prototypeContext : public antlr4::ParserRuleContext {
  public:
    Class_constructor_prototypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_FUNCTION();
    antlr4::tree::TerminalNode *KW_NEW();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *LPAREN();
    Tf_port_listContext *tf_port_list();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Class_constructor_prototypeContext* class_constructor_prototype();

  class  Class_itemContext : public antlr4::ParserRuleContext {
  public:
    Class_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Class_propertyContext *class_property();
    Class_methodContext *class_method();
    Class_constraintContext *class_constraint();
    Class_declarationContext *class_declaration();
    Covergroup_declarationContext *covergroup_declaration();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    antlr4::tree::TerminalNode *SEMI();
    Local_parameter_declarationContext *local_parameter_declaration();
    Parameter_declarationContext *parameter_declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Class_itemContext* class_item();

  class  Parameter_overrideContext : public antlr4::ParserRuleContext {
  public:
    Parameter_overrideContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_DEFPARAM();
    List_of_defparam_assignmentsContext *list_of_defparam_assignments();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Parameter_overrideContext* parameter_override();

  class  Gate_instantiationContext : public antlr4::ParserRuleContext {
  public:
    Gate_instantiationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SEMI();
    std::vector<Pull_gate_instanceContext *> pull_gate_instance();
    Pull_gate_instanceContext* pull_gate_instance(size_t i);
    std::vector<Enable_gate_or_mos_switch_or_cmos_switch_instanceContext *> enable_gate_or_mos_switch_or_cmos_switch_instance();
    Enable_gate_or_mos_switch_or_cmos_switch_instanceContext* enable_gate_or_mos_switch_or_cmos_switch_instance(size_t i);
    Enable_gatetypeContext *enable_gatetype();
    N_input_gatetypeContext *n_input_gatetype();
    std::vector<N_input_gate_instanceContext *> n_input_gate_instance();
    N_input_gate_instanceContext* n_input_gate_instance(size_t i);
    N_output_gatetypeContext *n_output_gatetype();
    std::vector<N_output_gate_instanceContext *> n_output_gate_instance();
    N_output_gate_instanceContext* n_output_gate_instance(size_t i);
    Pass_en_switchtypeContext *pass_en_switchtype();
    std::vector<Pass_enable_switch_instanceContext *> pass_enable_switch_instance();
    Pass_enable_switch_instanceContext* pass_enable_switch_instance(size_t i);
    Pass_switchtypeContext *pass_switchtype();
    std::vector<Pass_switch_instanceContext *> pass_switch_instance();
    Pass_switch_instanceContext* pass_switch_instance(size_t i);
    antlr4::tree::TerminalNode *KW_PULLDOWN();
    antlr4::tree::TerminalNode *KW_PULLUP();
    Cmos_switchtypeContext *cmos_switchtype();
    Mos_switchtypeContext *mos_switchtype();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    Delay3Context *delay3();
    Drive_strengthContext *drive_strength();
    Delay2Context *delay2();
    Pulldown_strengthContext *pulldown_strength();
    Pullup_strengthContext *pullup_strength();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Gate_instantiationContext* gate_instantiation();

  class  Enable_gate_or_mos_switch_or_cmos_switch_instanceContext : public antlr4::ParserRuleContext {
  public:
    Enable_gate_or_mos_switch_or_cmos_switch_instanceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    Output_terminalContext *output_terminal();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    Input_terminalContext *input_terminal();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    Name_of_instanceContext *name_of_instance();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Enable_gate_or_mos_switch_or_cmos_switch_instanceContext* enable_gate_or_mos_switch_or_cmos_switch_instance();

  class  N_input_gate_instanceContext : public antlr4::ParserRuleContext {
  public:
    N_input_gate_instanceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    Output_terminalContext *output_terminal();
    antlr4::tree::TerminalNode *RPAREN();
    Name_of_instanceContext *name_of_instance();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<Input_terminalContext *> input_terminal();
    Input_terminalContext* input_terminal(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  N_input_gate_instanceContext* n_input_gate_instance();

  class  N_output_gate_instanceContext : public antlr4::ParserRuleContext {
  public:
    N_output_gate_instanceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Output_terminalContext *> output_terminal();
    Output_terminalContext* output_terminal(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    Input_terminalContext *input_terminal();
    antlr4::tree::TerminalNode *RPAREN();
    Name_of_instanceContext *name_of_instance();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  N_output_gate_instanceContext* n_output_gate_instance();

  class  Pass_switch_instanceContext : public antlr4::ParserRuleContext {
  public:
    Pass_switch_instanceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Inout_terminalContext *> inout_terminal();
    Inout_terminalContext* inout_terminal(size_t i);
    antlr4::tree::TerminalNode *COMMA();
    antlr4::tree::TerminalNode *RPAREN();
    Name_of_instanceContext *name_of_instance();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Pass_switch_instanceContext* pass_switch_instance();

  class  Pass_enable_switch_instanceContext : public antlr4::ParserRuleContext {
  public:
    Pass_enable_switch_instanceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Inout_terminalContext *> inout_terminal();
    Inout_terminalContext* inout_terminal(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    Enable_terminalContext *enable_terminal();
    antlr4::tree::TerminalNode *RPAREN();
    Name_of_instanceContext *name_of_instance();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Pass_enable_switch_instanceContext* pass_enable_switch_instance();

  class  Pull_gate_instanceContext : public antlr4::ParserRuleContext {
  public:
    Pull_gate_instanceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    Output_terminalContext *output_terminal();
    antlr4::tree::TerminalNode *RPAREN();
    Name_of_instanceContext *name_of_instance();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Pull_gate_instanceContext* pull_gate_instance();

  class  Pulldown_strengthContext : public antlr4::ParserRuleContext {
  public:
    Pulldown_strengthContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    Strength0Context *strength0();
    Strength1Context *strength1();
    antlr4::tree::TerminalNode *COMMA();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Pulldown_strengthContext* pulldown_strength();

  class  Pullup_strengthContext : public antlr4::ParserRuleContext {
  public:
    Pullup_strengthContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    Strength0Context *strength0();
    antlr4::tree::TerminalNode *COMMA();
    Strength1Context *strength1();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Pullup_strengthContext* pullup_strength();

  class  Enable_terminalContext : public antlr4::ParserRuleContext {
  public:
    Enable_terminalContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Enable_terminalContext* enable_terminal();

  class  Inout_terminalContext : public antlr4::ParserRuleContext {
  public:
    Inout_terminalContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Net_lvalueContext *net_lvalue();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Inout_terminalContext* inout_terminal();

  class  Input_terminalContext : public antlr4::ParserRuleContext {
  public:
    Input_terminalContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Input_terminalContext* input_terminal();

  class  Output_terminalContext : public antlr4::ParserRuleContext {
  public:
    Output_terminalContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Net_lvalueContext *net_lvalue();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Output_terminalContext* output_terminal();

  class  Udp_instantiationContext : public antlr4::ParserRuleContext {
  public:
    Udp_instantiationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    Udp_instance_bodyContext *udp_instance_body();
    antlr4::tree::TerminalNode *SEMI();
    Drive_strengthContext *drive_strength();
    Delay2Context *delay2();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<Udp_instanceContext *> udp_instance();
    Udp_instanceContext* udp_instance(size_t i);
    Name_of_instanceContext *name_of_instance();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Udp_instantiationContext* udp_instantiation();

  class  Udp_instanceContext : public antlr4::ParserRuleContext {
  public:
    Udp_instanceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Udp_instance_bodyContext *udp_instance_body();
    Name_of_instanceContext *name_of_instance();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Udp_instanceContext* udp_instance();

  class  Udp_instance_bodyContext : public antlr4::ParserRuleContext {
  public:
    Udp_instance_bodyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    Output_terminalContext *output_terminal();
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<Input_terminalContext *> input_terminal();
    Input_terminalContext* input_terminal(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Udp_instance_bodyContext* udp_instance_body();

  class  Module_or_interface_or_program_or_udp_instantiationContext : public antlr4::ParserRuleContext {
  public:
    Module_or_interface_or_program_or_udp_instantiationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    std::vector<Hierarchical_instanceContext *> hierarchical_instance();
    Hierarchical_instanceContext* hierarchical_instance(size_t i);
    antlr4::tree::TerminalNode *SEMI();
    Parameter_value_assignmentContext *parameter_value_assignment();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Module_or_interface_or_program_or_udp_instantiationContext* module_or_interface_or_program_or_udp_instantiation();

  class  Hierarchical_instanceContext : public antlr4::ParserRuleContext {
  public:
    Hierarchical_instanceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Name_of_instanceContext *name_of_instance();
    antlr4::tree::TerminalNode *LPAREN();
    List_of_port_connectionsContext *list_of_port_connections();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Hierarchical_instanceContext* hierarchical_instance();

  class  List_of_port_connectionsContext : public antlr4::ParserRuleContext {
  public:
    List_of_port_connectionsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Ordered_port_connectionContext *> ordered_port_connection();
    Ordered_port_connectionContext* ordered_port_connection(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<Named_port_connectionContext *> named_port_connection();
    Named_port_connectionContext* named_port_connection(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  List_of_port_connectionsContext* list_of_port_connections();

  class  Ordered_port_connectionContext : public antlr4::ParserRuleContext {
  public:
    Ordered_port_connectionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Ordered_port_connectionContext* ordered_port_connection();

  class  Named_port_connectionContext : public antlr4::ParserRuleContext {
  public:
    Named_port_connectionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *MUL();
    IdentifierContext *identifier();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    Port_replication_connectionContext *port_replication_connection();
    Port_concatenation_connectionContext *port_concatenation_connection();
    Port_expression_connectionContext *port_expression_connection();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Named_port_connectionContext* named_port_connection();

  class  Port_expression_connectionContext : public antlr4::ParserRuleContext {
  public:
    Port_expression_connectionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Port_expression_connectionContext* port_expression_connection();

  class  Port_concatenation_connectionContext : public antlr4::ParserRuleContext {
  public:
    Port_concatenation_connectionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *RBRACE();
    antlr4::tree::TerminalNode *APOSTROPHE_LBRACE();
    antlr4::tree::TerminalNode *LBRACE();
    std::vector<Concatenation_itemContext *> concatenation_item();
    Concatenation_itemContext* concatenation_item(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Port_concatenation_connectionContext* port_concatenation_connection();

  class  Concatenation_itemContext : public antlr4::ParserRuleContext {
  public:
    Concatenation_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Concatenation_itemContext* concatenation_item();

  class  Port_replication_connectionContext : public antlr4::ParserRuleContext {
  public:
    Port_replication_connectionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ReplicationContext *replication();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Port_replication_connectionContext* port_replication_connection();

  class  Bind_directiveContext : public antlr4::ParserRuleContext {
  public:
    Bind_directiveContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_BIND();
    Bind_instantiationContext *bind_instantiation();
    IdentifierContext *identifier();
    Bind_target_instanceContext *bind_target_instance();
    antlr4::tree::TerminalNode *COLON();
    Bind_target_instance_listContext *bind_target_instance_list();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Bind_directiveContext* bind_directive();

  class  Bind_target_instanceContext : public antlr4::ParserRuleContext {
  public:
    Bind_target_instanceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Hierarchical_identifierContext *hierarchical_identifier();
    std::vector<Bit_selectContext *> bit_select();
    Bit_selectContext* bit_select(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Bind_target_instanceContext* bind_target_instance();

  class  Bind_target_instance_listContext : public antlr4::ParserRuleContext {
  public:
    Bind_target_instance_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Bind_target_instanceContext *> bind_target_instance();
    Bind_target_instanceContext* bind_target_instance(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Bind_target_instance_listContext* bind_target_instance_list();

  class  Bind_instantiationContext : public antlr4::ParserRuleContext {
  public:
    Bind_instantiationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Module_or_interface_or_program_or_udp_instantiationContext *module_or_interface_or_program_or_udp_instantiation();
    Checker_instantiationContext *checker_instantiation();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Bind_instantiationContext* bind_instantiation();

  class  Config_declarationContext : public antlr4::ParserRuleContext {
  public:
    Config_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_CONFIG();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    std::vector<antlr4::tree::TerminalNode *> SEMI();
    antlr4::tree::TerminalNode* SEMI(size_t i);
    Design_statementContext *design_statement();
    antlr4::tree::TerminalNode *KW_ENDCONFIG();
    antlr4::tree::TerminalNode *COLON();
    std::vector<Local_parameter_declarationContext *> local_parameter_declaration();
    Local_parameter_declarationContext* local_parameter_declaration(size_t i);
    std::vector<Config_rule_statementContext *> config_rule_statement();
    Config_rule_statementContext* config_rule_statement(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Config_declarationContext* config_declaration();

  class  Design_statementContext : public antlr4::ParserRuleContext {
  public:
    Design_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_DESIGN();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    std::vector<antlr4::tree::TerminalNode *> DOT();
    antlr4::tree::TerminalNode* DOT(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Design_statementContext* design_statement();

  class  Config_rule_statementContext : public antlr4::ParserRuleContext {
  public:
    Config_rule_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_DEFAULT();
    Liblist_clauseContext *liblist_clause();
    Inst_clauseContext *inst_clause();
    Cell_clauseContext *cell_clause();
    Use_clauseContext *use_clause();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Config_rule_statementContext* config_rule_statement();

  class  Inst_clauseContext : public antlr4::ParserRuleContext {
  public:
    Inst_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_INSTANCE();
    Inst_nameContext *inst_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Inst_clauseContext* inst_clause();

  class  Inst_nameContext : public antlr4::ParserRuleContext {
  public:
    Inst_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    std::vector<antlr4::tree::TerminalNode *> DOT();
    antlr4::tree::TerminalNode* DOT(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Inst_nameContext* inst_name();

  class  Cell_clauseContext : public antlr4::ParserRuleContext {
  public:
    Cell_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_CELL();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *DOT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Cell_clauseContext* cell_clause();

  class  Liblist_clauseContext : public antlr4::ParserRuleContext {
  public:
    Liblist_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_LIBLIST();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Liblist_clauseContext* liblist_clause();

  class  Use_clauseContext : public antlr4::ParserRuleContext {
  public:
    Use_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_USE();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    std::vector<Named_parameter_assignmentContext *> named_parameter_assignment();
    Named_parameter_assignmentContext* named_parameter_assignment(size_t i);
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *KW_CONFIG();
    antlr4::tree::TerminalNode *DOT();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Use_clauseContext* use_clause();

  class  Net_aliasContext : public antlr4::ParserRuleContext {
  public:
    Net_aliasContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_ALIAS();
    std::vector<Net_lvalueContext *> net_lvalue();
    Net_lvalueContext* net_lvalue(size_t i);
    antlr4::tree::TerminalNode *SEMI();
    std::vector<antlr4::tree::TerminalNode *> ASSIGN();
    antlr4::tree::TerminalNode* ASSIGN(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Net_aliasContext* net_alias();

  class  Specify_blockContext : public antlr4::ParserRuleContext {
  public:
    Specify_blockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_SPECIFY();
    antlr4::tree::TerminalNode *KW_ENDSPECIFY();
    std::vector<Specify_itemContext *> specify_item();
    Specify_itemContext* specify_item(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Specify_blockContext* specify_block();

  class  Generate_regionContext : public antlr4::ParserRuleContext {
  public:
    Generate_regionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_GENERATE();
    antlr4::tree::TerminalNode *KW_ENDGENERATE();
    std::vector<Generate_itemContext *> generate_item();
    Generate_itemContext* generate_item(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Generate_regionContext* generate_region();

  class  Genvar_expressionContext : public antlr4::ParserRuleContext {
  public:
    Genvar_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Constant_expressionContext *constant_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Genvar_expressionContext* genvar_expression();

  class  Loop_generate_constructContext : public antlr4::ParserRuleContext {
  public:
    Loop_generate_constructContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_FOR();
    antlr4::tree::TerminalNode *LPAREN();
    Genvar_initializationContext *genvar_initialization();
    std::vector<antlr4::tree::TerminalNode *> SEMI();
    antlr4::tree::TerminalNode* SEMI(size_t i);
    Genvar_expressionContext *genvar_expression();
    Genvar_iterationContext *genvar_iteration();
    antlr4::tree::TerminalNode *RPAREN();
    Generate_itemContext *generate_item();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Loop_generate_constructContext* loop_generate_construct();

  class  Genvar_initializationContext : public antlr4::ParserRuleContext {
  public:
    Genvar_initializationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *ASSIGN();
    Constant_expressionContext *constant_expression();
    antlr4::tree::TerminalNode *KW_GENVAR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Genvar_initializationContext* genvar_initialization();

  class  Genvar_iterationContext : public antlr4::ParserRuleContext {
  public:
    Genvar_iterationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    Assignment_operatorContext *assignment_operator();
    Genvar_expressionContext *genvar_expression();
    Inc_or_dec_operatorContext *inc_or_dec_operator();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Genvar_iterationContext* genvar_iteration();

  class  Conditional_generate_constructContext : public antlr4::ParserRuleContext {
  public:
    Conditional_generate_constructContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    If_generate_constructContext *if_generate_construct();
    Case_generate_constructContext *case_generate_construct();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Conditional_generate_constructContext* conditional_generate_construct();

  class  If_generate_constructContext : public antlr4::ParserRuleContext {
  public:
    If_generate_constructContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_IF();
    antlr4::tree::TerminalNode *LPAREN();
    Constant_expressionContext *constant_expression();
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<Generate_itemContext *> generate_item();
    Generate_itemContext* generate_item(size_t i);
    antlr4::tree::TerminalNode *KW_ELSE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  If_generate_constructContext* if_generate_construct();

  class  Case_generate_constructContext : public antlr4::ParserRuleContext {
  public:
    Case_generate_constructContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_CASE();
    antlr4::tree::TerminalNode *LPAREN();
    Constant_expressionContext *constant_expression();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *KW_ENDCASE();
    std::vector<Case_generate_itemContext *> case_generate_item();
    Case_generate_itemContext* case_generate_item(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Case_generate_constructContext* case_generate_construct();

  class  Case_generate_itemContext : public antlr4::ParserRuleContext {
  public:
    Case_generate_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Generate_itemContext *generate_item();
    antlr4::tree::TerminalNode *KW_DEFAULT();
    std::vector<Constant_expressionContext *> constant_expression();
    Constant_expressionContext* constant_expression(size_t i);
    antlr4::tree::TerminalNode *COLON();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Case_generate_itemContext* case_generate_item();

  class  Generate_begin_end_blockContext : public antlr4::ParserRuleContext {
  public:
    Generate_begin_end_blockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_BEGIN();
    antlr4::tree::TerminalNode *KW_END();
    std::vector<antlr4::tree::TerminalNode *> COLON();
    antlr4::tree::TerminalNode* COLON(size_t i);
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    std::vector<Generate_itemContext *> generate_item();
    Generate_itemContext* generate_item(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Generate_begin_end_blockContext* generate_begin_end_block();

  class  Generate_itemContext : public antlr4::ParserRuleContext {
  public:
    Generate_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Module_or_generate_itemContext *module_or_generate_item();
    Extern_tf_declarationContext *extern_tf_declaration();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    antlr4::tree::TerminalNode *KW_RAND();
    Data_declarationContext *data_declaration();
    Generate_regionContext *generate_region();
    Generate_begin_end_blockContext *generate_begin_end_block();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Generate_itemContext* generate_item();

  class  Program_generate_itemContext : public antlr4::ParserRuleContext {
  public:
    Program_generate_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Loop_generate_constructContext *loop_generate_construct();
    Conditional_generate_constructContext *conditional_generate_construct();
    Generate_regionContext *generate_region();
    Elaboration_system_taskContext *elaboration_system_task();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Program_generate_itemContext* program_generate_item();

  class  Module_or_generate_or_interface_or_checker_itemContext : public antlr4::ParserRuleContext {
  public:
    Module_or_generate_or_interface_or_checker_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Function_declarationContext *function_declaration();
    Checker_declarationContext *checker_declaration();
    Property_declarationContext *property_declaration();
    Sequence_declarationContext *sequence_declaration();
    Let_declarationContext *let_declaration();
    Covergroup_declarationContext *covergroup_declaration();
    Genvar_declarationContext *genvar_declaration();
    Clocking_declarationContext *clocking_declaration();
    Initial_constructContext *initial_construct();
    Always_constructContext *always_construct();
    Final_constructContext *final_construct();
    Assertion_itemContext *assertion_item();
    Continuous_assignContext *continuous_assign();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Module_or_generate_or_interface_or_checker_itemContext* module_or_generate_or_interface_or_checker_item();

  class  Module_or_generate_or_interface_itemContext : public antlr4::ParserRuleContext {
  public:
    Module_or_generate_or_interface_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Module_or_interface_or_program_or_udp_instantiationContext *module_or_interface_or_program_or_udp_instantiation();
    antlr4::tree::TerminalNode *SEMI();
    Default_clocking_or_dissable_constructContext *default_clocking_or_dissable_construct();
    Local_parameter_declarationContext *local_parameter_declaration();
    Parameter_declarationContext *parameter_declaration();
    Net_declarationContext *net_declaration();
    Data_declarationContext *data_declaration();
    Task_declarationContext *task_declaration();
    Module_or_generate_or_interface_or_checker_itemContext *module_or_generate_or_interface_or_checker_item();
    Dpi_import_exportContext *dpi_import_export();
    Extern_constraint_declarationContext *extern_constraint_declaration();
    Class_declarationContext *class_declaration();
    Interface_class_declarationContext *interface_class_declaration();
    Class_constructor_declarationContext *class_constructor_declaration();
    Bind_directiveContext *bind_directive();
    Net_aliasContext *net_alias();
    Loop_generate_constructContext *loop_generate_construct();
    Conditional_generate_constructContext *conditional_generate_construct();
    Elaboration_system_taskContext *elaboration_system_task();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Module_or_generate_or_interface_itemContext* module_or_generate_or_interface_item();

  class  Module_or_generate_itemContext : public antlr4::ParserRuleContext {
  public:
    Module_or_generate_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Parameter_overrideContext *parameter_override();
    Gate_instantiationContext *gate_instantiation();
    Udp_instantiationContext *udp_instantiation();
    Module_or_generate_or_interface_itemContext *module_or_generate_or_interface_item();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Module_or_generate_itemContext* module_or_generate_item();

  class  Elaboration_system_taskContext : public antlr4::ParserRuleContext {
  public:
    Elaboration_system_taskContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_DOLAR_FATAL();
    antlr4::tree::TerminalNode *KW_DOLAR_ERROR();
    antlr4::tree::TerminalNode *KW_DOLAR_WARNING();
    antlr4::tree::TerminalNode *KW_DOLAR_INFO();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *UNSIGNED_NUMBER();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *COMMA();
    List_of_argumentsContext *list_of_arguments();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Elaboration_system_taskContext* elaboration_system_task();

  class  Module_item_itemContext : public antlr4::ParserRuleContext {
  public:
    Module_item_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Module_or_generate_itemContext *module_or_generate_item();
    Specparam_declarationContext *specparam_declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Module_item_itemContext* module_item_item();

  class  Module_itemContext : public antlr4::ParserRuleContext {
  public:
    Module_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Generate_regionContext *generate_region();
    Module_item_itemContext *module_item_item();
    std::vector<Attribute_instanceContext *> attribute_instance();
    Attribute_instanceContext* attribute_instance(size_t i);
    Specify_blockContext *specify_block();
    Program_declarationContext *program_declaration();
    Module_declarationContext *module_declaration();
    Interface_declarationContext *interface_declaration();
    Timeunits_declarationContext *timeunits_declaration();
    Nonansi_port_declarationContext *nonansi_port_declaration();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Module_itemContext* module_item();


  bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;

  bool udp_declarationSempred(Udp_declarationContext *_localctx, size_t predicateIndex);
  bool interface_declarationSempred(Interface_declarationContext *_localctx, size_t predicateIndex);
  bool action_blockSempred(Action_blockContext *_localctx, size_t predicateIndex);
  bool seq_blockSempred(Seq_blockContext *_localctx, size_t predicateIndex);
  bool par_blockSempred(Par_blockContext *_localctx, size_t predicateIndex);
  bool conditional_statementSempred(Conditional_statementContext *_localctx, size_t predicateIndex);
  bool rs_if_elseSempred(Rs_if_elseContext *_localctx, size_t predicateIndex);
  bool property_exprSempred(Property_exprContext *_localctx, size_t predicateIndex);
  bool sequence_exprSempred(Sequence_exprContext *_localctx, size_t predicateIndex);
  bool clocking_declarationSempred(Clocking_declarationContext *_localctx, size_t predicateIndex);
  bool covergroup_declarationSempred(Covergroup_declarationContext *_localctx, size_t predicateIndex);
  bool select_expressionSempred(Select_expressionContext *_localctx, size_t predicateIndex);
  bool block_event_expressionSempred(Block_event_expressionContext *_localctx, size_t predicateIndex);
  bool primarySempred(PrimaryContext *_localctx, size_t predicateIndex);
  bool expressionSempred(ExpressionContext *_localctx, size_t predicateIndex);
  bool constraint_expressionSempred(Constraint_expressionContext *_localctx, size_t predicateIndex);
  bool module_declarationSempred(Module_declarationContext *_localctx, size_t predicateIndex);
  bool function_declarationSempred(Function_declarationContext *_localctx, size_t predicateIndex);
  bool task_declarationSempred(Task_declarationContext *_localctx, size_t predicateIndex);
  bool checker_declarationSempred(Checker_declarationContext *_localctx, size_t predicateIndex);
  bool class_declarationSempred(Class_declarationContext *_localctx, size_t predicateIndex);
  bool interface_class_declarationSempred(Interface_class_declarationContext *_localctx, size_t predicateIndex);
  bool package_declarationSempred(Package_declarationContext *_localctx, size_t predicateIndex);
  bool program_declarationSempred(Program_declarationContext *_localctx, size_t predicateIndex);
  bool sequence_declarationSempred(Sequence_declarationContext *_localctx, size_t predicateIndex);
  bool property_declarationSempred(Property_declarationContext *_localctx, size_t predicateIndex);
  bool config_declarationSempred(Config_declarationContext *_localctx, size_t predicateIndex);
  bool if_generate_constructSempred(If_generate_constructContext *_localctx, size_t predicateIndex);
  bool generate_begin_end_blockSempred(Generate_begin_end_blockContext *_localctx, size_t predicateIndex);

  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

}  // namespace mgp_sv
