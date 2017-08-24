/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "freezetablewidget.h"

#include <QScrollBar>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QContextMenuEvent>
#include <QMenu>
#include "timecost.h"

//! [constructor]
FreezeTableWidget::FreezeTableWidget(QWidget* parent) : QTableView(parent), frozenTableView(nullptr)
{
    //connect the headers and scrollbars of both tableviews together
    connect(horizontalHeader(),&QHeaderView::sectionResized, this,
            &FreezeTableWidget::updateSectionWidth);
    connect(verticalHeader(),&QHeaderView::sectionResized, this,
            &FreezeTableWidget::updateSectionHeight);
}
//! [constructor]

FreezeTableWidget::~FreezeTableWidget()
{
    delete frozenTableView;
}

void FreezeTableWidget::setModel(QAbstractItemModel *model_)
{
    QTableView::setModel(model_);

    if (!frozenTableView) {
        initFrozenTableView();
    }

    frozenTableView->setModel(model());
    frozenTableView->setSelectionModel(selectionModel());

    for (int col = 2; col < model()->columnCount(); ++col)
        frozenTableView->setColumnHidden(col, true);

    frozenTableView->setColumnWidth(0, columnWidth(0));
    frozenTableView->setColumnWidth(1, columnWidth(1));


    updateFrozenTableGeometry();
}

//! [init part1]
void FreezeTableWidget::initFrozenTableView()
{
    frozenTableView = new QTableView(this);
    frozenTableView->setFocusPolicy(Qt::NoFocus);
    frozenTableView->verticalHeader()->hide();
    frozenTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    viewport()->stackUnder(frozenTableView);
    //! [init part1]

    //! [init part2]
    frozenTableView->setStyleSheet("QTableView { border: none;"
                                   "background-color: #E0FFFF;"
                                   "selection-background-color: #3399ff}"); //for demo purposes

    frozenTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    frozenTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    frozenTableView->show();

    setHorizontalScrollMode(ScrollPerPixel);
    setVerticalScrollMode(ScrollPerPixel);
    frozenTableView->setVerticalScrollMode(ScrollPerPixel);

    connect(frozenTableView->verticalScrollBar(), &QAbstractSlider::valueChanged,
            verticalScrollBar(), &QAbstractSlider::setValue);
    connect(verticalScrollBar(), &QAbstractSlider::valueChanged,
            frozenTableView->verticalScrollBar(), &QAbstractSlider::setValue);
}
//! [init part2]


//! [sections]
void FreezeTableWidget::updateSectionWidth(int logicalIndex, int /* oldSize */, int newSize)
{
    if (logicalIndex == 0){
        frozenTableView->setColumnWidth(0, newSize);
        updateFrozenTableGeometry();
    }
}

void FreezeTableWidget::updateSectionHeight(int logicalIndex, int /* oldSize */, int newSize)
{
    frozenTableView->setRowHeight(logicalIndex, newSize);
}

//! [sections]


//! [resize]
void FreezeTableWidget::resizeEvent(QResizeEvent * event)
{
    QTableView::resizeEvent(event);
    updateFrozenTableGeometry();
}
//! [resize]


//! [navigate]
QModelIndex FreezeTableWidget::moveCursor(CursorAction cursorAction,
                                          Qt::KeyboardModifiers modifiers)
{
    QModelIndex current = QTableView::moveCursor(cursorAction, modifiers);

    if (cursorAction == MoveLeft && current.column() > 0
            && visualRect(current).topLeft().x() < frozenTableView->columnWidth(0) ){
        const int newValue = horizontalScrollBar()->value() + visualRect(current).topLeft().x()
                - frozenTableView->columnWidth(0);
        horizontalScrollBar()->setValue(newValue);
    }
    return current;
}
//! [navigate]

void FreezeTableWidget::scrollTo (const QModelIndex & index, ScrollHint hint){
    if (index.column() > 0)
        QTableView::scrollTo(index, hint);
}

//! [geometry]
void FreezeTableWidget::updateFrozenTableGeometry()
{
    frozenTableView->setGeometry(verticalHeader()->width() + frameWidth(),
                                 frameWidth(), columnWidth(0)+columnWidth(1),
                                 viewport()->height()+horizontalHeader()->height());
}
//! [geometry]

void FreezeTableWidget::contextMenuEvent(QContextMenuEvent * event)
{
    _selectedRow = rowAt(event->pos().y());
    _selectedColumn = columnAt(event->pos().x());

    QMenu* menu = new QMenu(this);

    QAction* act1 = new QAction(QStringLiteral("插入行"), this);
    QAction* act2 = new QAction(QStringLiteral("插入列"), this);
    QAction* act3 = new QAction(QStringLiteral("插入行列"), this);
    QAction* act4 = new QAction(QStringLiteral("删除行"), this);
    QAction* act5 = new QAction(QStringLiteral("删除列"), this);
    QAction* act6 = new QAction(QStringLiteral("删除行列"), this);

    menu->addAction(act1);
    menu->addAction(act2);
    menu->addAction(act3);
    menu->addSeparator();
    menu->addAction(act4);
    menu->addAction(act5);
    menu->addAction(act6);

    connect(act1, SIGNAL(triggered(bool)), this, SLOT(insertRow()));
    connect(act2, SIGNAL(triggered(bool)), this, SLOT(insertColumn()));
    connect(act3, SIGNAL(triggered(bool)), this, SLOT(insertRowColumn()));
    connect(act4, SIGNAL(triggered(bool)), this, SLOT(deleteRow()));
    connect(act5, SIGNAL(triggered(bool)), this, SLOT(deleteColumn()));
    connect(act6, SIGNAL(triggered(bool)), this, SLOT(deleteRowColumn()));

    menu->exec(event->globalPos());

    delete menu;
    delete act1;
    delete act2;
    delete act3;
    delete act4;
    delete act5;
    delete act6;
}

void FreezeTableWidget::insertRow()
{
    QStandardItemModel* m = (QStandardItemModel*)model();
    m->insertRow(_selectedRow, QList<QStandardItem*>());
    emit modelChanged();
}

void FreezeTableWidget::insertColumn()
{
    QStandardItemModel* m = (QStandardItemModel*)model();
    m->insertColumn(_selectedColumn);
    frozenTableView->setColumnHidden(_selectedColumn, true);
    emit modelChanged();
}

void FreezeTableWidget::insertRowColumn()
{
    QStandardItemModel* m = (QStandardItemModel*)model();
    m->insertRow(_selectedRow, nullptr);
    m->insertColumn(_selectedColumn);
    frozenTableView->setColumnHidden(_selectedColumn, true);
    emit modelChanged();
}

void FreezeTableWidget::deleteRow()
{
    QStandardItemModel* m = (QStandardItemModel*)model();
    m->removeRow(_selectedRow);
    emit modelChanged();
}

void FreezeTableWidget::deleteColumn()
{
    QStandardItemModel* m = (QStandardItemModel*)model();
    m->removeColumn(_selectedColumn);
    emit modelChanged();
}

void FreezeTableWidget::deleteRowColumn()
{
    QStandardItemModel* m = (QStandardItemModel*)model();
    m->removeRow(_selectedRow);
    m->removeColumn(_selectedColumn);
    emit modelChanged();
}
