#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class CodeEditor;
class QFileSystemModel;
class QAction;
class QMenu;
class QShowEvent;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void open_new_tab();
    void close_tab(int index);
    void close_current_tab();
    void next_tab();
    void prev_tab();

    void ask_open_file();
    void ask_save_file();

    void ask_create_file();
    void ask_delete_file();

    void ask_open_folder();

    void open_folder_file();

    void show_folder_context_menu(const QPoint &point) const;
private:
    void open_file(const QString &file_name);
    void save_file(const QString &file_name);

    void create_file(const QString &file_name);
    void delete_file(const QString &file_path);

    void open_folder(const QString &folder_name);

    QString get_opened_folder() const;

    void write_settings();
    void read_settings();

    void setup_folder_context_menu();

private:
    Ui::MainWindow *ui;

    QFileSystemModel *fs_model;

    QMenu *folder_context_menu;
    QAction *folder_open_file_action;
    QAction *folder_create_file_action;
    QAction *folder_delete_file_action;

    bool is_folder_opened = false;
};
#endif // MAINWINDOW_H
