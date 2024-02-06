#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>

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
    connect(ui->open_file_action, &QAction::triggered, this, &MainWindow::open_file);
    connect(ui->save_file_action, &QAction::triggered, this, &MainWindow::save_file);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::open_new_tab() {
    CodeEditor *editor = new CodeEditor(this);

    auto *highlighter = new QSourceHighlite::QSourceHighliter(editor->document());
    highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeCpp);

    ui->tab_widget->addTab(editor, "New");
}

void MainWindow::close_tab() {
    if (ui->tab_widget->count()) {
        ui->tab_widget->removeTab(ui->tab_widget->currentIndex());
    }
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