cmake_minimum_required(VERSION 3.15)  # Минимальная версия CMake
project(calculator C)  # Явно указываем язык проекта

set(CMAKE_C_STANDARD 11)

# Флаги оптимизации для C (не C++!)
set(MY_OPTIMIZATION_FLAGS
        -O3
        -march=native
        -flto
        -ffast-math
        -funroll-loops
        -fomit-frame-pointer
)

# Для проектов на C используем CheckCCompilerFlag вместо CXX
include(CheckCCompilerFlag)  # <- ИЗМЕНИЛИ ЗДЕСЬ
check_c_compiler_flag(-flto SUPPORTS_LTO)  # <- И ЗДЕСЬ

if(SUPPORTS_LTO)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# Устанавливаем Release режим
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

add_executable(calculator main.c
        EnumTypeMaps.h
        Definitions.h
        ArcGamma.c
        ArcGamma.h
        Enums.h
        Enums.c
        MyMath.h
        MyMath.c
        Utils.h
        Utils.c
        Murmur3.h
        Murmur3.c
        Error.h
        Error.c
        Lexer.h
        Lexer.c
        RBT.h
        RBT.c
        LexerToken.h
        LexerToken.c
        AST.h
        AST.c
        Parser.h
        Parser.c
        ASTNodePull.h
        ASTNodePull.c
        Globals.h
        Globals.c
        ASTEval.h
        ASTEval.c)

# Для C используем target_compile_options с C флагами
target_compile_options(calculator PRIVATE
        ${MY_OPTIMIZATION_FLAGS}
        -DNDEBUG
)

target_link_options(calculator PRIVATE
        ${MY_OPTIMIZATION_FLAGS}
)