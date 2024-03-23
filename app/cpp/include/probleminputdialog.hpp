#pragma once

#include "ui_probleminputdialog.h"

#include <QDialog>

#include "urlgenerator.hpp"

class ProblemInputDialog : public QDialog 
{
    Q_OBJECT
public:
    ProblemInputDialog(QDialog *parent = nullptr)
        :QDialog(parent), ui(new Ui::ProblemInputDialog)
    {
        ui->setupUi(this);
    }
    ~ProblemInputDialog()
    {
        delete ui;
    }

    QString get_url() const
    {
        switch (ui->select_method->currentIndex())
        {
            case 0:
                return url_from_comp();
            case 1:
                return ui->plain_url->text();
            
        }

        return "";
    }
private slots:
    void switch_selection_w(int index)
    {
        ui->selection_w->setCurrentIndex(index);
    }
private:
    QString url_from_comp() const
    {
        const QString comp = ui->from_comp_comp->text();
        const QString problem = ui->from_comp_problem->text();

        switch (ui->from_comp_source->currentIndex())
        {
            case 0:
                return UrlGenerator::codeforces_problem(comp, problem);
        }

        return "";
    }
private:
    Ui::ProblemInputDialog *ui;
};