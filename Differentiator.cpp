#include "Differentiator.hpp"

#include "File.hpp"

#include <cmath>
#include <cstring>
#include <cstdlib>
#include <cctype>

Differentiator::Differentiator()
: dumpLatexFile(nullptr)
{
    readTreeFromText();
}

void Differentiator::dumpToText() const
{
    auto dumpTextFile = std::fopen("DifferentiatorTextDump.txt", "w");

    std::fprintf(dumpTextFile, "Binary tree for source expression:\n");
    dumpTreeToTextRecursively(dumpTextFile, expressionBinaryTree.root, false);
    std::fprintf(dumpTextFile, "\n");

    std::fprintf(dumpTextFile, "Binary tree for differentiated expression:\n");
    dumpTreeToTextRecursively(dumpTextFile, differentiationBinaryTree.root, false);
    std::fprintf(dumpTextFile, "\n");

    std::fclose(dumpTextFile);
}

void Differentiator::readTreeFromText()
{
    auto dumpTextFile = std::fopen("input.txt", "r");
    char *buffer = readFileToBuf(dumpTextFile);
    std::fclose(dumpTextFile);

    char *reader = buffer;
    expressionBinaryTree.root = readTreeRecursively(&reader);

    std::free(buffer);
}

BinaryTree::Node *Differentiator::readTreeRecursively(char **text)
{
    if (*(*text)++ == '(') {
        if (**text == ')') {
            return nullptr;
        }

        auto node = (BinaryTree::Node *) std::calloc(1, sizeof(BinaryTree::Node));

        if (**text == '(') {
            node->left = readTreeRecursively(text);
            node->data = analyzeOperation(std::strtok(*text, ")("));
            node->type = NodeType::Operation;
            *text = std::strtok(nullptr, "") - 1;
            **text = '(';
            node->right = readTreeRecursively(text);

            return node;
        }

        auto data = std::strtok(*text, ")");
        *text = std::strtok(nullptr, "");
        if ((strlen(data) == 1) && std::isalpha(*data)) {
            node->data = (unsigned char) *data;
            node->type = NodeType::Variable;
        } else {
            node->data = std::strtol(data, nullptr, 10);
            node->type = NodeType::Constant;
        }

        return node;
    }

    return nullptr;
}

void Differentiator::dumpToDot() const
{
    std::FILE *dumpDotFile = std::fopen("DifferentiatorDump.dot", "w");
    std::fprintf(dumpDotFile, "digraph G {\n"
                              "\trankdir=HR\n");

    dumpToDotRecursively(dumpDotFile, expressionBinaryTree.root, true);
    dumpToDotRecursively(dumpDotFile, differentiationBinaryTree.root, true);

    std::fprintf(dumpDotFile, "}\n");
    std::fclose(dumpDotFile);

    std::system("dot -Tpng DifferentiatorDump.dot > DifferentiatorDump.png");
    std::system("start DifferentiatorDump.png");
    std::system("pause");
}

void Differentiator::dumpToDotRecursively(std::FILE *dumpDotFile, const BinaryTree::Node *node, bool isRoot) const
{
    static size_t index = 0;

    if (node == nullptr) return;

    if (isRoot) {
        ++index;
        dumpRootToDot(dumpDotFile, &index, node);
        return;
    }

    dumpNodeToDot(dumpDotFile, &index, node);
}

void Differentiator::dumpNodeTypeDataToDot(std::FILE *dumpDotFile, size_t index, const BinaryTree::Node *node)
{
    std::fprintf(dumpDotFile, "\t%zu[fillcolor=\"#add8e6\", color=\"#00008b\", fontcolor=\"#00008b\", label=\""
                              "<left> left | <data> data\\n ",
                 index);

    dumpNodeDataToData(dumpDotFile, node);
    std::fprintf(dumpDotFile, " | <type> type\\n %s | <right> right\", style=filled, shape=record];\n",
                 nodeTypeCodeToString((NodeType) node->type));
}

void Differentiator::dumpRootToDot(std::FILE *dumpDotFile, size_t *index, const BinaryTree::Node *root) const
{
    dumpNodeTypeDataToDot(dumpDotFile, *index, root);
    dumpNodeToDot(dumpDotFile, index, root);
}

