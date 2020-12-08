#ifndef BINARYTREE_HPP
#define BINARYTREE_HPP

#include <cstdio>

class BinaryTree {
public:
    struct Node {
        int type;
        int data;
        Node *left;
        Node *right;
    } *root;

    BinaryTree();
    void dtor();

    static Node *createNode(int type, int data, Node *left, Node *right);

    static BinaryTree::Node *copySubtreeRecursively(const BinaryTree::Node *node);

    static bool isLeaf(const Node *node);

    static void deleteRecursively(Node **node);
};

#endif /* BINARYTREE_HPP */
