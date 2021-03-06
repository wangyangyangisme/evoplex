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

#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QDockWidget>

#include "core/experiment.h"

#include "experimentwidget.h"
#include "maingui.h"

namespace evoplex {

class BaseGraphGL;

class GraphWidget : public QDockWidget
{
    Q_OBJECT

public:
    enum class Mode {
        Graph,
        Grid
    };

    explicit GraphWidget(Mode mode, ColorMapMgr* cMgr,
                         ExperimentPtr exp, ExperimentWidget* parent);

    virtual ~GraphWidget();

    inline BaseGraphGL* view() const;

public slots:
    void updateView(bool forceUpdate);

signals:
    void updateWidgets(bool);

private:
    BaseGraphGL* m_view;
};

inline BaseGraphGL* GraphWidget::view() const
{ return m_view; }

} // evoplex
#endif // GRAPHWIDGET_H
