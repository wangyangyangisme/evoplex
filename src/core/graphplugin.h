/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2017 - Marcos Cardinot <marcos@cardinot.net>
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

#ifndef GRAPHPLUGIN_H
#define GRAPHPLUGIN_H

#include <vector>

#include "enum.h"
#include "plugin.h"

namespace evoplex {
class GraphPlugin : public Plugin
{
    friend class Plugin;
    friend class MainApp;

public:
    using GraphTypes = std::vector<GraphType>;

    virtual ~GraphPlugin() = default;

    inline const GraphTypes& validGraphTypes() const;
    inline bool supportsEdgeAttrsGen() const;

protected:
    explicit GraphPlugin(QPluginLoader* loader, const QString& libPath);

private:
    bool m_supportsEdgeAttrsGen;
    std::vector<GraphType> m_validGraphTypes;
};

/************************************************************************
   GraphPlugin: Inline member functions
 ************************************************************************/

inline const GraphPlugin::GraphTypes& GraphPlugin::validGraphTypes() const
{ return m_validGraphTypes; }

inline bool GraphPlugin::supportsEdgeAttrsGen() const
{ return m_supportsEdgeAttrsGen; }

} //evoplex
#endif // GRAPHPLUGIN_H
