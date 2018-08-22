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

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSpacerItem>
#include <QToolBar>

#include "experimentwidget.h"
#include "linechart.h"
#include "graphwidget.h"
#include "titlebar.h"

namespace evoplex {

ExperimentWidget::ExperimentWidget(ExperimentPtr exp, MainGUI* mainGUI, ProjectsPage* ppage)
    : PPageDockWidget(ppage)
    , m_kIcon_play(QIcon(":/icons/play.svg"))
    , m_kIcon_pause(QIcon(":/icons/pause.svg"))
    , m_kIcon_next(QIcon(":/icons/next.svg"))
    , m_kIcon_reset(QIcon(":/icons/reset.svg"))
    , m_kIcon_stop(QIcon(":/icons/stop.svg"))
    , m_exp(exp)
    , m_innerWindow(new QMainWindow(this))
    , m_timer(new QTimer)
{
    setFocusPolicy(Qt::StrongFocus);

    setObjectName("ExperimentWidget");
    slotProjectNameChanged(m_exp->project()->name());
    connect(m_exp->project().get(), SIGNAL(nameChanged(QString)),
            SLOT(slotProjectNameChanged(QString)));

    auto titleBar = new TitleBar(this);
    titleBar->setSubtitle("EXPERIMENT");
    titleBar->setTitle(windowTitle());
    setTitleBarWidget(titleBar);

    // setup the inner qmainwindow
    m_innerWindow->setObjectName("experimentWindow");
    m_innerWindow->setDockOptions(QMainWindow::AllowTabbedDocks | QMainWindow::GroupedDragging);
    m_innerWindow->setDockNestingEnabled(true);
    m_innerWindow->setAnimated(true);
    m_innerWindow->setCentralWidget(nullptr);

    QToolBar* tb = new QToolBar("Controls", m_innerWindow);
    tb->setObjectName("Controls");
    m_aPlayPause = tb->addAction(m_kIcon_play, "Play/Pause");
    m_aNext = tb->addAction(m_kIcon_next, "Next step");
    m_aStop = tb->addAction(m_kIcon_stop, "Stop");
    m_aReset = tb->addAction(m_kIcon_reset, "Reset");
    tb->addSeparator();
    m_aGraph = tb->addAction(QIcon(":/icons/graph.svg"), "Graph");
    m_aGrid = tb->addAction(QIcon(":/icons/grid.svg"), "Grid");
    /* FIXME: Disabling LineChart button for now.
     * This feature is in a very unstable state yet.
    m_aLineChart = tb->addAction(QIcon(":/icons/line-chart.svg"), "Line chart");
    */

    QWidget* spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tb->addWidget(spacer);

    m_delay = new QSlider(Qt::Horizontal, this);
    m_delay->setSingleStep(10);
    m_delay->setPageStep(100);
    m_delay->setMaximum(500);
    m_delay->setMinimumWidth(100);
    m_delay->setMaximumWidth(100);
    m_delay->setValue(m_exp->delay());
    m_delay->setToolTip("Delay simulation");
    tb->addWidget(m_delay);

    tb->setMovable(false);
    tb->setFloatable(false);
    tb->setIconSize(QSize(20,20));
    tb->setFocusPolicy(Qt::StrongFocus);

    connect(m_aPlayPause, &QAction::triggered, [this]() { m_exp->toggle(); });
    connect(m_aNext, &QAction::triggered, [this]() { m_exp->playNext(); });
    connect(m_aStop, &QAction::triggered, [this]() { m_exp->stop(); });
    connect(m_aReset, &QAction::triggered, [this]() {
        QString error;
        if (!m_exp->reset(&error)) {
            QMessageBox::warning(this, "Experiment", error);
        }
    });
    connect(m_delay, &QSlider::valueChanged, [this](int v) {
        m_exp->setDelay(static_cast<quint16>(v));
    });

    connect(m_exp.get(), SIGNAL(statusChanged(Status)), SLOT(slotStatusChanged(Status)));
    slotStatusChanged(exp->expStatus()); // just to init the controls

    QVBoxLayout* layout = new QVBoxLayout(new QWidget(this));
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_innerWindow);
    layout->addWidget(tb);
    setWidget(layout->parentWidget());
    auto newGraph = [this, mainGUI](GraphWidget::Mode mode) {
        if (isAutoDeleteOff()) {
            GraphWidget* graph = new GraphWidget(mode, mainGUI->colorMapMgr(), m_exp, this);
            m_innerWindow->addDockWidget(Qt::TopDockWidgetArea, graph);
            connect(graph, SIGNAL(updateWidgets(bool)), this, SIGNAL(updateWidgets(bool)));
            connect(this, SIGNAL(updateWidgets(bool)), graph, SLOT(updateView(bool)));
        }
    };
    connect(m_aGraph, &QAction::triggered,
            [newGraph]() { newGraph(GraphWidget::Mode::Graph); });
    connect(m_aGrid, &QAction::triggered,
            [newGraph]() { newGraph(GraphWidget::Mode::Grid); });

