import sys

from nevec.check.check import Check
from nevec.parse.parse import Parse
from nevec.ir.toir import ToIr
from nevec.ir.reg import InterferenceGraph
from nevec.compile.compile import Compile
from nevec.opt.opt import Opt

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

        had_err = Check().visit(ast)

        if had_err:
            exit(1)

        print(ast)

        toir = ToIr()

        syms = toir.syms

        ir = toir.build_ir(ast)

        print("unoptimized:")
        print("\n".join(map(str, ir)))

        opt_ir = Opt(syms).optimize(ir)
        print("optimized:")
        print("\n".join(map(str, opt_ir)))

        graph = InterferenceGraph(syms.values())

    output_file = filename.removesuffix(".neve") + ".geada"

    with open(output_file, "wb") as f:
        compile = Compile(graph)
        compile.compile(opt_ir)
    
        bytecode = compile.output(to=f)


