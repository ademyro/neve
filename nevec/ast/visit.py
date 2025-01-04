from typing import Any

class Visit[T, U]:
    def visit(self, node: T, *extra_data: Any) -> U:
        method_name = "visit_" + type(node).__name__
        method = getattr(self, method_name, None)

        if method is None:
            raise TypeError(
                f"Visit error: no visit method for {type(node).__name__}"
            )

        return method(node, *extra_data) 
