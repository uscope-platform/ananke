set(MGP_SV_PARSER_LIB_SRCS /
        sv2017Lexer.cpp /
        sv2017.cpp /
        sv2017BaseListener.cpp /
        sv2017BaseVisitor.cpp /
        sv2017Listener.cpp /
        sv2017Visitor.cpp )

set(MGP_SV_PARSER_LIB_INCL /
        sv2017Lexer.h /
        sv2017.h /
        sv2017BaseListener.h /
        sv2017BaseVisitor.h /
        sv2017Listener.h /
        sv2017Visitor.h
        ../data_model/HDL/HDL_loop.hpp
        ../data_model/Depfile/depfile_validator.hpp)

add_library(mgp_sv_lib STATIC ${MGP_SV_PARSER_LIB_SRCS} ${MGP_SV_PARSER_LIB_INCL})

set_property(TARGET mgp_sv_lib PROPERTY INTERPROCEDURAL_OPTIMIZATION True)

target_link_libraries(mgp_sv_lib antlr4_static)
set_property(TARGET mgp_sv_lib PROPERTY POSITION_INDEPENDENT_CODE 1)

