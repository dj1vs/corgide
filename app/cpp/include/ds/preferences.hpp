#pragma once

#include "shortcuttype.hpp"

#include <unordered_map>
#include <QString>
#include <QShortcut>
#include <QFont>
#include <QKeySequence>

struct Preferences {
    QString compiler_path;
    QString compiler_args;

    QFont editor_font;

    QMap <ShortcutType, QKeySequence> shortcuts;
};