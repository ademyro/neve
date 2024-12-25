from typing import override, List

from nevec.ast.ast import Expr

from nevec.err.err import *
from nevec.err.report import Report

from nevec.lex.tok import Loc


class TypeErr(Err):
    def __init__(self, msg: str, locus: Loc, *exprs: Expr):
        self.msg: str = msg
        self.locus: Loc = locus
        self.exprs: List[Expr] = list(exprs)

        if self.exprs == []:
            raise ValueError(
                "at least one Expr node should be given to a TypeErr"
            )
    
    @override
    def emit(self) -> str:
        first_expr = self.exprs[0]

        first_line = Line(first_expr.loc)
        lines = self.make_lines(self.exprs, [first_line])

        err = Report.err(
            self.msg,
            self.locus
        )

        # wrapping it all around list() because silly Python doesn't 
        # immediately interpret map() objects
        list(map(err.show, lines))
        
        return err.emit()

    def make_lines(
        self,
        exprs: List[Expr],
        lines: List[Line],
        previous_line: Optional[int]=None
    ) -> List[Line]:
        if exprs == []:
            return []

        head = exprs[0]
        current_line = head.loc.line

        previous_line = previous_line if previous_line else current_line

        if current_line != previous_line:
            line = Line(self.locus, show_previous_line=True).add(
                Note(
                    NoteType.ERR,
                    head.loc,
                    str(head.type)
                )
            )

            return self.make_lines(
                exprs[1:],
                lines + [line],
                previous_line=current_line
            ) + [line]

        last_line = lines[-1]
        last_line.add(
            Note(
                NoteType.ERR,
                head.loc,
                str(head.type)
            )
        )

        return self.make_lines(
            exprs[1:],
            lines,
            previous_line=current_line
        )