void Differentiator::dumpNodeToDot(std::FILE *dumpDotFile, size_t *index, const BinaryTree::Node *node) const
{
    if (BinaryTree::isLeaf(node)) return;

    size_t nodeI = *index;

    ++*index;
    if (node->right != nullptr) {
        dumpNodeTypeDataToDot(dumpDotFile, *index, node->right);
        std::fprintf(dumpDotFile, "\t%zu:<right>->%zu;\n", nodeI, *index);
        dumpToDotRecursively(dumpDotFile, node->right, false);
    }

    ++*index;
    if (node->left != nullptr) {
        dumpNodeTypeDataToDot(dumpDotFile, *index, node->left);
        std::fprintf(dumpDotFile, "\t%zu:<left>->%zu;\n", nodeI, *index);
        dumpToDotRecursively(dumpDotFile, node->left, false);
    }
}

void Differentiator::dtor()
{
    expressionBinaryTree.dtor();
}

void Differentiator::operator()()
{
    dumpLatexFile = std::fopen("DifferentiatorDumpLatex.tex", "w");
    std::fprintf(dumpLatexFile, "\\documentclass[a4paper,12pt]{article}\n"
                                "\\usepackage{ulem}\n"
                                "\\usepackage{breqn}\n"
                                "\\begin{document}\n"
                                "\\begin{center}\n"
                                "\t{\\large \\textbf{Smoke MATH everyday}}\n"
                                "\\end{center}\n"
                                "\\begin{center}\n"
                                "\t{\\large \\textit{\\textbf{G.K. Lebedev},\\\\\n"
                                "Department of Applied MATH and Informatics,\\\\\n"
                                "Applied MATH and Physics, Applied MATH Programme,\\\\\n"
                                "Moscow Institute of Physics and \\sout{MATH} Technologies,\\\\\n"
                                "academic adviser \\textbf{V.A. Gordin},\\\\\n"
                                "HSE \\& Hydrometcenter of Russia}}\n"
                                "\\end{center}\n");

    std::fprintf(dumpLatexFile, "\\section*{\\textbf{Abstract}}\n"
                                "We listen to Taylor Swift and expand to taylor swift. Any MATH needs an application and in this work\n"
                                "we wanted to demonstrate how to do MATH with SWAG.\\\n"
                                "Original expression (looks legit):\n");
    dumpTreeToLatex(expressionBinaryTree.root);
    std::fprintf(dumpLatexFile, "\\section*{\\textbf{Original expression analysis}}\n");
    optimizeTree(&expressionBinaryTree.root);
    std::fprintf(dumpLatexFile, "Original expression after simplifications:\n");
    dumpTreeToLatex(expressionBinaryTree.root);

    std::fprintf(dumpLatexFile, "\\section*{\\textbf{Differentiation of the original expression}}\n");
    differentiationBinaryTree.root = differentiateNodeRecursively(expressionBinaryTree.root);
    std::fprintf(dumpLatexFile, "Derivative of the original expression:\n");
    dumpTreeToLatex(differentiationBinaryTree.root);
    std::fprintf(dumpLatexFile, "\\section*{\\textbf{Original expression derivative analysis}}\n");
    optimizeTree(&differentiationBinaryTree.root);
    std::fprintf(dumpLatexFile, "Derivative of expression after simplification:\n");
    dumpTreeToLatex(differentiationBinaryTree.root);

    std::fprintf(dumpLatexFile, "\\section*{References}\n"
                                "\\begin{enumerate}\n"
                                "\\item Snoop Dogg ft. Dr. Dre --- Smoke Weed Everyday\n"
                                "\\item Some soviet MATH textbook for kindergarten"
                                "\\item Thomas W. Judson, Stephen F. Abstract Algebra: Theory and Applications\n"
                                "\\item Silvanus P. Thompson. Calculus Made Easy\n"
                                "\\item Gareth James. An Introduction to Statistical Learning: With Applications in R\n"
                                "\\end{enumerate}\n"
                                "\\end{document}\n");
    std::fclose(dumpLatexFile);
}

