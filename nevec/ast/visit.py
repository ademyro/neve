from sys import stderr

from nevec.ast.ast import Ast

class Visit[T, U]:
    def visit(self, node: T) -> U:
        method_name = "visit_" + type(node).__name__
        method = getattr(self, method_name)

        if method is None:
            raise TypeError(
                f"Visit error: no visit method for {type(node).__name__}"
            )

        return method(node) 
