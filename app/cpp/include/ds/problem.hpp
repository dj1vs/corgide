#pragma once

#include <QString>
#include <QVector>

struct ProblemExample;

struct Problem
{
public:
    QString title;
    int32_t time_limit_ms;
    QString memory_limit;
    QString source_size_limit;
    QString input_file;
    QString output_file;
    QString description;
    QString input_description;
    QString output_description;
    QVector<ProblemExample> examples;
    QString note;
    QString author;
    QStringList tags;
    QString difficulty;
    int32_t total_attempts;
    int32_t total_completed;
    QString date_created;
    QStringList languages;
};

struct ProblemExample
{
    QString input;
    QString output;
};