import sys

from nevec.parse.parse import Parse

if __name__ == "__main__":
    args = sys.argv

    if len(args) != 2:
        # TODO: replace this with a CLI err
        print("usage: nevec [file]")
        exit(1)

    filename = args[1]

    with open(filename) as f:
        code = f.read()
        parse = Parse(code)

        ast = parse.parse()

        print(ast)

