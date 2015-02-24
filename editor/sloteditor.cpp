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

#include "sloteditor.h"
#include "campaignmanager.h"
#include "undocommands.h"
#include "ogrebase.h"
#include <sstream>


SlotWidget::SlotWidget(long int x, long int y): QPushButton(), m_x(x), m_y(y), m_defined(false)
{
    connect(this, &SlotWidget::clicked, this, &SlotWidget::myClicked);
    std::ostringstream stream;
    stream << m_x << " " << m_y;
    setText(stream.str().c_str());
}

void SlotWidget::setDefined(bool defined)
{
    m_defined = defined;
    if(m_defined) {
        std::ostringstream stream;
        stream << "X" << m_x << " " << m_y << "X";
        setText(stream.str().c_str());
    } else {
        std::ostringstream stream;
        stream << m_x << " " << m_y;
        setText(stream.str().c_str());
    }
}

void SlotWidget::myClicked()
{
    setDefined(!m_defined);
    auto command = new undo::ModifyTerrainSlot(m_defined, m_x, m_y);
    OgreBase::getSingleton().getStack()->push(command);
}




SlotEditor::SlotEditor(QWidget* parent)
 : QDialog(parent)
{
    ui.setupUi(this);
    connect(ui.buttons, &QDialogButtonBox::clicked, this, &SlotEditor::clicked);
    connect(ui.addColumnButton, &QPushButton::clicked, this, &SlotEditor::addColumn);
    connect(ui.addRowButton, &QPushButton::clicked, this, &SlotEditor::addRow);
}

void SlotEditor::slotToTable(long int slot_x, long int slot_y, int* column, int* row) const
{
    auto campaign = CampaignManager::getOpenDocument();
    auto bound = campaign->getSlotBoundary();
    *column = slot_x - bound.left + (ui.slotTable->columnCount() - bound.right + bound.left)/2;
    *row = slot_y - bound.top + (ui.slotTable->rowCount() - bound.bottom + bound.top)/2;
//     std::cout << slot_x << "->" << *column << "; " << slot_y << "->" << *row << std::endl;
}

void SlotEditor::tableToSlot(int column, int row, long int* slot_x, long int* slot_y) const
{
    auto campaign = CampaignManager::getOpenDocument();
    auto bound = campaign->getSlotBoundary();
    *slot_x = column + bound.left - (ui.slotTable->columnCount() - bound.right + bound.left)/2;
    *slot_y = row + bound.top - (ui.slotTable->rowCount() - bound.bottom + bound.top)/2;
//     std::cout << slot_x << "->" << *column << "; " << slot_y << "->" << *row << std::endl;
}

void SlotEditor::addColumn()
{
    int insertIdx = 0;
    if(ui.slotTable->columnCount() % 2 == 1) {
        insertIdx = ui.slotTable->columnCount();
    }
    ui.slotTable->insertColumn(insertIdx);
    for(size_t i=0; i<ui.slotTable->rowCount(); i++) {
        long slot_x, slot_y;
        tableToSlot(insertIdx, i, &slot_x, &slot_y);
        auto widget = new SlotWidget(slot_x, slot_y);
        ui.slotTable->setCellWidget(i, insertIdx, widget);
    }
}

void SlotEditor::addRow()
{
    int insertIdx = 0;
    if(ui.slotTable->rowCount() % 2 == 1) {
        insertIdx = ui.slotTable->rowCount();
    }
    ui.slotTable->insertRow(insertIdx);
    for(size_t i=0; i<ui.slotTable->columnCount(); i++) {
        long slot_x, slot_y;
        tableToSlot(i, insertIdx, &slot_x, &slot_y);
        auto widget = new SlotWidget(slot_x, slot_y);
        ui.slotTable->setCellWidget(insertIdx, i, widget);
    }
}

void SlotEditor::ok()
{
    apply();
    cancel();
}
void SlotEditor::apply()
{
}

void SlotEditor::cancel()
{
    hide();
    emit close();
}

void SlotEditor::populate()
{
    ui.slotTable->clear();
    auto campaign = CampaignManager::getOpenDocument();
    auto bound = campaign->getSlotBoundary();
    ui.slotTable->setColumnCount(bound.right - bound.left + 1);
    ui.slotTable->setRowCount(bound.bottom - bound.top + 1);
    for(size_t col=0; col < ui.slotTable->columnCount(); col++) {
        for(size_t row=0; row < ui.slotTable->rowCount(); row++) {
            long slot_x, slot_y;
            tableToSlot(col, row, &slot_x, &slot_y);
            auto widget = new SlotWidget(slot_x, slot_y);
            ui.slotTable->setCellWidget(row, col, widget);
        }
    }
    long x, y;
    for(auto idx: campaign->getDefinedSlots()) {
        campaign->unpackSlotIndex(idx, &x, &y);
        int row, column;
        slotToTable(x, y, &column, &row);
        auto widget = dynamic_cast<SlotWidget*>(ui.slotTable->cellWidget(row, column));
        if(widget) {
            widget->setDefined(true);
        }
    }
}

void SlotEditor::clicked(QAbstractButton* button)
{
    auto clickedButton = ui.buttons->standardButton(button);
    if(clickedButton == QDialogButtonBox::Ok) {
        ok();
    } else if(clickedButton == QDialogButtonBox::Apply) {
        apply();
    } else if(clickedButton == QDialogButtonBox::Close) {
        close();
    }
}

