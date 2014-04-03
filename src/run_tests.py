# coding: UTF-8
import os
import subprocess
import re

__author__ = "Ryannnnnnn"


START_DIR   = "tests"
LEXER       = "lexer"

def test_lexer(path):
    lexer = os.path.abspath(LEXER)
    output = subprocess.check_output("{} {}".format(lexer, path),
                        stderr=subprocess.STDOUT, shell=True)
    result = re.findall("successful", output)  # This is a trick! Be careful

    errfile = "".join(path.split(".")[:-1]) + ".err"
    if os.path.isfile(errfile):
        return not result
    else:
        return result


def run_tests():
    """Finding tests(pcat files) and test them"""
    start_dir = os.path.abspath(START_DIR)
    paths = os.listdir(start_dir)

    correct = total = 0
    for path in paths:
        if path.endswith(".pcat"):
            total += 1
            full_path = os.path.join(start_dir, path)
            if test_lexer(full_path):
                correct += 1
            else:
                print "File '{}' tested uncorrect!".format(path)

    print "run {total} tests, {correct}/{total} correct".format(correct=correct, total=total)


if __name__ == "__main__":
    run_tests()
