from sys import stderr

from nevec.ast.ast import Ast

class Visit[T]:
    def visit(self, ast: Ast) -> T:
        method_name = "visit_" + type(ast).__name__
        method = getattr(self, method_name)

        if method is None:
            raise TypeError(
                f"Visit error: no visit method for {type(ast).__name__}"
            )

        return method(self, ast) 
