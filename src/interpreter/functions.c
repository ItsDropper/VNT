#include "interpreter_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Value invalid_value(void) {
    Value invalid = {0};
    return invalid;
}

static Value builtin_input(
    AstNode *call,
    Environment *environment
) {
    if (call->function_call.argument_count != 1) {
        printf(
            "Runtime error: input() expects 1 argument.\n"
        );

        return invalid_value();
    }

    Value prompt =
        evaluate_expression(
            call->function_call.arguments,
            environment
        );

    if (prompt.type == VALUE_INVALID) {
        return prompt;
    }

    if (prompt.type != VALUE_STRING) {
        value_free(&prompt);

        printf(
            "Runtime error: input() requires a string prompt.\n"
        );

        return invalid_value();
    }

    printf(
        "%s",
        prompt.string != NULL
            ? prompt.string
            : ""
    );

    fflush(stdout);

    value_free(&prompt);

    char buffer[4096];

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        printf(
            "Runtime error: failed to read input.\n"
        );

        return invalid_value();
    }

    size_t length =
        strlen(buffer);

    if (
        length > 0 &&
        buffer[length - 1] == '\n'
    ) {
        buffer[length - 1] = '\0';

        length--;

        if (
            length > 0 &&
            buffer[length - 1] == '\r'
        ) {
            buffer[length - 1] = '\0';
        }
    }

    return value_string(buffer);
}

static Value builtin_mod(
    AstNode *call,
    Environment *environment
) {
    if (
        call->function_call.argument_count != 2
    ) {
        printf(
            "Runtime error: modulo expects 2 arguments.\n"
        );

        return invalid_value();
    }

    AstNode *argument =
        call->function_call.arguments;

    Value left =
        evaluate_expression(
            argument,
            environment
        );

    if (left.type == VALUE_INVALID) {
        return left;
    }

    argument =
        argument->next;

    Value right =
        evaluate_expression(
            argument,
            environment
        );

    if (right.type == VALUE_INVALID) {
        value_free(&left);
        return right;
    }

    if (
        left.type != VALUE_INTEGER ||
        right.type != VALUE_INTEGER
    ) {
        value_free(&left);
        value_free(&right);

        printf(
            "Runtime error: modulo requires integers.\n"
        );

        return invalid_value();
    }

    if (right.integer == 0) {
        value_free(&left);
        value_free(&right);

        printf(
            "Runtime error: modulo by zero.\n"
        );

        return invalid_value();
    }

    int result =
        left.integer % right.integer;

    value_free(&left);
    value_free(&right);

    return value_integer(result);
}

static Value builtin_len(
    AstNode *call,
    Environment *environment
) {
    if (
        call->function_call.argument_count != 1
    ) {
        printf(
            "Runtime error: len() expects 1 argument.\n"
        );

        return invalid_value();
    }

    Value value =
        evaluate_expression(
            call->function_call.arguments,
            environment
        );

    if (value.type == VALUE_INVALID) {
        return value;
    }

    int length;

    if (value.type == VALUE_STRING) {
        length =
            value.string != NULL
                ? (int)strlen(value.string)
                : 0;
    } else if (value.type == VALUE_ARRAY) {
        length =
            value.array.count;
    } else {
        value_free(&value);

        printf(
            "Runtime error: len() requires a string or array.\n"
        );

        return invalid_value();
    }

    value_free(&value);

    return value_integer(length);
}

