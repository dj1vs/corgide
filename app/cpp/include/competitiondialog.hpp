#pragma once

#include "ui_competitiondialog.h"

#include <QDialog>

class CompetitionDialog : public QDialog
{
    Q_OBJECT
public:
    CompetitionDialog(QDialog *parent = nullptr)
        :QDialog(parent), ui(new Ui::CompetitionDialog)
    {
        ui->setupUi(this);
    }

    ~CompetitionDialog()
    {
        delete ui;
    }

    QString get_url() const
    {
        const int selection_ind = ui->selection_variant->currentIndex();

        switch (selection_ind)
        {
            case 0:
                return ui->plain_url_url->text();
            case 1:
                return get_url_from_platform_and_title();
        }
    }
private slots:
    void switch_selection_w(int index)
    {
        ui->selection_w->setCurrentIndex(index);
    }
private:
    QString get_url_from_platform_and_title() const
    {
        const int platform_ind = ui->platform_and_title_platform->currentIndex();
        const QString title = ui->platfrom_and_title_title->text();

        switch (platform_ind)
        {
            case 0: // codeforces
                return "https://codeforces.com/contest/" + title;
        }
    }
private:
    Ui::CompetitionDialog *ui;
    
};