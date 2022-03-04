import subprocess

from random import choice, randrange
from sys import argv


def make_add_test(max_value):
    a = randrange(-max_value, max_value)
    b = randrange(-max_value, max_value)
    r = a + b
    return f'{a}\n+\n{b}', f'{r}'


def make_sub_test(max_value):
    a = randrange(-max_value, max_value)
    b = randrange(-max_value, max_value)
    r = a - b
    return f'{a}\n-\n{b}', f'{r}'


def make_mult_test(max_value):
    a = randrange(-max_value, max_value)
    b = randrange(-max_value, max_value)
    r = a * b
    return f'{a}\n*\n{b}', f'{r}'


def make_div_test(max_value):
    a = randrange(0, max_value * max_value)
    b = randrange(1, max_value)
    r = a // b
    if randrange(2) == 0 and a != 0:
        a *= -1
        r *= -1

    if randrange(2) == 0:
        b *= -1
        r *= -1

    return f'{a}\n/\n{b}', f'{r}'


def make_mod_test(max_value):
    a = randrange(0, max_value * max_value)
    b = randrange(1, max_value)
    r = a % b
    if randrange(2) == 0 and a != 0:
        a *= -1
        r *= -1

    return f'{a}\n%\n{b}', f'{r}'


def make_test(max_value):
    return choice([
        make_add_test,
        make_sub_test,
        make_mult_test,
        make_div_test,
        make_mod_test
    ])(max_value)


def test(path, max_value):
    test_input, test_output = make_test(max_value)
    print(test_input)
    print(test_output)
    proc = subprocess.Popen(path, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    output = proc.communicate(test_input.encode())[0].decode().split('\n')[0]
    print(output)
    return test_output == output


def main(path, max_value):
    try:
        while test(path, max_value):
            print('OK\n')

        print('FAIL')

    except KeyboardInterrupt:
        print()


if __name__ == '__main__':
    if len(argv) == 2:
        main(argv[1], 10 ** 1000)

    elif len(argv) > 2:
        main(argv[1], max(int(argv[2]), 2))