BinaryTree::Node *Differentiator::differentiateNodeRecursively(const BinaryTree::Node *node)
{
#define L node->left
#define R node->right
#define TYPE node->type
#define lTYPE node->left->type
#define rTYPE node->right->type
#define dL differentiateNodeRecursively(L)
#define dR differentiateNodeRecursively(R)
#define cL BinaryTree::copySubtreeRecursively(L)
#define cR BinaryTree::copySubtreeRecursively(R)
#define CREATE_OPERATION_NODE(operation, leftNode, rightNode) BinaryTree::createNode(Operation, (operation), (leftNode), (rightNode))
#define CONSTANT(constant) BinaryTree::createNode(Constant, (constant), nullptr, nullptr)
#define ADDITION(leftNode, rightNode) CREATE_OPERATION_NODE(Addition, (leftNode), (rightNode))
#define SUBTRACTION(leftNode, rightNode) CREATE_OPERATION_NODE(Subtraction, (leftNode), (rightNode))
#define MULTIPLICATION(leftNode, rightNode) CREATE_OPERATION_NODE(Multiplication, (leftNode), (rightNode))
#define DIVISION(leftNode, rightNode) CREATE_OPERATION_NODE(Division, (leftNode), (rightNode))
#define POWER(leftNode, rightNode) CREATE_OPERATION_NODE(Power, (leftNode), (rightNode))
#define SIN(node) CREATE_OPERATION_NODE(Sin, nullptr, node)
#define COS(node) CREATE_OPERATION_NODE(Cos, nullptr, node)
#define LN(node) CREATE_OPERATION_NODE(Ln, nullptr, node)
#define EXPONENT(node) CREATE_OPERATION_NODE(Exponent, nullptr, node)
#define BEGIN_DIFFERENTIATION                                                                       \
    do {                                                                                            \
        std::fprintf(dumpLatexFile, "Let us differentiate this part of the expression above:\\\n"); \
        dumpTreeToLatex(node);                                                                      \
    } while (0)
#define END_DIFFERENTIATION(comment)                                                                   \
    do {                                                                                      \
        std::fprintf(dumpLatexFile, "%s:\\\n", (comment)); \
        dumpTreeToLatex(resultNode);                                                          \
    } while (0)

    BEGIN_DIFFERENTIATION;

    switch (TYPE) {
        case Constant: {
            auto resultNode = CONSTANT(0);
            END_DIFFERENTIATION("Zero the derivative of a constant everyday");
            return resultNode;
        }
        case Variable: {
            auto resultNode = CONSTANT(1);
            END_DIFFERENTIATION("Formula from \"An Introduction to Statistical Learning: With Applications in R\"");
            return resultNode;
        }
        case Operation: {
            switch (node->data) {
                case Addition: {
                    auto resultNode = ADDITION(dL, dR);
                    END_DIFFERENTIATION("No matter addition or substraction - the differentiation operator is linear");
                    return resultNode;
                }
                case Subtraction: {
                    auto resultNode = SUBTRACTION(dL, dR);
                    END_DIFFERENTIATION("No matter addition or substraction - the differentiation operator is linear");
                    return resultNode;
                }
                case Multiplication: {
                    auto resultNode = ADDITION(MULTIPLICATION(dL, cR), MULTIPLICATION(cL, dR));
                    END_DIFFERENTIATION("At least we dont need the general Leibniz rule here");
                    return resultNode;
                }
                case Division: {
                    auto resultNode = DIVISION(SUBTRACTION(MULTIPLICATION(dL, cR), MULTIPLICATION(cL, dR)), POWER(cR, CONSTANT(2)));
                    END_DIFFERENTIATION("Divide and conquer");
                    return resultNode;
                }
                case Power: {
                    BinaryTree::Node *resultNode = nullptr;
                    if (((lTYPE == Variable) || lTYPE == Operation) && (rTYPE == Constant)) {
                        resultNode = MULTIPLICATION(cR, MULTIPLICATION(POWER(cL, SUBTRACTION(cR, CONSTANT(1))), dL));
                    } else if ((lTYPE == Constant) && ((rTYPE == Variable) || (rTYPE == Operation))) {
                        resultNode = MULTIPLICATION(MULTIPLICATION(POWER(cL, cR), LN(cR)), dR);
                    } else {
                        resultNode = MULTIPLICATION(POWER(cL, cR),
                                                    ADDITION(MULTIPLICATION(dR, LN(cL)),
                                                             MULTIPLICATION(cR, MULTIPLICATION(DIVISION(CONSTANT(1), cL), dL))));
                    }
                    END_DIFFERENTIATION("Every soviet kindergarten graduate knows");
                    return resultNode;
                }
                case Sin: {
                    auto resultNode = MULTIPLICATION(COS(cR), dR);
                    END_DIFFERENTIATION("Yours sins will be forgiven");
                    return resultNode;
                }
                case Cos: {
                    auto resultNode = SUBTRACTION(CONSTANT(0), MULTIPLICATION(SIN(cR), dR));
                    END_DIFFERENTIATION("Bless you on behalf of trigonometry");
                    return resultNode;
                }
                case Ln: {
                    auto resultNode = MULTIPLICATION(DIVISION(CONSTANT(1), cR), dR);
                    END_DIFFERENTIATION("Before you take the logarithm you better know the rithm");
                    return resultNode;
                }
                case Exponent: {
                    auto resultNode = MULTIPLICATION(EXPONENT(cR), dR);
                    END_DIFFERENTIATION("No matter how hard you try, she doesn't care");
                    return resultNode;
                }
                case UnknownOperation: {
                    std::fprintf(stderr, "ERROR: unknown operation in expression binary tree");
                    return nullptr;
                }
            }
        }
        default:
            return nullptr;
    }

#undef L
#undef R
#undef TYPE
#undef lTYPE
#undef rTYPE
#undef dL
#undef dR
#undef cL
#undef cR
#undef CREATE_OPERATION_NODE
#undef CONSTANT
#undef ADDITION
#undef SUBTRACTION
#undef MULTIPLICATION
#undef DIVISION
#undef POWER
#undef SIN
#undef COS
#undef LN
#undef EXPONENT
#undef BEGIN_DIFFERENTIATION
#undef END_DIFFERENTIATION
}

