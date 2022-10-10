#!/bin/python3
import subprocess
import sys, getopt

def findnth(string, substring, n):
    parts = string.split(substring, n + 1)
    if len(parts) <= n + 1:
        return -1
    return len(string) - len(parts[-1]) - len(substring)

def main(argv):

    toshow = False

    try:
        arg = argv[0]

    except:
        print("Bad argument.\ncoverage-check.py <dir> [--show]")
        sys.exit(2)

    for elem in argv[1:]:
        if elem == "--show":
            toshow = True
        elif elem == "-h":
            print("coverage-check.py <dir> [--show]")
            sys.exit(2)
        else:
            print("Bad argument.\ncoverage-check.py <dir> [--show]")
            sys.exit(2)

    if (arg[-1] == '/'):
        arg = arg[:-1]

    name = arg[arg.rfind('/') + 1:]

    #print("llvm-profdata merge -o out.profdata " + arg + "/default.profraw")
    subprocess.check_output("llvm-profdata merge -o out.profdata " + arg + "/default.profraw", shell=True, universal_newlines=True)
    #print("llvm-cov report " + arg + '/' + name + " --instr-profile=out.profdata")
    report = subprocess.check_output("llvm-cov report " + arg + '/' + name + " --instr-profile=out.profdata", shell=True, universal_newlines=True)

    result = ""
    for line in report.split('\n'):
        if line.count("------") or line.count(name + '/' + name.lower() + '.h'):
            result += line + '\n'
        if line.count("Filename") or line.count("TOTAL"):
            result += line + '\n'


    if (toshow):
        show = subprocess.check_output("llvm-cov show " + arg + '/' + name + " --instr-profile=out.profdata", shell=True, universal_newlines=True)
        result += '\n\n\n'
        result += show[show.find('/' + name + '/' + name.lower() + ".h"):]

    subprocess.check_output("rm ./out.profdata", shell=True, universal_newlines=True)
    print(result)

if __name__ == "__main__":
    main(sys.argv[1:])
