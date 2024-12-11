from typing import List

from nevec.err.err import Err
from nevec.lex.tok import Loc

class Report:
    file_name: str
    lines: List[str]

    @staticmethod
    def setup(file_name: str, lines: List[str]):
        Report.file_name = file_name
        Report.lines = lines

    @staticmethod
    def err(msg: str, loc: Loc) -> Err:
        return Err(
            Report.file_name,
            Report.lines,
            msg,
            loc
        )
