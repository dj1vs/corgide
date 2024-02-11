#pragma once

#include <unordered_map>
#include <string>

enum class Compiler {
    GCC,
    GPP,
    Clang
};

static std::unordered_map <Compiler, std::string> CompilerNames {
    {Compiler::GCC, "GCC"},
    {Compiler::GPP, "G++"},
    {Compiler::Clang, "Clang"}
};