enum Differentiator::Operation Differentiator::analyzeOperation(const char *operation)
{
    if (std::strcmp(operation, "+") == 0) return Addition;
    if (std::strcmp(operation, "-") == 0) return Subtraction;
    if (std::strcmp(operation, "*") == 0) return Multiplication;
    if (std::strcmp(operation, "/") == 0) return Division;
    if (std::strcmp(operation, "^") == 0) return Power;
    if (std::strcmp(operation, "sin") == 0) return Sin;
    if (std::strcmp(operation, "cos") == 0) return Cos;
    if (std::strcmp(operation, "ln") == 0) return Ln;
    if (std::strcmp(operation, "exp") == 0) return Exponent;

    std::fprintf(stderr, "ERROR: unknown operation in expression binary tree");
    return UnknownOperation;
}

const char *Differentiator::operationCodeToString(enum Differentiator::Operation operation)
{
    switch (operation) {
        case Addition: {
            return "+";
        }
        case Subtraction: {
            return "-";
        }
        case Multiplication: {
            return "*";
        }
        case Division: {
            return "/";
        }
        case Power: {
            return "^";
        }
        case Sin: {
            return "sin";
        }
        case Cos: {
            return "cos";
        }
        case Ln: {
            return "ln";
        }
        case Exponent: {
            return "exp";
        }
        case UnknownOperation: {
            std::fprintf(stderr, "ERROR: unknown operation in binary tree");
            return nullptr;
        }
    }
}

