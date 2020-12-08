#include "BinaryTree.hpp"

#include <cstdlib>

BinaryTree::BinaryTree()
: root(nullptr)
{
}

void BinaryTree::dtor()
{
    deleteRecursively(&root);
}

void BinaryTree::deleteRecursively(Node **node)
{
    if ((*node)->left != nullptr) {
        deleteRecursively(&(*node)->left);
    }

    if ((*node)->right != nullptr) {
        deleteRecursively(&(*node)->right);
    }

    std::free(*node);
    *node = nullptr;
}

bool BinaryTree::isLeaf(const Node *node)
{
    return (node->left == nullptr) && (node->right == nullptr);
}

BinaryTree::Node *BinaryTree::createNode(int type, int data, BinaryTree::Node *left, BinaryTree::Node *right)
{
    auto node = (Node *) std::calloc(1, sizeof(Node));
    node->type = type;
    node->data = data;
    node->left = left;
    node->right = right;

    return node;
}

BinaryTree::Node *BinaryTree::copySubtreeRecursively(const BinaryTree::Node *node)
{
    if (node == nullptr) return nullptr;

    auto nodeCopy = (Node *) std::calloc(1, sizeof(Node));
    nodeCopy->type = node->type;
    nodeCopy->data = node->data;
    nodeCopy->left = copySubtreeRecursively(node->left);
    nodeCopy->right = copySubtreeRecursively(node->right);

    return nodeCopy;
}
