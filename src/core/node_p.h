/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2016 - Marcos Cardinot <marcos@cardinot.net>
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

#ifndef NODE_P_H
#define NODE_P_H

#include <memory>

#include "attributes.h"
#include "edges.h"
#include "prg.h"

namespace evoplex {

class BaseNode;
using NodePtr = std::shared_ptr<BaseNode>;

class NodeInterface
{
    friend class AbstractGraph;
    friend class Nodes;
    friend class TestNode;
    friend class TestEdge;

public:
    virtual ~NodeInterface() = default;
    virtual NodePtr clone() const = 0;
    virtual const Edges& inEdges() const = 0;
    virtual const Edges& outEdges() const = 0;
    virtual int degree() const = 0;
    virtual int inDegree() const = 0;
    virtual int outDegree() const = 0;

private:
    virtual void addInEdge(const Edge& inEdge) = 0;
    virtual void addOutEdge(const Edge& outEdge) = 0;
    virtual void removeInEdge(const int edgeId) = 0;
    virtual void removeOutEdge(const int edgeId) = 0;
    virtual void clearInEdges() = 0;
    virtual void clearOutEdges() = 0;
};

/**
 * @brief A node belongs to either a directed or undirected graph.
 *        Directed graphs are composed of DNode objects, while an undirected
 *        graph is composed of UNode objects.
 *
 * @attention A node can only be created by an AbstractGraph derived object
 *            or from a Nodes container.
 */
class BaseNode : public NodeInterface
{
    friend class AbstractGraph;
    friend class NodesPrivate;
    friend class TestNode;
    friend class TestEdge;

public:
    inline const Attributes& attrs() const;
    inline const Value& attr(int id) const;
    inline Value attr(const QString& name, Value defaultValue=Value()) const;
    inline void setAttr(int id, const Value& value);

    inline int id() const;
    inline float x() const;
    inline float y() const;

    inline void setX(float x);
    inline void setY(float y);
    inline void setCoords(float x, float y);

    Node randNeighbour(PRG* prg) const;

protected:
    Edges m_outEdges;

    struct constructor_key { /* this is a private key accessible only to friends */ };

    explicit BaseNode(const constructor_key&, int id, const Attributes& attrs, float x, float y);
    explicit BaseNode(const constructor_key& k, int id, const Attributes& attr);
    ~BaseNode() override;

private:
    const int m_id;
    Attributes m_attrs;
    float m_x;
    float m_y;
};

class UNode : public BaseNode
{
public:
    explicit UNode(const constructor_key& k, int id, const Attributes& attrs, float x, float y);
    explicit UNode(const constructor_key& k, int id, const Attributes& attrs);
    ~UNode() override = default;

    inline NodePtr clone() const override;
    inline const Edges& inEdges() const override;
    inline const Edges& outEdges() const override;
    inline int degree() const override;
    inline int inDegree() const override;
    inline int outDegree() const override;

private:
    inline void addInEdge(const Edge& inEdge) override;
    inline void addOutEdge(const Edge& outEdge) override;
    inline void removeInEdge(const int edgeId) override;
    inline void removeOutEdge(const int edgeId) override;
    inline void clearInEdges() override;
    inline void clearOutEdges() override;
};

class DNode : public BaseNode
{
public:
    explicit DNode(const constructor_key& k, int id, const Attributes& attrs, float x, float y);
    explicit DNode(const constructor_key& k, int id, const Attributes& attrs);
    ~DNode() override = default;

    inline NodePtr clone() const override;
    inline const Edges& inEdges() const override;
    inline const Edges& outEdges() const override;
    inline int degree() const override;
    inline int inDegree() const override;
    inline int outDegree() const override;

private:
    Edges m_inEdges;

    inline void addInEdge(const Edge& inEdge) override;
    inline void addOutEdge(const Edge& outEdge) override;
    inline void removeInEdge(const int edgeId) override;
    inline void removeOutEdge(const int edgeId) override;
    inline void clearInEdges() override;
    inline void clearOutEdges() override;
};

/************************************************************************
   BaseNode: Inline member functions
 ************************************************************************/

inline const Attributes& BaseNode::attrs() const
{ return m_attrs; }

inline const Value& BaseNode::attr(int id) const
{ return m_attrs.value(id); }

inline Value BaseNode::attr(const QString& name, Value defaultValue) const
{ return m_attrs.value(name, defaultValue); }

inline void BaseNode::setAttr(int id, const Value& value)
{ m_attrs.setValue(id, value); }

inline int BaseNode::id() const
{ return m_id; }

inline float BaseNode::x() const
{ return m_x; }

inline void BaseNode::setX(float x)
{ m_x = x; }

inline float BaseNode::y() const
{ return m_y; }

inline void BaseNode::setY(float y)
{ m_y = y; }

inline void BaseNode::setCoords(float x, float y)
{ setX(x); setY(y); }

/************************************************************************
   UNode: Inline member functions
 ************************************************************************/

inline NodePtr UNode::clone() const
{ return std::make_shared<UNode>(constructor_key(), id(), attrs(), x(), y()); }

inline const Edges& UNode::inEdges() const
{ return m_outEdges; }

inline const Edges& UNode::outEdges() const
{ return m_outEdges; }

inline int UNode::degree() const
{ return static_cast<int>(m_outEdges.size()); }

inline int UNode::inDegree() const
{ return degree(); }

inline int UNode::outDegree() const
{ return degree(); }

inline void UNode::addInEdge(const Edge& inEdge)
{ addOutEdge(inEdge); }

inline void UNode::addOutEdge(const Edge& outEdge)
{ m_outEdges.insert({outEdge.id(), outEdge}); }

inline void UNode::removeInEdge(const int edgeId)
{ removeOutEdge(edgeId); }

inline void UNode::removeOutEdge(const int edgeId)
{ m_outEdges.erase(edgeId); }

inline void UNode::clearInEdges()
{ clearOutEdges(); }

inline void UNode::clearOutEdges()
{ m_outEdges.clear(); }

/************************************************************************
   DNode: Inline member functions
 ************************************************************************/

NodePtr DNode::clone() const
{ return std::make_shared<DNode>(constructor_key(), id(), attrs(), x(), y()); }

inline const Edges& DNode::inEdges() const
{ return m_inEdges; }

inline const Edges& DNode::outEdges() const
{ return m_outEdges; }

inline int DNode::degree() const
{ return inDegree() + outDegree(); }

inline int DNode::inDegree() const
{ return static_cast<int>(m_inEdges.size()); }

inline int DNode::outDegree() const
{ return static_cast<int>(m_outEdges.size()); }

inline void DNode::addInEdge(const Edge& inEdge)
{ m_inEdges.insert({inEdge.id(), inEdge}); }

inline void DNode::addOutEdge(const Edge& outEdge)
{ m_outEdges.insert({outEdge.id(), outEdge}); }

inline void DNode::removeInEdge(const int edgeId)
{ m_inEdges.erase(edgeId); }

inline void DNode::removeOutEdge(const int edgeId)
{ m_outEdges.erase(edgeId); }

inline void DNode::clearInEdges()
{ m_inEdges.clear(); }

inline void DNode::clearOutEdges()
{ m_outEdges.clear(); }

} // evoplex
#endif // NODE_P_H
