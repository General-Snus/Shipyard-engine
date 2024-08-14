#include "Engine/AssetManager/AssetManager.pch.h"

#include "BrainTree.h"

namespace BrainTree
{

void Blackboard::setStatus(Node *ptr, Node::Status status)
{
    nodeStatus[ptr] = status;
}

Node::Status Blackboard::getStatus(Node *ptr)
{
    return nodeStatus[ptr];
}

std::unordered_map<Node *, Node::Status> &Blackboard::getAllStatus()
{
    return nodeStatus;
}

Node::Status Node::tick()
{
    if (m_Status != Status::Running)
    {
        initialize();
    }

    m_Status = update();

    getBlackboard()->setStatus(this, m_Status);
    if (m_Status != Status::Running)
    {
        terminate(m_Status);
    }

    return m_Status;
}

} // namespace BrainTree
