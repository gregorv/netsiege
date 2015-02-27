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

#include "terrainlayerlistmodel.h"
#include <qt/QtCore/QVariant>

TerrainLayerListModel::TerrainLayerListModel(QObject* parent)
 : QAbstractListModel(parent)
{

}

TerrainLayerListModel::~TerrainLayerListModel()
{

}

size_t TerrainLayerListModel::newLayer()
{
    QModelIndex idx;
    beginInsertRows(idx, m_layers.size(), m_layers.size());
    m_layers.resize(m_layers.size()+1);
    auto& layer = m_layers[m_layers.size()-1];
    layer.setName("New Layer 1337");
    endInsertRows();
    return m_layers.size()-1;
}

void TerrainLayerListModel::deleteLayer(size_t idx)
{
    m_layers.erase(m_layers.begin()+idx);
}

QVariant TerrainLayerListModel::data(const QModelIndex& index, int role) const
{
    if(index.row() < rowCount(index) && role == Qt::DisplayRole)
        return QVariant(m_layers[index.row()].name().c_str());
    else
        return QVariant::Invalid;
    return QVariant::Invalid;
}

// int TerrainLayerListModel::columnCount(const QModelIndex& parent) const
// {
//     return 1;
// }

int TerrainLayerListModel::rowCount(const QModelIndex& parent) const
{
    return m_layers.size();
}

const TerrainLayer& TerrainLayerListModel::layer(const QModelIndex& idx) const
{
    if(idx.row() < rowCount(idx))
        return m_layers[idx.row()];
    return m_layers[0];
}

// QModelIndex TerrainLayerListModel::parent(const QModelIndex& child) const
// {
//
// }
