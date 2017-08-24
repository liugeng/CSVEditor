#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QList>

namespace Ui {
class MainWindow;
}

class QStandardItem;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void onTableViewItemChanged(QStandardItem* item);
    void onTableViewModelChanged();

private:
    void openDir(const QString& dir);
    void loadCSV(const QString& path);
    void saveCSV(const QString& path);
    void setArrData(QList<QList<QString>>& arr, int row, int col, const QString& val);

    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui;
    QString _sep;
    bool _modified;
};

#endif // MAINWINDOW_H
