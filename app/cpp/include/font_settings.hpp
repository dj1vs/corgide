#pragma once

#include <QSettings>
#include <QFont>

static void write_font_to_settings(const QString &section_name, const QFont &font, QSettings &settings)
{
    settings.beginGroup(section_name);

    settings.setValue("font_family", font.family());
    settings.setValue("font_size",   font.pointSize());
    settings.setValue("bold",        font.bold());
    settings.setValue("italic",      font.italic());
    settings.setValue("strikeout",   font.strikeOut());
    settings.setValue("underline",   font.underline());

    settings.endGroup();
}

static QFont read_font_from_settings(const QString &section_name, QSettings &settings, const QFont &default_font = QFont())
{
    QFont font;

    settings.beginGroup(section_name);

    font.setFamily   (settings.value("font_family", default_font.family()   ).toString());
    font.setPointSize(settings.value("font_size",   default_font.pointSize()).toInt());
    font.setBold     (settings.value("bold",        default_font.bold()     ).toBool());
    font.setItalic   (settings.value("italic",      default_font.italic()   ).toBool());
    font.setStrikeOut(settings.value("strikeout",   default_font.strikeOut()).toBool());
    font.setUnderline(settings.value("underline",   default_font.underline()).toBool());    

    settings.endGroup();

    return font;
}