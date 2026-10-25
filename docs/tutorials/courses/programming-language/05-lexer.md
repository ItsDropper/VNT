# 05 — Build a Lexer

The lexer converts raw source characters into tokens. A token typically has a kind, the text or value it represents, and a source location.

For example, a statement such as `say "Hi"` could produce:

```text
KEYWORD_SAY  line 1
STRING       "Hi"  line 1
EOF          line 1
```

The exact token names are an implementation choice. Consistency matters more than naming style.

## Token representation

In VNT, you can represent each token with an object containing fields such as `kind`, `text`, and `line`. If you use a linked list, include a `next` field. If you use arrays, verify the VNT array operations you need before designing the whole lexer around them.

Do not assume a VNT feature exists because another language has it. Check the VNT examples and compiler tests, then write a tiny experiment.

## Scanning identifiers

Start at a letter or underscore, then consume letters, digits, or underscores. After collecting the text, compare it against the keyword list. If it matches a keyword, emit the keyword token; otherwise emit an identifier token.

## Scanning strings

On an opening quote, remember the starting line, then consume characters until the matching unescaped quote. Handle escapes according to your specification. If the file ends first, report an unterminated string at the opening line.

## Errors and EOF

Always emit an end-of-file token. This lets the parser detect the end of input without indexing beyond the token collection.

A lexer should reject unknown punctuation, unterminated strings, and any configured token/source limit. Do not silently discard bad characters.

## Exercise

Add a token-dump mode or temporary debug output. Feed it one valid statement, one comment, one escaped string, and one invalid character. Compare the output to your predictions.

## Checkpoint

Why should the lexer distinguish the two-character operator `==` from two adjacent assignment tokens?
