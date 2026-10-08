#include <vnt/ir.h>

static int fold(AstNode *n, int *changed) {
    if (!n) return 0;

    switch (n->type) {
        case AST_BINARY_EXPRESSION: {
            fold(n->binary_expression.left, changed);
            fold(n->binary_expression.right, changed);

            AstNode *left = n->binary_expression.left;
            AstNode *right = n->binary_expression.right;

            if (left && right &&
                left->type == AST_INTEGER_LITERAL &&
                right->type == AST_INTEGER_LITERAL) {
                int a = left->integer_literal.value;
                int b = right->integer_literal.value;
                int value = 0;
                int valid = 1;

                switch (n->binary_expression.operator) {
                    case BINARY_ADD: value = a + b; break;
                    case BINARY_SUBTRACT: value = a - b; break;
                    case BINARY_MULTIPLY: value = a * b; break;
                    case BINARY_DIVIDE:
                        if (!b) valid = 0;
                        else value = a / b;
                        break;
                    case BINARY_MODULO:
                        if (!b) valid = 0;
                        else value = a % b;
                        break;
                    default: valid = 0; break;
                }

                if (valid) {
                    AstNode *replacement = ast_create_integer(value);
                    if (!replacement) return 0;
                    ast_free(left);
                    ast_free(right);
                    n->binary_expression.left = NULL;
                    n->binary_expression.right = NULL;
                    n->type = replacement->type;
                    n->integer_literal.value = replacement->integer_literal.value;
                    free(replacement);
                    *changed = 1;
                }
            }
            return 0;
        }

        case AST_UNARY_EXPRESSION:
            fold(n->unary_expression.operand, changed);
            return 0;

        case AST_ARRAY_LITERAL:
            for (AstNode *e = n->array_literal.elements; e; e = e->next)
                fold(e, changed);
            return 0;

        case AST_INDEX_EXPRESSION:
            fold(n->index_expression.array, changed);
            fold(n->index_expression.index, changed);
            return 0;

        case AST_MEMBER_EXPRESSION:
            fold(n->member_expression.object, changed);
            return 0;

        case AST_ASSIGNMENT:
            fold(n->assignment.target, changed);
            fold(n->assignment.value, changed);
            return 0;

        case AST_VARIABLE_DECLARATION:
            fold(n->variable_declaration.value, changed);
            return 0;

        case AST_FUNCTION_CALL:
            for (AstNode *a = n->function_call.arguments; a; a = a->next)
                fold(a, changed);
            return 0;

        case AST_IF_STATEMENT:
            fold(n->if_statement.condition, changed);
            fold(n->if_statement.then_branch, changed);
            fold(n->if_statement.else_branch, changed);
            return 0;

        case AST_WHILE_STATEMENT:
            fold(n->while_statement.condition, changed);
            fold(n->while_statement.body, changed);
            return 0;

        case AST_FUNCTION_DECLARATION:
            fold(n->function_declaration.body, changed);
            return 0;

        case AST_RETURN_STATEMENT:
            fold(n->return_statement.expression, changed);
            return 0;

        default:
            return 0;
    }
}

int vnt_ir_lower(VntIrProgram *ir, AstNode *program) {
    if (!ir || !program || program->type != AST_PROGRAM)
        return 0;

    ir->program = program;
    ir->optimized_nodes = 0;
    return 1;
}

int vnt_ir_optimize(VntIrProgram *ir) {
    if (!ir || !ir->program)
        return 0;

    int changed = 0;
    fold(ir->program->program.statements, &changed);
    ir->optimized_nodes = changed;
    return 1;
}
