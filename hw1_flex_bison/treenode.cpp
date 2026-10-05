#include "treenode.h"
#include <stdexcept>
#include <string>
#include <vector>

namespace asttree {
static unsigned counter = 0;
treenode* BNFConverter::statementNode = nullptr;

static std::vector<string> usedNames;
static bool namesCollected = false;

void setParents(treenode* curr, treenode* parent)
{
    curr->setParent(parent);
    for (int i = 0; i < MAX_CHILD; i++) {
        treenode* child = curr->getChild(i);
        if (child != nullptr)
            setParents(child, curr);
    }
}

bool nameExists(const std::string& name)
{
    for (const std::string& used : usedNames)
    {
        if (used == name)
            return true;
    }

    return false;
}

void collectNames(treenode* node)
{
    if (node == nullptr)
        return;

    terminal* leaf = dynamic_cast<terminal*>(node);

    if (leaf != nullptr && !nameExists(leaf->value()))
        usedNames.push_back(leaf->value());

    for (int i = 0; i < MAX_CHILD; i++)
    {
        treenode* child = node->getChild(i);

        if (child != nullptr)
            collectNames(child);
    }
}


std::string freshName(expr* target)
{
    if (!namesCollected)
    {
        treenode* root = target;

        while (root->getParent() != nullptr)
            root = root->getParent();

        collectNames(root);
        namesCollected = true;
    }

    std::string name = "Symbol_" + std:: to_string(counter++);

    while(nameExists(name)){
        name = "Symbol_" + std:: to_string(counter);
        counter++;
    }

    usedNames.push_back(name);

    return name;
}

quant* reference(const std::string& name)
{
    return new quant(new terminal(name));
}

expr* sequence(treenode* left, treenode* right)
{
    return new expr(new expr(left), right);
}

rest* alternatives(treenode* left, treenode* right)
{
    return new rest(new rest(left), new terminal("|"), right);
}

void replaceTarget(expr* target, const std::string& name)
{
    treenode* parent = target->getParent();

    terminal* replacement = new terminal(name);
    parent->replace(target, replacement);
    replacement->setParent(parent);
}

void appendRule(const std::string& name, treenode* body)
{
    treenode* curr_pointer = BNFConverter::statementNode;
    // if (oldTail == nullptr || oldTail->getChild(1) != nullptr)
    //     throw std::logic_error("Expected a one-child rule-list tail");

    statement* rule = new statement(
        new terminal(name), new terminal(":"), body, new terminal(";")
    );
    statements* new_end = new statements(rule);

    curr_pointer->addChild(new_end);
    setParents(new_end, curr_pointer);
    BNFConverter::statementNode = new_end;
}

void BNFConverter::doZeroOrMore(expr* target, treenode* rhs)
{
    std::string name = freshName(target);
    treenode* body = alternatives(
        new empty(), sequence(rhs, reference(name))
    );

    replaceTarget(target, name);
    appendRule(name, body);
}

void BNFConverter::doZeroOrOne(expr* target, treenode* rhs)
{
    std::string name = freshName(target);
    treenode* body = alternatives(new empty(), rhs);

    replaceTarget(target, name);
    appendRule(name, body);
}

void BNFConverter::doOneOrMore(expr* target, treenode* rhs)
{
    std::string unitName = freshName(target);
    std::string name = freshName(target);
    treenode* body = alternatives(
        reference(unitName),
        sequence(reference(unitName), reference(name))
    );

    replaceTarget(target, name);
    appendRule(unitName, new rest(rhs));
    appendRule(name, body);
}

void BNFConverter::doGrouping(expr* target, treenode* rhs)
{
    std::string name = freshName(target);
    replaceTarget(target, name);
    appendRule(name, new rest(rhs));
}

void quant::doConversion()
{
    treenode::doConversion();

    if (numChildren == 3) {
        std::string opening = getChild(0)->value();
        treenode* contents = getChild(1);

        if (opening == "(")
            BNFConverter::doGrouping(this, contents);
        else if (opening == "[")
            BNFConverter::doZeroOrOne(this, contents);
        else if (opening == "<")
            BNFConverter::doZeroOrMore(this, contents);
    }
    else if (numChildren == 2) {
        std::string suffix = getChild(1)->value();
        treenode* contents = getChild(0);

        if (suffix == "+")
            BNFConverter::doOneOrMore(this, contents);
        else if (suffix == "*")
            BNFConverter::doZeroOrMore(this, contents);
    }

}

}
