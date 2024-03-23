#pragma once

class QNetworkAccessManager;
class MyParser;

#include <QObject>

struct CodeforcesProblem {
    QString title;
    QString time_limit;
    QString memory_limit;
    QString statement;
};

class CodeforcesWrapper : public QObject{
    Q_OBJECT;
public:
    CodeforcesWrapper();
    ~CodeforcesWrapper();

public slots:
    void get_problem(QString url);
signals:
    void problem_parsed(CodeforcesProblem);
private:
    CodeforcesProblem parse_problem(const QString &html);
private:
    QNetworkAccessManager *network_access_manager;

    MyParser *parser;

};