/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef PROJECTWIDGET_H
#define PROJECTWIDGET_H

#include <QCloseEvent>
#include <QDockWidget>
#include <QMap>
#include <QMainWindow>

#include "experimentdesigner.h"
#include "projectspage.h"
#include "tablewidget.h"
#include "core/project.h"

class Ui_ProjectWidget;

namespace evoplex {

class ProjectWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit ProjectWidget(MainGUI* mainGUI, Project* project, ProjectsPage* ppage);
    ~ProjectWidget();

    inline Project* project() const { return m_project; }

    void clearSelection();

signals:
    void closed();

protected:
    void closeEvent(QCloseEvent* event);

signals:
    void expSelectionChanged(Experiment* exp);
    void openExperiment(Experiment* exp);
    void hasUnsavedChanges(Project* project);

public slots:
    void slotInsertRow(int expId);
    void slotUpdateRow(int expId);
    void slotHasUnsavedChanges(bool b);

private slots:
    void slotSelectionChanged();
    void onItemDoubleClicked(QTableWidgetItem* item);

private:
    Ui_ProjectWidget* m_ui;
    MainGUI* m_mainGUI;
    Project* m_project;

    QMap<TableWidget::Header, int> m_headerIdx; // map Header to column index

    void fillRow(int row, Experiment* exp);

    void insertItem(int row, TableWidget::Header header, QString label, QString tooltip="");
};
}
#endif // PROJECTWIDGET_H