static Value builtin_range(
    AstNode *call,
    Environment *environment
) {
    int count =
        call->function_call.argument_count;

    if (
        count < 1 ||
        count > 3
    ) {
        printf(
            "Runtime error: range() expects 1, 2, or 3 arguments.\n"
        );

        return invalid_value();
    }

    Value values[3];

    AstNode *argument =
        call->function_call.arguments;

    for (
        int i = 0;
        i < count;
        i++
    ) {
        values[i] =
            evaluate_expression(
                argument,
                environment
            );

        if (values[i].type == VALUE_INVALID) {
            for (int j = 0; j < i; j++) {
                value_free(&values[j]);
            }

            return invalid_value();
        }

        if (values[i].type != VALUE_INTEGER) {
            for (int j = 0; j <= i; j++) {
                value_free(&values[j]);
            }

            printf(
                "Runtime error: range() requires integer arguments.\n"
            );

            return invalid_value();
        }

        argument =
            argument->next;
    }

    int start;
    int end;
    int step;

    if (count == 1) {
        start = 0;
        end = values[0].integer;
        step = 1;
    } else if (count == 2) {
        start = values[0].integer;
        end = values[1].integer;
        step = 1;
    } else {
        start = values[0].integer;
        end = values[1].integer;
        step = values[2].integer;
    }

    for (int i = 0; i < count; i++) {
        value_free(&values[i]);
    }

    if (step == 0) {
        printf(
            "Runtime error: range() step cannot be zero.\n"
        );

        return invalid_value();
    }

    Value result =
        value_array();

    /*
     * Positive step:
     *
     * range(0, 5) -> [0, 1, 2, 3, 4]
     */
    if (step > 0) {
        for (
            int i = start;
            i < end;
            i += step
        ) {
            Value item =
                value_integer(i);

            if (
                !value_array_append(
                    &result,
                    item
                )
            ) {
                value_free(&item);
                value_free(&result);

                printf(
                    "Runtime error: could not build range.\n"
                );

                return invalid_value();
            }
        }
    } else {
        /*
         * Negative step:
         *
         * range(5, 0, -1)
         * -> [5, 4, 3, 2, 1]
         */
        for (
            int i = start;
            i > end;
            i += step
        ) {
            Value item =
                value_integer(i);

            if (
                !value_array_append(
                    &result,
                    item
                )
            ) {
                value_free(&item);
                value_free(&result);

                printf(
                    "Runtime error: could not build range.\n"
                );

                return invalid_value();
            }
        }
    }

    return result;
}

Value execute_function_call(
    AstNode *call,
    Environment *environment
) {
    /*
     * Built-in functions.
     */
    if (
        strcmp(
            call->function_call.name,
            "input"
        ) == 0
    ) {
        return builtin_input(
            call,
            environment
        );
    }

    if (
        strcmp(
            call->function_call.name,
            "mod"
        ) == 0
    ) {
        return builtin_mod(
            call,
            environment
        );
    }

    if (
        strcmp(
            call->function_call.name,
            "len"
        ) == 0
    ) {
        return builtin_len(
            call,
            environment
        );
    }

    if (
        strcmp(
            call->function_call.name,
            "range"
        ) == 0
    ) {
        return builtin_range(
            call,
            environment
        );
    }

    AstNode *function =
        environment_get_function(
            environment,
            call->function_call.name
        );

    if (function == NULL) {
        printf(
            "Runtime error: undefined function '%s'.\n",
            call->function_call.name
        );

        return invalid_value();
    }

    int expected =
        function->function_declaration.parameter_count;

    int actual =
        call->function_call.argument_count;

    if (expected != actual) {
        printf(
            "Runtime error: function '%s' expects %d argument(s), got %d.\n",
            call->function_call.name,
            expected,
            actual
        );

        return invalid_value();
    }

    Environment local_environment;

    environment_init(
        &local_environment
    );

    for (
        int i = 0;
        i < environment->function_count;
        i++
    ) {
        if (
            !environment_define_function(
                &local_environment,
                environment->functions[i].declaration
            )
        ) {
            environment_free(
                &local_environment
            );

            printf(
                "Runtime error: could not create function environment.\n"
            );

            return invalid_value();
        }
    }

    AstNode *argument =
        call->function_call.arguments;

    for (
        int i = 0;
        i < actual;
        i++
    ) {
        Value value =
            evaluate_expression(
                argument,
                environment
            );

        if (value.type == VALUE_INVALID) {
            environment_free(
                &local_environment
            );

            return value;
        }

        if (
            !environment_define(
                &local_environment,
                function->function_declaration.parameters[i],
                value
            )
        ) {
            value_free(&value);

            environment_free(
                &local_environment
            );

            printf(
                "Runtime error: could not store function argument.\n"
            );

            return invalid_value();
        }

        argument =
            argument->next;
    }

    AstNode *statement =
        function->function_declaration.body;

    while (statement != NULL) {
        ExecutionResult result =
            execute_node(
                statement,
                &local_environment,
                0
            );

        if (
            result.signal ==
            EXECUTION_RETURN
        ) {
            Value returned_value =
                copy_value(
                    &result.value
                );

            value_free(
                &result.value
            );

            environment_free(
                &local_environment
            );

            return returned_value;
        }

        if (
            result.signal ==
            EXECUTION_BREAK ||
            result.signal ==
            EXECUTION_CONTINUE
        ) {
            value_free(
                &result.value
            );

            environment_free(
                &local_environment
            );

            printf(
                "Runtime error: loop control statement escaped function.\n"
            );

            return invalid_value();
        }

        statement =
            statement->next;
    }

    environment_free(
        &local_environment
    );

    return invalid_value();
}