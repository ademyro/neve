import sys

from nevec.lex.lex import Lex, TokType

if __name__ == "__main__":
    args = sys.argv

    if len(args) != 2:
        # TODO: replace this with a CLI err
        print("usage: nevec [file]")
        exit(1)

    filename = args[1]

    with open(filename) as f:
        code = f.read()
        lex = Lex(code)

        tok = lex.next()
        toks = [tok]

        while tok.type != TokType.EOF:
            tok = lex.next()
            toks.append(tok) 

        print(toks)

