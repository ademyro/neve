import test

from nevec.check.check import Check
from nevec.parse.parse import Parse

def all_ok(input) -> bool:
    parse = Parse(input)
    ast = parse.parse()

    return Check().visit(ast)

class TestCheck:
    def test_one(self):
        assert all_ok("(1)")

    def test_two(self):
        assert all_ok("(-1)")

    def test_three(self):
        assert all_ok("(true)")

    def test_four(self):
        assert all_ok("(\"hey\")")

    def test_five(self):
        assert all_ok("(\"Hello, #{\"world!\"}\")")

    def test_six(self):
        assert all_ok("(nil)")

    def test_seven(self):
        assert all_ok("1 & 4")

    def test_eight(self):
        assert all_ok("4.2 + 0.3")

    def test_nine(self):
        assert not all_ok("1 + 2.3")

    def test_ten(self):
        assert not all_ok("6.4 & 3.4")

    def test_eleven(self):
        assert not all_ok("(1\n+\n4.5)")

    def test_twelve(self):
        assert not all_ok("(1 +\n4.3)")
