from nevec.ast.ast import *
from nevec.ast.visit import Visit

from nevec.err.err import Err, Line, Note, NoteType, Suggestion
from nevec.err.report import Report

class Check(Visit[bool]):
    def __init__(self, file_name: str):
        self.had_err: bool = False

    def fail(self, err: Err) -> bool:
        self.had_err = True
        err.print()

        return True

    def visit_Parens(self, parens: Parens) -> bool: 
        return self.visit(parens.expr)

    def visit_UnOp(self, un_op: UnOp) -> bool:
        expr = un_op.expr

        if un_op.op == UnOp.Op.NEG:
            if expr.type.is_num():
                return False

            un_op.type.poison()
            return self.fail(Report.err(
                # TODO: change this to 
                # "{type} doesn’t implement the `Neg` idea"
                # once we implement ideas.
                "can only negate Int or Float values",
                un_op.loc
            ).show(
                Line(expr.loc).add(
                    Note(
                        NoteType.ERR,
                        expr.loc,
                        str(expr.type)
                    )
                )
            ))

        if un_op.op == UnOp.Op.NOT:
            if expr.type == Types.BOOL:
                return False

            return self.fail(Report.err(
                # TODO: change this to 
                # "{type} doesn’t implement the `Not` idea"
                # once we implement ideas.
                "can only flip booleans",
                un_op.loc
            ).show(
                Line(expr.loc).add(
                    Note(
                        NoteType.ERR,
                        expr.loc,
                        str(expr.type)
                    )            
                 )
            ))
    
    
