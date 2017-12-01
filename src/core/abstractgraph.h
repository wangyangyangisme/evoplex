/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef ABSTRACT_GRAPH_H
#define ABSTRACT_GRAPH_H

#include "agent.h"
#include "constants.h"
#include "edge.h"
#include "enums.h"
#include "prg.h"

namespace evoplex {

class AbstractBaseGraph
{
    friend class AbstractGraph;
    friend class Experiment;

public:
    inline const QString& name() const { return m_name; }
    inline const GraphType& type() const { return m_type; }
    inline const Attributes* attrs() const { return m_attrs; }
    inline Edges edges() const { return m_edges; }
    inline Agents agents() const { return m_agents; }
    inline Agent* agent(int id) const { return m_agents.at(id); }
    inline Agent* randAgent() const { return m_agents.at(m_prg->randI(m_agents.size())); }

protected:
    Agents m_agents;
    Edges m_edges;

private:
    const QString m_name;
    GraphType m_type;
    Attributes* m_attrs;
    PRG* m_prg;

    explicit AbstractBaseGraph(const QString& name)
        : m_name(name), m_type(Invalid_Type), m_attrs(nullptr), m_prg(nullptr) {}

    virtual ~AbstractBaseGraph() {
        qDeleteAll(m_agents);
        m_agents.clear();
        Agents().swap(m_agents);

        qDeleteAll(m_edges);
        m_edges.clear();
        Edges().swap(m_edges);
    }

    // takes the ownership of the agents
    inline void setup(PRG* prg, Agents& agents, Attributes* attrs) {
        // make sure it'll be called only once
        Q_ASSERT(!m_prg && m_agents.empty());
        m_prg = prg;
        m_agents = agents;
        m_attrs = attrs;
        m_type = (GraphType) attrs->value(PLUGIN_ATTRIBUTE_GRAPH_TYPE).toInt;
    }
};


class AbstractGraph : public AbstractBaseGraph
{
public:
    // constructor
    explicit AbstractGraph(const QString& name)
        : AbstractBaseGraph(name) {}

    // Provide the destructor to keep compilers happy.
    virtual ~AbstractGraph() {}

    // Initializes the graph object.
    // This method is called once when a new graph object is being created.
    // It is usually used to validate the graph attributes and the set of agents.
    virtual bool init() = 0;

    // Resets the graph object to the original state.
    // This method is triggered after a successful init()
    virtual void reset() = 0;
};
} // evoplex

#endif // ABSTRACT_GRAPH_H