void Differentiator::dumpTreeToTextRecursively(std::FILE *dumpFile, const BinaryTree::Node *node, bool latex)
{
    if (node == nullptr) return;

    if (BinaryTree::isLeaf(node)) {
        dumpLeafNodeDataToText(dumpFile, node);
        return;
    }

#define PLACE_OPENING_BRACE_FOR_PRIORITY(needPriorityBrace) \
    if (needPriorityBrace) std::fprintf(dumpFile, "(")
#define PLACE_CLOSING_BRACE_FOR_PRIORITY(needPriorityBrace) \
    if (needPriorityBrace) std::fprintf(dumpFile, ")")
#define DUMP_UNARY_OPERATION_NODE(operationString)           \
    do {                                                     \
        bool needPriorityBrace = operationPriority(node, node->right); \
        std::fprintf(dumpFile, "%s", (operationString));     \
        PLACE_OPENING_BRACE_FOR_PRIORITY(needPriorityBrace); \
        dumpTreeToTextRecursively(dumpFile, node->right, latex);       \
        PLACE_CLOSING_BRACE_FOR_PRIORITY(needPriorityBrace); \
    } while (0)
#define DUMP_BINARY_OPERATION_NODE(operationString)                   \
    do {                                                              \
        bool needPriorityBraceLeftNode = operationPriority(node, node->left);  \
        bool needPriorityBraceRightNode = operationPriority(node, node->right); \
        PLACE_OPENING_BRACE_FOR_PRIORITY(needPriorityBraceLeftNode);  \
        dumpTreeToTextRecursively(dumpFile, node->left, latex);                \
        PLACE_CLOSING_BRACE_FOR_PRIORITY(needPriorityBraceLeftNode);  \
        std::fprintf(dumpFile, "%s", (operationString));              \
        PLACE_OPENING_BRACE_FOR_PRIORITY(needPriorityBraceRightNode); \
        dumpTreeToTextRecursively(dumpFile, node->right, latex);                \
        PLACE_CLOSING_BRACE_FOR_PRIORITY(needPriorityBraceRightNode); \
    } while (0)
#define DUMP_SUBTRACTION_NODE_LATEX                                       \
    do {                                                                  \
        bool needPriorityBraceLeftNode = operationPriority(node, node->left);      \
        bool needPriorityBraceRightNode = operationPriority(node, node->right);     \
        bool isUnaryMinus = isConstant(node->left, 0);                    \
        if (!isUnaryMinus) dumpTreeToTextRecursively(dumpFile, node->left, latex); \
        PLACE_OPENING_BRACE_FOR_PRIORITY(isUnaryMinus);                   \
        std::fprintf(dumpFile, "-");                                      \
        dumpTreeToTextRecursively(dumpFile, node->right, latex);                    \
        PLACE_CLOSING_BRACE_FOR_PRIORITY(isUnaryMinus);                   \
    } while (0)
#define DUMP_DIVISION_NODE_LATEX                                 \
    do {                                                         \
        std::fprintf(dumpFile, " \\frac{");                      \
        dumpTreeToTextRecursively(dumpFile, node->left, latex);           \
        std::fprintf(dumpFile, "}{");                            \
        dumpTreeToTextRecursively(dumpFile, node->right, latex);           \
        std::fprintf(dumpFile, "}");                             \
    } while (0)
#define DUMP_POWER_NODE_LATEX                                                                 \
    do {                                                                                      \
        bool needPriorityBraceLeftNode = operationPriority(node, node->left);                          \
        if (isFunction(node->left)) {                                                                  \
            std::fprintf(dumpFile, " \\%s^{", operationCodeToString((enum Operation) node->left->data)); \
            dumpTreeToTextRecursively(dumpFile, node->right, latex);                                    \
            std::fprintf(dumpFile, "}");                                                      \
            PLACE_OPENING_BRACE_FOR_PRIORITY(needPriorityBraceLeftNode);                      \
            dumpTreeToTextRecursively(dumpFile, node->left->right, latex);                             \
            PLACE_CLOSING_BRACE_FOR_PRIORITY(needPriorityBraceLeftNode);                      \
        } else {                                                                              \
            std::fprintf(dumpFile, "{");                                                      \
            PLACE_OPENING_BRACE_FOR_PRIORITY(needPriorityBraceLeftNode);                      \
            dumpTreeToTextRecursively(dumpFile, node->left, latex);                                    \
            PLACE_CLOSING_BRACE_FOR_PRIORITY(needPriorityBraceLeftNode);                      \
            std::fprintf(dumpFile, "}");                                                      \
            std::fprintf(dumpFile, "^{");                                                     \
            dumpTreeToTextRecursively(dumpFile, node->right, latex);                                    \
            std::fprintf(dumpFile, "}");                                                      \
        }                                                                                     \
    } while (0)

    switch (node->data) {
        case Addition: {
            DUMP_BINARY_OPERATION_NODE("+");
            break;
        }
        case Subtraction: {
            if (latex) {
                DUMP_SUBTRACTION_NODE_LATEX;
            } else {
                DUMP_BINARY_OPERATION_NODE("-");
            }
            break;
        }
        case Multiplication: {
            if (latex) {
                DUMP_BINARY_OPERATION_NODE(" \\cdot ");
            } else {
                DUMP_BINARY_OPERATION_NODE("*");
            }
            break;
        }
        case Division: {
            if (latex) {
                DUMP_DIVISION_NODE_LATEX;
            } else {
                DUMP_BINARY_OPERATION_NODE("/");
            }
            break;
        }
        case Power: {
            if (latex) {
                DUMP_POWER_NODE_LATEX;
            } else {
                DUMP_BINARY_OPERATION_NODE("^");
            }
            break;
        }
        case Sin: {
            if (latex) {
                DUMP_UNARY_OPERATION_NODE(" \\sin ");
            } else {
                DUMP_UNARY_OPERATION_NODE("sin");
            }
            break;
        }
        case Cos: {
            if (latex) {
                DUMP_UNARY_OPERATION_NODE(" \\cos ");
            } else {
                DUMP_UNARY_OPERATION_NODE("cos");
            }
            break;
        }
        case Ln: {
            if (latex) {
                DUMP_UNARY_OPERATION_NODE(" \\ln ");
            } else {
                DUMP_UNARY_OPERATION_NODE("ln");
            }
            break;
        }
        case Exponent: {
            if (latex) {
                DUMP_UNARY_OPERATION_NODE(" \\exp ");
            } else {
                DUMP_UNARY_OPERATION_NODE("exp");
            }
        }
    }

#undef PLACE_OPENING_BRACE_FOR_PRIORITY
#undef PLACE_CLOSING_BRACE_FOR_PRIORITY
#undef DUMP_UNARY_OPERATION_NODE
#undef DUMP_BINARY_OPERATION_NODE
#undef DUMP_SUBTRACTION_NODE_LATEX
#undef DUMP_DIVISION_NODE_LATEX
#undef DUMP_POWER_NODE_LATEX
}

