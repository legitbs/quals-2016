import random
from collections import deque
from os import environ

funcwords = ["str", "eq", "cpy", "mem", "main", "echo", "cat", "cmp", "get",
             "print", "val", "fmt", "stk", "alloc", "free", "mk", "tx", "rx"]

argtypes = ["int", "char", "long"]

iops = ["*", "-", "+"]

def makefun():
    funcname = ''.join(random.sample(funcwords, 4))
    args = map(lambda(name):
               random.choice(argtypes) +
               ("*" * random.randint(0, 3)) +
               " " +
               name
            , random.sample(funcwords, 3))
    ret = random.choice(argtypes)

    buf = deque()

    buf.append("{} {}({}) {}".format(ret, funcname, ", ".join(args), "{"))
    buf.append("{} x = {};".format(ret, random.randint(-10, 10)))
    for i in xrange(random.randint(0, 40)):
        op = random.choice(iops)
        buf.append("x = x {} {};".format(op, random.randint(-30, 30)))
    buf.append("return x;}")

    return "\n".join(buf)

random.seed(environ['INTERSTITIAL_SEED'])

with open("./src/service.c.tpl") as f:
    with open("./src/service.c", "w") as g:
        for line in f:
            if "###" == line.strip():
                g.write(makefun())
            else:
                g.write(line)
