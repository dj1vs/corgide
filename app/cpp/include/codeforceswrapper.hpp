#pragma once

class QNetworkAccessManager;
class MyParser;

#include <QObject>

typedef struct {
    QString title;
    QString time_limit;
    QString memory_limit;
} CodeforcesProblem;

class CodeforcesWrapper : public QObject{
    Q_OBJECT;
public:
    CodeforcesWrapper();
    ~CodeforcesWrapper();
signals:
    void problem_parsed(CodeforcesProblem);
private:
    QNetworkAccessManager *network_access_manager;

    MyParser *parser;

};