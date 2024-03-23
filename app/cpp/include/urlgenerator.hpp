#pragma once

#include <QString>

class UrlGenerator 
{
public:
    UrlGenerator()
    {

    }
    ~UrlGenerator()
    {

    }

    static QString codeforces_problem(const QString &comp, const QString &problem)
    {
        return "https://codeforces.com/contest/" + comp + "/problem/" + problem;
    }
};