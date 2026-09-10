
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
    (void)environment;

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

Value execute_function_call(
    AstNode *call,
    Environment *environment
) {
    /*
     * Built-in functions.
     *
     * These are handled before user-defined
     * functions because they do not have an
     * AstNode function declaration.
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

    /*
     * Functions are visible inside other functions.
     */
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