const char *Differentiator::nodeTypeCodeToString(Differentiator::NodeType nodeType)
{
#define IDENTIFIER_TO_STRING(identifier) case identifier: return #identifier

    switch (nodeType) {
        IDENTIFIER_TO_STRING(Constant);
        IDENTIFIER_TO_STRING(Variable);
        IDENTIFIER_TO_STRING(Operation);
    }

#undef IDENTIFIER_TO_STRING
}

#define BEGIN_EXPRESSION_CONVERSION(root, comment)            \
    do {                                                                 \
    std::fprintf(dumpLatexFile, "Obviously, node MATH expression:\\\n"); \
    dumpTreeToLatex(root);                                               \
    std::fprintf(dumpLatexFile, "is equivalent to the following (%s):\\\n", (comment));  \
    } while (0)
#define END_EXPRESSION_CONVERSION(root) \
    dumpTreeToLatex((root))

bool Differentiator::optimizeNeutralElementNodesRecursively(BinaryTree::Node **node)
{
    if (*node == nullptr) return false;

    bool optimizedLeftSubtree = optimizeNeutralElementNodesRecursively(&(*node)->left);
    bool optimizedRightSubtree = optimizeNeutralElementNodesRecursively(&(*node)->right);

    if ((*node)->type != Operation) return optimizedLeftSubtree || optimizedRightSubtree;

    bool optimizedSubtree = false;
    switch ((*node)->data) {
        case Addition: {
            if (isConstant((*node)->left, 0)) {
                BEGIN_EXPRESSION_CONVERSION(*node, "see \"Abstract Algebra: Theory and Applications\"");

                optimizeRightNode(node);
                optimizedSubtree = true;

                END_EXPRESSION_CONVERSION(*node);
                break;
            }

            if (isConstant((*node)->right, 0)) {
                BEGIN_EXPRESSION_CONVERSION(*node, "i dunno really why");

                optimizeLeftNode(node);
                optimizedSubtree = true;

                END_EXPRESSION_CONVERSION(*node);
                break;
            }

            break;
        }
        case Subtraction: {
            if (isConstant((*node)->right, 0)) {
                BEGIN_EXPRESSION_CONVERSION(*node, "U WOT M8?");

                optimizeLeftNode(node);
                optimizedSubtree = true;

                dumpTreeToLatex(*node);

                END_EXPRESSION_CONVERSION(*node);
                break;
            }

            break;
        }
        case Multiplication: {
            if (isConstant((*node)->left, 1)) {
                BEGIN_EXPRESSION_CONVERSION(*node, "many thanks to Evariste Galoi");

                optimizeRightNode(node);
                optimizedSubtree = true;

                END_EXPRESSION_CONVERSION(*node);
                break;
            }

            if (isConstant((*node)->right, 1)) {
                BEGIN_EXPRESSION_CONVERSION(*node, "multiply by one son");

                optimizeLeftNode(node);
                optimizedSubtree = true;

                END_EXPRESSION_CONVERSION(*node);
                break;
            }

            break;
        }
        case Division:
        case Power: {
            if (isConstant((*node)->right, 1)) {
                BEGIN_EXPRESSION_CONVERSION(*node, "we call node MATH power");

                optimizeLeftNode(node);
                optimizedSubtree = true;

                END_EXPRESSION_CONVERSION(*node);
                break;
            }
        }
    }

    return optimizedSubtree || optimizedLeftSubtree || optimizedRightSubtree;
}

