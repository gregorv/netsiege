/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2015  Gregor Vollmer <gregor@celement.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef SLOTEDITOR_H
#define SLOTEDITOR_H

#include <QtWidgets/QLabel>
#include "ui_sloteditor.h"

class SlotWidget : public QPushButton
{
Q_OBJECT
public:
    SlotWidget(long x, long y);
public slots:
    void setDefined(bool defined);
    void myClicked();
private:
    long m_x;
    long m_y;
    bool m_defined;
};

class SlotEditor : public QDialog
{
Q_OBJECT
public:
    SlotEditor(QWidget* parent);

    void slotToTable(long slot_x, long slot_y, int* column, int* row) const;
    void tableToSlot(int column, int row, long* slot_x, long* slot_y) const;

    void addRow();
    void addColumn();

signals:
    void close();

public slots:
    void ok();
    void apply();
    void cancel();

    void populate();

    void clicked(QAbstractButton* button);

private:
    Ui_SlotEditor ui;
};

#endif // SLOTEDITOR_H
