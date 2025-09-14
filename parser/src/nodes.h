#pragma once
#include <vector>
#include <string>

struct Attribute
{
    bool need_reflect;
    bool force_no_reflect;
};

class Node
{
  public:
    enum class Type
    {
        Unknown,
        Namespace,
        Field,
        Class,
        Enum
    };
    Node() = default;
    Node(Type type) : type(type) {}
    Node(Type type, const std::string &name) : type(type), name(name) {}
    virtual ~Node()
    {
        for (auto child : children) { delete child; }
        children.clear();
    }
    Type type;
    std::string name;
    std::vector<Node *> children;
    Attribute attr = {false, false};
};

class NamespaceNode : public Node
{
  public:
    NamespaceNode() : Node(Node::Type::Namespace) {}
    NamespaceNode(const std::string &name) : Node(Node::Type::Namespace, name) {}
};

class FieldNode : public Node
{
  public:
    FieldNode() : Node(Node::Type::Field) {}
    FieldNode(const std::string &name) : Node(Node::Type::Field, name) {}
};

class ClassNode : public Node
{
  public:
    ClassNode() : Node(Node::Type::Class) {}
    ClassNode(const std::string &name) : Node(Node::Type::Class, name) {}
    ~ClassNode()
    {
        for (auto field : fields) { delete field; }
        fields.clear();
    }
    std::vector<FieldNode *> fields;
};

class EnumNode : public Node
{
  public:
    EnumNode() : Node(Node::Type::Enum) {}
    EnumNode(const std::string &name) : Node(Node::Type::Enum, name) {}

    std::vector<std::string> items;
};
