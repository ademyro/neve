import test

from nevec.parse.parse import Parse

def get_repr(input: str):
    parse = Parse(input)

    ast = parse.parse()

    return str(ast)

class TestParse:
    def test_one(self):
        input = "(1 + 2 == 3)"

        repr = get_repr(input)
        
        assert repr == input

    def test_two(self):
        input = "---not --not -not (1 + 2 + 3 < (4 > 2) * 3)"

        repr = get_repr(input)

        assert repr == input

    def test_three(self):
        input = "1 bitor 2 ^ 3849348 / 23 * 9 + nil / true & false"

        repr = get_repr(input)

        assert repr == input

    def test_four(self):
        input = "1 bitor \"Hello, world!\" + -34.5 / 92 ^ \"()\""
        
        repr = get_repr(input)

        assert repr == input

    def test_five(self):
        input = "\"Hello, #{\"world!\"}  From #{\"Mars!\"}  This message took #{1.5 * 2} minutes to #{\"reach #{\"Earth\"}.\"}\""

        repr = get_repr(input)

        assert repr == input
