#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <algorithm>

#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QRegularExpression>

#include "qsourcehighliter.h"

#include "codeeditor.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    ui->tab_widget->setTabsClosable(true);

    connect(ui->tab_widget, &QTabWidget::tabCloseRequested, this, &MainWindow::close_tab);
    
    connect(ui->new_tab_action,   &QAction::triggered, this, &MainWindow::open_new_tab);
    connect(ui->close_tab_action, &QAction::triggered, this, &MainWindow::close_tab);
    connect(ui->next_tab_action, &QAction::triggered, this, &MainWindow::next_tab);
    connect(ui->prev_tab_action, &QAction::triggered, this, &MainWindow::prev_tab);
    connect(ui->open_file_action, &QAction::triggered, this, &MainWindow::open_file);
    connect(ui->save_file_action, &QAction::triggered, this, &MainWindow::save_file);

    open_new_tab();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::open_new_tab() {
    CodeEditor *editor = new CodeEditor(this);

    auto *highlighter = new QSourceHighlite::QSourceHighliter(editor->document());
    highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeCpp);

    QRegularExpression untitled_regexp("Untitled (^\\d+$)");
    QVector<int> untitled_numbers;
    const int tabs_amount = ui->tab_widget->count();
    for (int i = 0; i < tabs_amount; ++i) {
        const QString tab_name = ui->tab_widget->tabText(i);
        const auto untitled_regexp_match = untitled_regexp.match(tab_name);
        if (!untitled_regexp_match.isValid()) {
            continue;
        }

        const qsizetype tab_name_begin = tab_name.indexOf('(') + 1;
        const qsizetype tab_name_end = tab_name.indexOf(')') - 1;
        const QString tab_number = tab_name.mid(tab_name_begin, tab_name_end - tab_name_begin + 1);
        untitled_numbers.push_back(tab_number.toInt());
    }

    std::sort(untitled_numbers.begin(), untitled_numbers.end());
    int new_tab_number = -1;
    for (qsizetype i = 1; i < untitled_numbers.size(); ++i) {
        if (untitled_numbers[i] != untitled_numbers[i-1] + 1) {
            new_tab_number = untitled_numbers[i-1] + 1;
            break;
        }
    }

    if (new_tab_number == -1 && untitled_numbers.size()) {
        new_tab_number = untitled_numbers.back() + 1;
    }

    QString new_tab_name = "Untitled";
    if (new_tab_number != -1) {
        new_tab_name += " (" + QString::number(new_tab_number) + ')';
    }

    ui->tab_widget->addTab(editor, new_tab_name);
    ui->tab_widget->setCurrentIndex(ui->tab_widget->count() - 1); // go to new tab
}

void MainWindow::close_tab(int index) {
    if (ui->tab_widget->count()) {
        ui->tab_widget->removeTab(index);
    }
}

void MainWindow::next_tab() {
    const int cur_ind = ui->tab_widget->currentIndex();
    
    int next_ind = cur_ind + 1;
    if (next_ind == ui->tab_widget->count()) {
        next_ind = 0;
    }

    ui->tab_widget->setCurrentIndex(next_ind);
}

void MainWindow::prev_tab() {
    const int cur_ind = ui->tab_widget->currentIndex();
    
    int next_ind = cur_ind - 1;
    if (next_ind == -1) {
        next_ind = ui->tab_widget->count() - 1;
    }

    ui->tab_widget->setCurrentIndex(next_ind);
}

void MainWindow::open_file() {
    QString file_name_filter = "";
    const QString file_name = QFileDialog::getOpenFileName(this, "Choose file to open", "", "", &file_name_filter);
    if (!file_name.length()) {
        QMessageBox::warning(this, "Warning", "File was not selected", QMessageBox::Ok);
        return;
    } 

    QFile file(file_name);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Can't open file", QMessageBox::Ok);
        return;
    }

    QString file_contents = file.readAll();

    if (!ui->tab_widget->count()) {
        open_new_tab();
    }

    auto cur_editor = dynamic_cast<CodeEditor*>(ui->tab_widget->currentWidget());

    cur_editor->setPlainText(file_contents);
    cur_editor->set_file_name(file.fileName());

    const int cur_tab_ind = ui->tab_widget->currentIndex();
    ui->tab_widget->setTabText(cur_tab_ind, QFileInfo(file.fileName()).fileName());
}

void MainWindow::save_file() {
    QString file_name;

    auto cur_editor = dynamic_cast<CodeEditor*>(ui->tab_widget->currentWidget());
    if (cur_editor->get_file_name().has_value()) {
        file_name = cur_editor->get_file_name().value();
    } else {
        QString file_name_filter = "";
        file_name = QFileDialog::getSaveFileName(this, "Chose file save path", "", "", &file_name_filter);
        if (!file_name.length()) {
            QMessageBox::warning(this, "Warning", "File was not selected", QMessageBox::Ok);
            return;
        }
    }

    QFile file(file_name);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Error", "Can't open file", QMessageBox::Ok);
        return;
    }

    file.write(dynamic_cast<QPlainTextEdit*>(ui->tab_widget->currentWidget())->toPlainText().toUtf8());
}