#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class CodeEditor;
class QAction;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void open_new_tab();
    void close_tab();

    void open_file();
    void save_file();

private:
    Ui::MainWindow *ui;

    QAction *new_tab_action;
    QAction *open_file_action;
    QAction *save_file_action;
};
#endif // MAINWINDOW_H