bool Differentiator::isConstant(const BinaryTree::Node *node, int constant)
{
    return (node->type == Constant) && (node->data == constant);
}

bool Differentiator::optimizeEvaluableNodesRecursively(BinaryTree::Node *node)
{
    if (node == nullptr) return false;

    bool optimizedLeftSubtree = optimizeEvaluableNodesRecursively(node->left);
    bool optimizedRightSubtree = optimizeEvaluableNodesRecursively(node->right);

    if ((node->type != Operation) || ((node->left == nullptr) && (node->right->type != Constant)) || (node->left->type != Constant) ||
        (node->right->type != Constant)) {
        return optimizedLeftSubtree || optimizedRightSubtree;
    }

    switch (node->data) {
        case Addition: {
            BEGIN_EXPRESSION_CONVERSION(node, "trust me, just check rick astley's song");
            node->data = node->left->data + node->right->data;
            break;
        }
        case Subtraction: {
            BEGIN_EXPRESSION_CONVERSION(node, "look, i was gonna go easy on u");
            node->data = node->left->data - node->right->data;
            break;
        }
        case Multiplication: {
            BEGIN_EXPRESSION_CONVERSION(node, "i'm beginning to feel like a rap god");
            node->data = node->left->data * node->right->data;
            break;
        }
        case Division: {
            BEGIN_EXPRESSION_CONVERSION(node, "LMAO");
            node->data = node->left->data / node->right->data;
            break;
        }
        case Power: {
            BEGIN_EXPRESSION_CONVERSION(node, "OK, im out of node shit");
            node->data = std::pow(node->left->data, node->right->data);
            break;
        }
        case Sin: {
            BEGIN_EXPRESSION_CONVERSION(node, "brace yourselves, bois");
            node->data = (int) std::sin(node->right->data);
            break;
        }
        case Cos: {
            BEGIN_EXPRESSION_CONVERSION(node, "lol, how could it not be obvious");
            node->data = (int) std::cos(node->right->data);
            break;
        }
        case Ln: {
            BEGIN_EXPRESSION_CONVERSION(node, "discussed, for example, in \"Calculus Made Easy\"");
            node->data = (int) std::log(node->right->data);
            break;
        }
        case Exponent: {
            BEGIN_EXPRESSION_CONVERSION(node, "well, at least it is for me");
            node->data = (int) std::exp(node->right->data);
            break;
        }
    }

    node->type = Constant;
    std::free(node->left);
    node->left = nullptr;
    std::free(node->right);
    node->right = nullptr;

    END_EXPRESSION_CONVERSION(node);

    return optimizedLeftSubtree || optimizedRightSubtree;
}

