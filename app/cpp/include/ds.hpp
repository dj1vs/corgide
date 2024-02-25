#pragma once

#include <unordered_map>
#include <QString>

struct Preferences {
    QString compiler_path;
    QString compiler_args;

    QFont editor_font;
};

