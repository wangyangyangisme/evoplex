/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QPainter>

#include "core/trial.h"

#include "gridview.h"
#include "ui_graphwidget.h"
#include "ui_graphsettings.h"
#include "utils.h"

namespace evoplex
{

GridView::GridView(MainGUI* mainGUI, ExperimentPtr exp, ExperimentWidget* parent)
    : GraphWidget(mainGUI, exp, parent)
{
    setWindowTitle("Grid");
    //m_settingsDlg->edges->setHidden(true);
    m_ui->bShowNodes->hide();
    m_ui->bShowEdges->hide();
    setTrial(0); // init at trial 0
}

CacheStatus GridView::refreshCache()
{
    if (paintingActive()) {
        return CacheStatus::Scheduled;
    }
    Utils::deleteAndShrink(m_cache);
    if (!m_trial || !m_trial->graph()) {
        return CacheStatus::Ready;
    }

    const Nodes& nodes = m_trial->graph()->nodes();
    m_cache.reserve(nodes.size());

    for (auto const& np : nodes) {
        QRectF r(m_origin.x() + np.second->x() * m_nodeRadius,
                 m_origin.y() + np.second->y() * m_nodeRadius,
                 m_nodeRadius, m_nodeRadius);

        if (!rect().contains(r.x(), r.y()))
            continue;

        Cache c;
        c.node = np.second;
        c.rect = r;
        m_cache.emplace_back(c);
    }
    m_cache.shrink_to_fit();

    return CacheStatus::Ready;
}

void GridView::paintEvent(QPaintEvent*)
{
    if (m_cacheStatus != CacheStatus::Ready) {
        return;
    }

    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    for (const Cache& cache : m_cache) {
        QColor color;
        if (m_selectedNode == cache.node->id()) {
            color = QColor(10,10,10,100);
        } else if (m_nodeAttr >= 0) {
            const Value& value = cache.node->attr(m_nodeAttr);
            color = m_nodeCMap->colorFromValue(value);
        } else {
            color = m_nodeCMap->colors().front();
        }
        painter.setBrush(color);
        painter.setPen(color);
        painter.drawRect(cache.rect);
    }

    painter.end();
}

NodePtr GridView::selectNode(const QPoint& pos) const
{
    if (m_cacheStatus == CacheStatus::Ready) {
        for (const Cache& cache : m_cache) {
            if (cache.rect.contains(pos)) {
                return cache.node;
            }
        }
    }
    return nullptr;
}

}
