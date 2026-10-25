# 04 — Source Text and Character Positions

A lexer begins with source text, usually a string loaded from a file. Its job is to inspect the text from left to right and recognize meaningful units.

A useful lexer tracks a current position. At each step it should either:

1. skip whitespace;
2. skip a comment;
3. recognize an identifier or keyword;
4. recognize a number;
5. recognize a quoted string;
6. recognize punctuation or an operator;
7. report an unexpected character.

Every successful branch must advance the position. Otherwise the lexer can become stuck forever on one character.

## Lookahead

Some tokens share a first character. For example, `=` and `==` are different tokens. The lexer looks at the current character and, when needed, the next character before deciding which token to emit.

This is called lookahead. It does not require a complicated parser; one character of lookahead is enough for many operators.

## Strings

A string scanner must distinguish the closing quote from an escaped quote. If your language supports escapes such as `\n` and `\t`, specify exactly how they are translated.

Track line numbers while consuming newlines, including newlines inside strings if your language permits them. Error locations are far more useful than a generic “invalid program” message.

## Exercise

Write a table for every character category in your language and state how the scanner handles it. Trace a sample input one character at a time.

## Checkpoint

What happens if a lexer branch detects an invalid character but does not advance the input position?
