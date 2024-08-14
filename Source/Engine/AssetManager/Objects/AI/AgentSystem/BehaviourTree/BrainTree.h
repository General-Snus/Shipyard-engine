// BrainTree - A C++ behavior tree single header library.
// Copyright 2015-2018 Par Arvidsson. All rights reserved.
// Licensed under the MIT license (https://github.com/arvidsson/BrainTree/blob/master/LICENSE).

#pragma once
#include <Engine/AssetManager/ComponentSystem/GameObject.h>
#include <Engine/AssetManager/Reflection/ReflectionTemplate.h>
#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace BrainTree
{

class Blackboard;

class Node : public Reflectable
{
  public:
    MYLIB_REFLECTABLE();
    enum class Status
    {
        Invalid,
        Success,
        Failure,
        Running,
    };

    virtual ~Node()
    {
    }
    void setBlackboard(std::shared_ptr<Blackboard> board)
    {
        blackboard = board;
    }
    std::shared_ptr<Blackboard> getBlackboard() const
    {
        return blackboard;
    }

    virtual Status update() = 0;
    virtual void initialize()
    {
    }
    virtual void terminate(Status s)
    {
        s;
    }

    Status tick();

    bool isSuccess() const
    {
        return m_Status == Status::Success;
    }
    bool isFailure() const
    {
        return m_Status == Status::Failure;
    }
    bool isRunning() const
    {
        return m_Status == Status::Running;
    }
    bool isTerminated() const
    {
        return isSuccess() || isFailure();
    }

    void reset()
    {
        m_Status = Status::Invalid;
    }
    Status getStatus() const
    {
        return m_Status;
    }

    void setName(const std::string &aName)
    {
        name = aName;
    }

    std::string getName() const
    {
        return name;
    }

    using Ptr = std::shared_ptr<Node>;

  protected:
    std::string name;
    Status m_Status = Status::Invalid;
    std::shared_ptr<Blackboard> blackboard = nullptr;
};

class Blackboard
{
  public:
    void setBool(std::string key, bool value)
    {
        bools[key] = value;
    }
    bool getBool(std::string key)
    {
        if (bools.find(key) == bools.end())
        {
            bools[key] = false;
        }
        return bools[key];
    }
    bool hasBool(std::string key) const
    {
        return bools.find(key) != bools.end();
    }

    void setGameObject(std::string key, GameObject value)
    {
        gameobjects[key] = value;
    }
    GameObject getGameObject(std::string key)
    {
        return gameobjects[key];
    }

    void setInt(std::string key, int value)
    {
        ints[key] = value;
    }
    int getInt(std::string key)
    {
        if (ints.find(key) == ints.end())
        {
            ints[key] = 0;
        }
        return ints[key];
    }
    bool hasInt(std::string key) const
    {
        return ints.find(key) != ints.end();
    }

    void setFloat(std::string key, float value)
    {
        floats[key] = value;
    }
    float getFloat(std::string key)
    {
        if (floats.find(key) == floats.end())
        {
            floats[key] = 0.0f;
        }
        return floats[key];
    }
    bool hasFloat(std::string key) const
    {
        return floats.find(key) != floats.end();
    }

    void setDouble(std::string key, double value)
    {
        doubles[key] = value;
    }
    double getDouble(std::string key)
    {
        if (doubles.find(key) == doubles.end())
        {
            doubles[key] = 0.0f;
        }
        return doubles[key];
    }
    bool hasDouble(std::string key) const
    {
        return doubles.find(key) != doubles.end();
    }

    void setString(std::string key, std::string value)
    {
        strings[key] = value;
    }
    std::string getString(std::string key)
    {
        if (strings.find(key) == strings.end())
        {
            strings[key] = "";
        }
        return strings[key];
    }
    bool hasString(std::string key) const
    {
        return strings.find(key) != strings.end();
    }

    void setStatus(Node *ptr, Node::Status status);
    Node::Status getStatus(Node *ptr);
    std::unordered_map<Node *, Node::Status> &getAllStatus();

    using Ptr = std::shared_ptr<Blackboard>;

  protected:
    std::unordered_map<std::string, bool> bools;
    std::unordered_map<std::string, GameObject> gameobjects;
    std::unordered_map<std::string, int> ints;
    std::unordered_map<Node *, Node::Status> nodeStatus;
    std::unordered_map<std::string, float> floats;
    std::unordered_map<std::string, double> doubles;
    std::unordered_map<std::string, std::string> strings;
};

class Composite : public Node
{
  public:
    MYLIB_REFLECTABLE();
    virtual ~Composite()
    {
    }

    void addChild(Node::Ptr child)
    {
        children.push_back(child);
        it = children.begin();
    }
    bool hasChildren() const
    {
        return !children.empty();
    }

  protected:
    std::vector<Node::Ptr> children;
    std::vector<Node::Ptr>::iterator it;
};

class Decorator : public Node
{
  public:
    MYLIB_REFLECTABLE();
    virtual ~Decorator()
    {
    }

    void setChild(Node::Ptr node)
    {
        child = node;
    }
    bool hasChild() const
    {
        return child != nullptr;
    }

  protected:
    Node::Ptr child = nullptr;
};

class Leaf : public Node
{
  public:
    MYLIB_REFLECTABLE();
    Leaf()
    {
    }
    virtual ~Leaf()
    {
    }
    Leaf(Blackboard::Ptr blackboard) : blackboard(blackboard)
    {
    }

    virtual Status update() = 0;

  protected:
    Blackboard::Ptr blackboard;
};

class BehaviorTree : public Node
{
  public:
    MYLIB_REFLECTABLE();
    BehaviorTree()
    {
        blackboard = std::make_shared<Blackboard>();
    }
    BehaviorTree(const Node::Ptr &rootNode) // I belive this is a bug in many cases lets instead check if there is a
                                            // blackboard in the tree : BehaviorTree()
    {
        root = rootNode;
        if (const auto board = rootNode->getBlackboard())
        {
            blackboard = board;
        }
    }

    Status update()
    {
        return root->tick();
    }

    void setRoot(const Node::Ptr &node)
    {
        root = node;
    }

  private:
    Node::Ptr root = nullptr;
};

template <class Parent> class DecoratorBuilder;

template <class Parent> class CompositeBuilder
{
  public:
    CompositeBuilder(Parent *parent, Composite *node) : parent(parent), node(node)
    {
    }

    template <class NodeType, typename... Args>
    CompositeBuilder<Parent> leaf(const std::string &name = "", Args... args)
    {
        auto child = std::make_shared<NodeType>((args)...);
        child->setBlackboard(node->getBlackboard());
        node->addChild(child);
        child->setName(name);
        return *this;
    }

    template <class CompositeType, typename... Args>
    CompositeBuilder<CompositeBuilder<Parent>> composite(const std::string &name = "", Args... args)
    {
        auto child = std::make_shared<CompositeType>((args)...);
        child->setBlackboard(node->getBlackboard());
        node->addChild(child);
        child->setName(name);
        return CompositeBuilder<CompositeBuilder<Parent>>(this, (CompositeType *)child.get());
    }

    template <class DecoratorType, typename... Args>
    DecoratorBuilder<CompositeBuilder<Parent>> decorator(const std::string &name = "", Args... args)
    {
        auto child = std::make_shared<DecoratorType>((args)...);
        child->setBlackboard(node->getBlackboard());
        node->addChild(child);
        child->setName(name);
        return DecoratorBuilder<CompositeBuilder<Parent>>(this, (DecoratorType *)child.get());
    }

    Parent &end()
    {
        return *parent;
    }

  private:
    Parent *parent;
    Composite *node;
};

template <class Parent> class DecoratorBuilder
{
  public:
    DecoratorBuilder(Parent *parent, Decorator *node) : parent(parent), node(node)
    {
    }

    template <class NodeType, typename... Args>
    DecoratorBuilder<Parent> leaf(const std::string &name = "", Args... args)
    {
        auto child = std::make_shared<NodeType>((args)...);
        child->setBlackboard(node->getBlackboard());
        node->setChild(child);
        child->setName(name);
        return *this;
    }

    template <class CompositeType, typename... Args>
    CompositeBuilder<DecoratorBuilder<Parent>> composite(const std::string &name = "", Args... args)
    {
        auto child = std::make_shared<CompositeType>((args)...);
        child->setBlackboard(node->getBlackboard());
        node->setChild(child);
        child->setName(name);
        return CompositeBuilder<DecoratorBuilder<Parent>>(this, (CompositeType *)child.get());
    }

    template <class DecoratorType, typename... Args>
    DecoratorBuilder<DecoratorBuilder<Parent>> decorator(const std::string &name = "", Args... args)
    {
        auto child = std::make_shared<DecoratorType>((args)...);
        child->setBlackboard(node->getBlackboard());
        node->setChild(child);
        child->setName(name);
        return DecoratorBuilder<DecoratorBuilder<Parent>>(this, (DecoratorType *)child.get());
    }

    Parent &end()
    {
        return *parent;
    }

  private:
    Parent *parent;
    Decorator *node;
};

class Builder
{
  public:
    Builder()
    {
        tree = std::make_shared<BehaviorTree>();
    }
    template <class NodeType, typename... Args> Builder leaf(const std::string &name = "", Args... args)
    {
        root = std::make_shared<NodeType>((args)...);
        root->setBlackboard(tree->getBlackboard());
        root->setName(name);
        return *this;
    }

    template <class CompositeType, typename... Args>
    CompositeBuilder<Builder> composite(const std::string &name = "", Args... args)
    {
        root = std::make_shared<CompositeType>((args)...);
        root->setBlackboard(tree->getBlackboard());
        root->setName(name);
        return CompositeBuilder<Builder>(this, (CompositeType *)root.get());
    }

    template <class DecoratorType, typename... Args>
    DecoratorBuilder<Builder> decorator(const std::string &name = "", Args... args)
    {
        root = std::make_shared<DecoratorType>((args)...);
        root->setBlackboard(tree->getBlackboard());
        root->setName(name);
        return DecoratorBuilder<Builder>(this, (DecoratorType *)root.get());
    }

    Node::Ptr build()
    {
        assert(root != nullptr && "The Behavior Tree is empty!");
        tree->setRoot(root);
        return tree;
    }

  private:
    Node::Ptr root;
    std::shared_ptr<BehaviorTree> tree;
};

// The Selector composite ticks each child node in order.
// If a child succeeds or runs, the selector returns the same status.
// In the next tick, it will try to run each child in order again.
// If all children fails, only then does the selector fail.
class Selector : public Composite
{
  public:
    MYLIB_REFLECTABLE();
    void initialize() override
    {
        it = children.begin();
    }

    Status update() override
    {
        assert(hasChildren() && "Composite has no children");

        while (it != children.end())
        {
            auto status = (*it)->tick();

            if (status != Status::Failure)
            {
                return status;
            }

            it++;
        }

        return Status::Failure;
    }
};

// The Sequence composite ticks each child node in order.
// If a child fails or runs, the sequence returns the same status.
// In the next tick, it will try to run each child in order again.
// If all children succeeds, only then does the sequence succeed.
class Sequence : public Composite
{
  public:
    MYLIB_REFLECTABLE();
    void initialize() override
    {
        it = children.begin();
    }

    Status update() override
    {
        assert(hasChildren() && "Composite has no children");

        while (it != children.end())
        {
            auto status = (*it)->tick();

            if (status != Status::Success)
            {
                return status;
            }

            it++;
        }

        return Status::Success;
    }
};

// The StatefulSelector composite ticks each child node in order, and remembers what child it prevously tried to tick.
// If a child succeeds or runs, the stateful selector returns the same status.
// In the next tick, it will try to run the next child or start from the beginning again.
// If all children fails, only then does the stateful selector fail.
class StatefulSelector : public Composite
{
  public:
    MYLIB_REFLECTABLE();
    Status update() override
    {
        assert(hasChildren() && "Composite has no children");

        while (it != children.end())
        {
            auto status = (*it)->tick();

            if (status != Status::Failure)
            {
                return status;
            }

            it++;
        }

        it = children.begin();
        return Status::Failure;
    }
};

// The StatefulSequence composite ticks each child node in order, and remembers what child it prevously tried to tick.
// If a child fails or runs, the stateful sequence returns the same status.
// In the next tick, it will try to run the next child or start from the beginning again.
// If all children succeeds, only then does the stateful sequence succeed.
class StatefulSequence : public Composite
{
  public:
    MYLIB_REFLECTABLE();
    Status update() override
    {
        assert(hasChildren() && "Composite has no children");

        while (it != children.end())
        {
            auto status = (*it)->tick();

            if (status != Status::Success)
            {
                return status;
            }

            it++;
        }

        it = children.begin();
        return Status::Success;
    }
};

class ParallelSequence : public Composite
{
  public:
    MYLIB_REFLECTABLE();
    ParallelSequence(bool successOnAll = true, bool failOnAll = true)
        : useSuccessFailPolicy(true), successOnAll(successOnAll), failOnAll(failOnAll)
    {
    }
    ParallelSequence(int minSuccess, int minFail) : minSuccess(minSuccess), minFail(minFail)
    {
    }

    Status update() override
    {
        assert(hasChildren() && "Composite has no children");

        int minimumSuccess = minSuccess;
        int minimumFail = minFail;

        if (useSuccessFailPolicy)
        {
            if (successOnAll)
            {
                minimumSuccess = static_cast<int>(children.size());
            }
            else
            {
                minimumSuccess = 1;
            }

            if (failOnAll)
            {
                minimumFail = static_cast<int>(children.size());
            }
            else
            {
                minimumFail = 1;
            }
        }

        int total_success = 0;
        int total_fail = 0;

        for (auto &child : children)
        {
            auto status = child->tick();
            if (status == Status::Success)
            {
                total_success++;
            }
            if (status == Status::Failure)
            {
                total_fail++;
            }
        }

        if (total_success >= minimumSuccess)
        {
            return Status::Success;
        }
        if (total_fail >= minimumFail)
        {
            return Status::Failure;
        }

        return Status::Running;
    }

  private:
    bool useSuccessFailPolicy = false;
    bool successOnAll = true;
    bool failOnAll = true;
    int minSuccess = 0;
    int minFail = 0;
};

// The Succeeder decorator returns success, regardless of what happens to the child.
class Succeeder : public Decorator
{
  public:
    MYLIB_REFLECTABLE();
    Status update() override
    {
        child->tick();
        return Status::Success;
    }
};

// The Failer decorator returns failure, regardless of what happens to the child.
class Failer : public Decorator
{
  public:
    MYLIB_REFLECTABLE();
    Status update() override
    {
        child->tick();
        return Status::Failure;
    }
};

// The Inverter decorator inverts the child node's status, i.e. failure becomes success and success becomes failure.
// If the child runs, the Inverter returns the status that it is running too.
class Inverter : public Decorator
{
  public:
    MYLIB_REFLECTABLE();
    Status update() override
    {
        auto s = child->tick();

        if (s == Status::Success)
        {
            return Status::Failure;
        }
        else if (s == Status::Failure)
        {
            return Status::Success;
        }

        return s;
    }
};

// The Repeater decorator repeats infinitely or to a limit until the child returns success.
class Repeater : public Decorator
{
  public:
    MYLIB_REFLECTABLE();
    Repeater(int limit = 0) : limit(limit)
    {
    }

    void initialize() override
    {
        counter = 0;
    }

    Status update() override
    {
        child->tick();

        if (limit > 0 && ++counter == limit)
        {
            return Status::Success;
        }

        return Status::Running;
    }

  protected:
    int limit;
    int counter = 0;
};

// The UntilSuccess decorator repeats until the child returns success and then returns success.
class UntilSuccess : public Decorator
{
  public:
    MYLIB_REFLECTABLE();
    Status update() override
    {
        while (1)
        {
            auto status = child->tick();

            if (status == Status::Success)
            {
                return Status::Success;
            }
        }
    }
};

// The UntilFailure decorator repeats until the child returns fail and then returns success.
class UntilFailure : public Decorator
{
  public:
    MYLIB_REFLECTABLE();
    Status update() override
    {
        while (1)
        {
            auto status = child->tick();

            if (status == Status::Failure)
            {
                return Status::Success;
            }
        }
    }
};

} // namespace BrainTree

REFL_AUTO(type(BrainTree::Node))
REFL_AUTO(type(BrainTree::Composite))
REFL_AUTO(type(BrainTree::Decorator))
REFL_AUTO(type(BrainTree::Leaf))
REFL_AUTO(type(BrainTree::BehaviorTree))
REFL_AUTO(type(BrainTree::Selector))
REFL_AUTO(type(BrainTree::Sequence))
REFL_AUTO(type(BrainTree::StatefulSelector))
REFL_AUTO(type(BrainTree::StatefulSequence))
REFL_AUTO(type(BrainTree::ParallelSequence))
REFL_AUTO(type(BrainTree::Succeeder))
REFL_AUTO(type(BrainTree::Failer))
REFL_AUTO(type(BrainTree::Inverter))
REFL_AUTO(type(BrainTree::Repeater))
REFL_AUTO(type(BrainTree::UntilSuccess))
REFL_AUTO(type(BrainTree::UntilFailure))
