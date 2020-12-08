#ifndef DIFFERENTIATOR_HPP
#define DIFFERENTIATOR_HPP

#include "BinaryTree.hpp"

#include <cstdio>

class Differentiator {
public:
    Differentiator();
    void dtor();

    void dumpToText() const;
    void dumpToDot() const;

    void operator()();

private:
    enum NodeType {
        Operation,
        Constant,
        Variable
    };

    enum Operation {
        UnknownOperation,
        Addition,
        Subtraction,
        Multiplication,
        Division,
        Power,
        Sin,
        Cos,
        Ln,
        Exponent
    };

    BinaryTree expressionBinaryTree;
    BinaryTree differentiationBinaryTree;

    std::FILE *dumpLatexFile;

    void optimizeTree(BinaryTree::Node **root);
    bool optimizeNeutralElementNodesRecursively(BinaryTree::Node **node);
    bool optimizeEvaluableNodesRecursively(BinaryTree::Node *node);
    bool optimizeSpecialConstantNodesRecursively(BinaryTree::Node **node);

    static void optimizeLeftNode(BinaryTree::Node **node);
    static void optimizeRightNode(BinaryTree::Node **node);
    static void zeroNode(BinaryTree::Node *node);

    static bool operationPriority(const BinaryTree::Node *node1, const BinaryTree::Node *node2);

    static bool isConstant(const BinaryTree::Node *node, int constant);
    static bool isFunction(const BinaryTree::Node *node);

    void dumpTreeToLatex(const BinaryTree::Node *root);
    static void dumpTreeToTextRecursively(std::FILE *dumpFile, const BinaryTree::Node *node, bool latex);
    static void dumpLeafNodeDataToText(std::FILE *dumpFile, const BinaryTree::Node *node);

    void dumpToDotRecursively(std::FILE *dumpDotFile, const BinaryTree::Node *node, bool isRoot) const;
    void dumpRootToDot(std::FILE *dumpDotFile, size_t *index, const BinaryTree::Node *root) const;
    void dumpNodeToDot(std::FILE *dumpDotFile, size_t *index, const BinaryTree::Node *node) const;
    static void dumpNodeTypeDataToDot(std::FILE *dumpDotFile, size_t index, const BinaryTree::Node *node);
    static void dumpNodeDataToData(std::FILE *dumpFile, const BinaryTree::Node *node);


    static enum Operation analyzeOperation(const char *operation);
    static const char *operationCodeToString(enum Operation operation);

    static const char *nodeTypeCodeToString(NodeType nodeType);

    void readTreeFromText();

    BinaryTree::Node *readTreeRecursively(char **text);

    BinaryTree::Node *differentiateNodeRecursively(const BinaryTree::Node *node);
};

#endif //DIFFERENTIATOR_HPP
