#include "interpreter_internal.h"

#include <stdio.h>

Value execute_function_call(
    AstNode *call,
    Environment *environment
) {
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

        Value invalid = {0};
        return invalid;
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

        Value invalid = {0};
        return invalid;
    }

    Environment local_environment;
    environment_init(&local_environment);

    /*
     * Functions are visible inside other functions.
     */
    for (
        int i = 0;
        i < environment->function_count;
        i++
    ) {
        if (!environment_define_function(
                &local_environment,
                environment->functions[i].declaration
            )) {
            environment_free(&local_environment);

            printf(
                "Runtime error: could not create function environment.\n"
            );

            Value invalid = {0};
            return invalid;
        }
    }

    AstNode *argument =
        call->function_call.arguments;

    for (int i = 0; i < actual; i++) {
        Value value =
            evaluate_expression(
                argument,
                environment
            );

        if (value.type == VALUE_INVALID) {
            environment_free(&local_environment);
            return value;
        }

        if (!environment_define(
                &local_environment,
                function->function_declaration.parameters[i],
                value
            )) {
            value_free(&value);
            environment_free(&local_environment);

            printf(
                "Runtime error: could not store function argument.\n"
            );

            Value invalid = {0};
            return invalid;
        }

        argument = argument->next;
    }

    AstNode *statement =
        function->function_declaration.body;

    while (statement != NULL) {
        ExecutionResult result =
            execute_node(
                statement,
                &local_environment
            );

        if (result.returned) {
            Value returned_value =
                copy_value(&result.value);

            value_free(&result.value);
            environment_free(&local_environment);

            return returned_value;
        }

        statement = statement->next;
    }

    environment_free(&local_environment);

    /*
     * A function that reaches the end without
     * returning has no value.
     */
    Value invalid = {0};
    return invalid;
}