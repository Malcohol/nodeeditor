#include "AbstractGraphModel.hpp"

#include <QtNodes/ConnectionIdUtils>

namespace QtNodes {

QJsonObject AbstractGraphModel::saveNode(NodeId const nodeId) const {
    QJsonObject nodeJson;
    nodeJson["id"] = static_cast<qint64>(nodeId);
    return nodeJson;
}

void AbstractGraphModel::portsAboutToBeDeleted(NodeId const nodeId,
                                               PortType const portType,
                                               PortIndex const first,
                                               PortIndex const last)
{
    _shiftedByDynamicPortsConnections.clear();

    auto portCountRole = portType == PortType::In ? NodeRole::InPortCount : NodeRole::OutPortCount;

    unsigned int portCount = nodeData(nodeId, portCountRole).toUInt();

    if (first > portCount - 1)
        return;

    if (last < first)
        return;

    auto clampedLast = std::min(last, portCount - 1);

    for (PortIndex portIndex = first; portIndex <= clampedLast; ++portIndex) {
        std::unordered_set<ConnectionId> conns = connections(nodeId, portType, portIndex);

        for (auto connectionId : conns) {
            deleteConnection(connectionId);
        }
    }

    std::size_t const nRemovedPorts = clampedLast - first + 1;

    for (PortIndex portIndex = clampedLast + 1; portIndex < portCount; ++portIndex) {
        std::unordered_set<ConnectionId> conns = connections(nodeId, portType, portIndex);

        for (auto connectionId : conns) {
            // Erases the information about the port on one side;
            auto c = makeIncompleteConnectionId(connectionId, portType);

            c = makeCompleteConnectionId(c, nodeId, portIndex - nRemovedPorts);

            _shiftedByDynamicPortsConnections.push_back(c);

            deleteConnection(connectionId);
        }
    }
}

void AbstractGraphModel::portsDeleted()
{
    for (auto const connectionId : _shiftedByDynamicPortsConnections) {
        addConnection(connectionId);
    }

    _shiftedByDynamicPortsConnections.clear();
}

void AbstractGraphModel::portsAboutToBeInserted(NodeId const nodeId,
                                                PortType const portType,
                                                PortIndex const first,
                                                PortIndex const last)
{
    _shiftedByDynamicPortsConnections.clear();

    auto portCountRole = portType == PortType::In ? NodeRole::InPortCount : NodeRole::OutPortCount;

    unsigned int portCount = nodeData(nodeId, portCountRole).toUInt();

    if (first > portCount)
        return;

    if (last < first)
        return;

    std::size_t const nNewPorts = last - first + 1;

    for (PortIndex portIndex = first; portIndex < portCount; ++portIndex) {
        std::unordered_set<ConnectionId> conns = connections(nodeId, portType, portIndex);

        for (auto connectionId : conns) {
            // Erases the information about the port on one side;
            auto c = makeIncompleteConnectionId(connectionId, portType);

            c = makeCompleteConnectionId(c, nodeId, portIndex + nNewPorts);

            _shiftedByDynamicPortsConnections.push_back(c);

            deleteConnection(connectionId);
        }
    }
}

void AbstractGraphModel::portsInserted()
{
    for (auto const connectionId : _shiftedByDynamicPortsConnections) {
        addConnection(connectionId);
    }

    _shiftedByDynamicPortsConnections.clear();
}

} // namespace QtNodes