bool Differentiator::optimizeSpecialConstantNodesRecursively(BinaryTree::Node **node)
{
    if (*node == nullptr) return false;

    bool optimizedLeftSubtree = optimizeSpecialConstantNodesRecursively(&(*node)->left);
    bool optimizedRightSubtree = optimizeSpecialConstantNodesRecursively(&(*node)->right);

    if ((*node)->type != Operation) return optimizedLeftSubtree || optimizedRightSubtree;

    bool optimizedSubtree = false;
    switch ((*node)->data) {
        case Multiplication: {
            if (isConstant((*node)->left, 0)) {
                BEGIN_EXPRESSION_CONVERSION(*node, "okay okay im literally baffled");

                zeroNode(*node);
                optimizedSubtree = true;

                END_EXPRESSION_CONVERSION(*node);

                break;
            }

            if (isConstant((*node)->right, 0)) {
                BEGIN_EXPRESSION_CONVERSION(*node, "zero divisor of the integers ring");

                zeroNode(*node);
                optimizedSubtree = true;

                END_EXPRESSION_CONVERSION(*node);

                break;
            }

            break;
        }
        case Division: {
            if (isConstant((*node)->left, 0)) {
                BEGIN_EXPRESSION_CONVERSION(*node, "i cant keep myself serious");

                zeroNode(*node);
                optimizedSubtree = true;

                END_EXPRESSION_CONVERSION(*node);

                break;
            }
        }
        case Power: {
            if (isConstant((*node)->right, 0)) {
                BEGIN_EXPRESSION_CONVERSION(*node, "you better tell me its worth it");

                optimizeLeftNode(node);
            }
        }
    }

    return optimizedSubtree || optimizedLeftSubtree || optimizedRightSubtree;
}

void Differentiator::dumpLeafNodeDataToText(std::FILE *dumpFile, const BinaryTree::Node *node)
{
    switch (node->type) {
        case Constant: {
            std::fprintf(dumpFile, "%d", node->data);
            break;
        }
        case Variable: {
            std::fprintf(dumpFile, "%c", node->data + 'a');
            break;
        }
    }
}

void Differentiator::dumpNodeDataToData(std::FILE *dumpFile, const BinaryTree::Node *node)
{
    switch (node->type) {
        case Constant: {
            std::fprintf(dumpFile, "%d", node->data);
            break;
        }
        case Variable: {
            std::fprintf(dumpFile, "%c", node->data + 'a');
            break;
        }
        case Operation: {
            std::fprintf(dumpFile, "%s", operationCodeToString((enum Operation) node->data));
            break;
        }
    }
}

void Differentiator::optimizeTree(BinaryTree::Node **root)
{
    while (true) {
        bool optimized = false;
        if (optimizeNeutralElementNodesRecursively(root)) optimized = true;
        if (optimizeEvaluableNodesRecursively(*root)) optimized = true;
        if (optimizeSpecialConstantNodesRecursively(root)) optimized = true;

        if (!optimized) break;
    }
}

void Differentiator::dumpTreeToLatex(const BinaryTree::Node *root)
{
    std::fprintf(dumpLatexFile, "\\begin{dmath}\n");
    dumpTreeToTextRecursively(dumpLatexFile, root, true);
    std::fprintf(dumpLatexFile, "\n\\end{dmath}\n");
}

void Differentiator::optimizeLeftNode(BinaryTree::Node **node)
{
    std::free((*node)->right);
    (*node)->right = nullptr;
    auto optimizedNode = (*node)->left;
    std::free(*node);
    *node = optimizedNode;
}

void Differentiator::optimizeRightNode(BinaryTree::Node **node)
{
    std::free((*node)->left);
    (*node)->left = nullptr;
    auto optimizedNode = (*node)->right;
    std::free(*node);
    *node = optimizedNode;
}

void Differentiator::zeroNode(BinaryTree::Node *node)
{
    BinaryTree::deleteRecursively(&node->left);
    BinaryTree::deleteRecursively(&node->right);
    node->type = Constant;
    node->data = 0;
}

bool Differentiator::operationPriority(const BinaryTree::Node *node1, const BinaryTree::Node *node2)
{
    if ((node1->type == Operation) && (node2->type == Operation)) {
        if ((node1->data == Multiplication) && ((node2->data == Addition) || (node2->data == Subtraction))) return true;
        if ((node1->data == Power) && (node2->data != Sin) && (node2->data != Cos) && (node2->data != Ln) &&
        (node2->data != Exponent))
            return true;
        if ((node1->data == Sin) || (node1->data == Cos) || (node1->data == Ln)) return true;
    }

    return false;
}

bool Differentiator::isFunction(const BinaryTree::Node *node)
{
    return (node->type == Operation) && ((node->data == Sin) || (node->data == Cos) || (node->data == Ln) || (node->data == Exponent));
}