    /* FIXME: Disabling LineChart button for now.
     * This feature is in a very unstable state yet.
    connect(m_aLineChart, &QAction::triggered, [this]() {
        if (isAutoDeleteOff()) {
            LineChart* lineChart = new LineChart(m_exp, this);
            m_innerWindow->addDockWidget(Qt::TopDockWidgetArea, lineChart);
            connect(this, SIGNAL(updateWidgets(bool)), lineChart, SLOT(updateSeries()));
        }
    });
    */

    connect(m_timer, &QTimer::timeout, [this]() {
        if (m_exp->expStatus() != Status::Invalid)
            emit(updateWidgets(false));
    });
    m_timer->start(100);
}

ExperimentWidget::~ExperimentWidget()
{
    delete m_timer;
}

void ExperimentWidget::closeEvent(QCloseEvent* event)
{
    m_exp->disconnect(this); // important to avoid triggering slotStatusChanged()
    m_exp->project()->disconnect(this); // important to avoid setting the windowTitle
    emit (closed());
    QDockWidget::closeEvent(event);
}

void ExperimentWidget::slotProjectNameChanged(const QString& newName)
{
    if (m_exp) {
        setWindowTitle(QString("%1(%2)").arg(newName).arg(m_exp->id()));
    }
}

void ExperimentWidget::slotStatusChanged(Status status)
{
    if (status == Status::Paused || status == Status::Disabled) {
        m_aPlayPause->setIcon(m_kIcon_play);
        m_aPlayPause->setEnabled(true);
        m_aNext->setEnabled(true);
        m_aStop->setEnabled(status != Status::Disabled);
        m_aReset->setEnabled(true);
    } else if (status == Status::Running || status == Status::Queued) {
        m_aPlayPause->setIcon(m_kIcon_pause);
        m_aPlayPause->setEnabled(true);
        m_aNext->setEnabled(false);
        m_aStop->setEnabled(true);
        m_aReset->setEnabled(false);
    } else if (status == Status::Finished || status == Status::Invalid) {
        m_aPlayPause->setIcon(m_kIcon_play);
        m_aPlayPause->setEnabled(false);
        m_aNext->setEnabled(false);
        m_aStop->setEnabled(false);
        m_aReset->setEnabled(true);
    } else {
        qFatal("invalid status!");
    }

    if (status == Status::Invalid) {
        QMessageBox::warning(this, "Experiment", "Something went wrong with your settings!");
    }
}

bool ExperimentWidget::isAutoDeleteOff()
{
    if (m_exp->autoDeleteTrials()) {
        int r = QMessageBox::warning(this, "Experiment",
                    "We cannot open widgets if 'autoDelete' is enabled.\n"
                    "Would you like to disable it for this experiment?",
                    QMessageBox::Yes, QMessageBox::No);
        if (r == QMessageBox::No) {
            return false;
        }
        m_exp->setAutoDeleteTrials(false);
    }
    return true;
}

} // evoplex
