#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QFontMetrics>
#include <QLabel>
#include "rotatelabel.h"
#include <QHeaderView>
#include <QStandardItemModel>

QBrush colorBrush(QColor("#E0FFFF"));
QBrush whiteBrush(QColor("#FFFFFF"));

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _modified(false)
{
    ui->setupUi(this);

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(ui->tableView, SIGNAL(modelChanged()), this, SLOT(onTableViewModelChanged()));

    openDir("./debug");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openDir(const QString &dirname)
{
    //qDebug("dirname: %s", qPrintable(dirname));
    QDir dir(dirname);
    if (!dir.exists()) {
        return;
    }

    int maxWidth = 0;

    dir.setFilter(QDir::Files);
    QFileInfoList list = dir.entryInfoList(QStringList("*.csv"));
    for (auto file : list) {
        QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
        item->setText(file.baseName());
        item->setSizeHint(QSize(0, 30));
        item->setData(Qt::UserRole, QVariant(file.filePath()));

        QFontMetrics fm(item->font());
        int width = fm.width(item->text());
        if (width > maxWidth) {
            maxWidth = width;
        }
    }

    ui->listWidget->setMaximumWidth(maxWidth + 20);

    if (ui->listWidget->count() > 0) {
        ui->listWidget->setCurrentRow(0);
    }
}

void MainWindow::loadCSV(const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly)) {
        return;
    }

    QTextStream ts(&file);
    QString str = ts.readLine();

    if (str.indexOf('\t') >= 0) {
        _sep = '\t';
    } else if (str.indexOf(',') >= 0) {
        _sep = ',';
    } else if (str.indexOf(';') >= 0) {
        _sep = ';';
    } else if (str.indexOf(':') >= 0) {
        _sep = ':';
    } else {
        file.close();
        return;
    }


    QStandardItemModel* model = new QStandardItemModel();
    model->setRowCount(100);
    model->setColumnCount(100);

    int row = 0;
    int col = 0;

    while (!str.isEmpty()) {
        QStringList list = str.split(_sep);
        if (row == 0) {
            model->setHorizontalHeaderLabels(list);
        }
        for (col = 0; col < list.size(); col++) {
            QStandardItem* item = new QStandardItem(list[col]);
            item->setTextAlignment(Qt::AlignCenter);
            model->setItem(row, col, item);
            if (row == 0 && !list[col].isEmpty()) {
                item->setBackground(colorBrush);
            }
        }

        row++;
        str = ts.readLine();
    }
    ui->tableView->setModel(model);
    connect(model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(onTableViewItemChanged(QStandardItem*)));

    file.close();
}

void MainWindow::saveCSV(const QString &path)
{
    QList<QList<QString>> datas;

    QStandardItemModel* model = (QStandardItemModel*)ui->tableView->model();
    for (int i = 0; i < model->rowCount(); i++) {
        for (int j = 0; j < model->columnCount(); j++) {
            QStandardItem* item = model->item(i, j);
            if (item && !item->text().isEmpty()) {
                setArrData(datas, i, j, item->text());
            }
        }
    }

    int colCount = 0;
    for (auto r : datas) {
        colCount = std::max(colCount, r.size());
    }

    QFile file(path);
    if (file.open(QFile::WriteOnly)) {
        QTextStream ts(&file);

        for (int i = 0; i < datas.size(); i++) {
            QList<QString>& row = datas[i];
            for (int j = 0; j < colCount; j++) {
                if (j < row.size()) {
                    ts << row[j];
                }
                if (j < colCount-1) {
                    ts << _sep;
                }
            }
            ts << endl;
        }
    }
}

void MainWindow::setArrData(QList<QList<QString> > &arr, int row, int col, const QString& val)
{
    for (int i = arr.size(); i <= row; i++) {
        arr.append(QList<QString>());
    }

    QList<QString>& rowlist = arr[row];
    for (int i = rowlist.size(); i <= col; i++) {
        rowlist.append("");
    }

    arr[row][col] = val;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (_modified) {
        saveCSV(ui->listWidget->currentItem()->data(Qt::UserRole).toString());
    }
    QMainWindow::closeEvent(event);
}

void MainWindow::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (_modified && previous) {
        saveCSV(previous->data(Qt::UserRole).toString());
    }

    loadCSV(current->data(Qt::UserRole).toString());
    _modified = false;
}

void MainWindow::onTableViewItemChanged(QStandardItem* item)
{
    Q_UNUSED(item);
    _modified = true;

    if (item->row() == 0) {
        QStandardItemModel* model = (QStandardItemModel*)ui->tableView->model();
        if (model) {
            model->setHorizontalHeaderItem(item->column(), new QStandardItem(item->text()));
        }

        if (!item->text().isEmpty()) {
            item->setBackground(colorBrush);
        } else {
            item->setBackground(whiteBrush);
        }
    }
}

void MainWindow::onTableViewModelChanged()
{
    _modified = true;
}

