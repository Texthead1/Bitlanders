def strip_comments(line):
    return line.split(";")[0].strip()

def line_is_empty(line):
    return not line.strip()

def tokenize(line):
    line = strip_comments(line)
    if line_is_empty(line):
        return []
    return line.split()

def tokens_empty(tokens):
    return not tokens

def token_amount_not_expected(tokens, expected):
    return len(tokens) != expected