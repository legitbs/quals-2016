import random
from collections import deque
from os import environ

funcwords = ["str", "eq", "cpy", "mem", "main", "echo", "cat", "cmp", "get",
             "print", "val", "fmt", "stk", "alloc", "free", "mk", "tx", "rx",
             "xchg", "swap", "gs", "msr", "base", "acid", "caller", "binding",
             "stack", "box", "unbox", "factory", "permute", "shuffle", "rnd"]

realfuncnames = ["gets", "print", "streq", "echo"]

argtypes = ["int", "char", "long"]

iops = ["*", "-", "+"]

def makefuncname():
    return ''.join(random.sample(funcwords, 4))

def makefun():
    funcname = makefuncname()
    args = map(lambda name:
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

funcsubs = map(lambda realfunc:
               [realfunc, makefuncname()]
            , realfuncnames)

with open("./src/service.c.tpl") as f:
    with open("./src/service.c", "w") as g:
        src = f.read()
        subbed = reduce(lambda accum, pair:
                        accum.replace(pair[0], pair[1])
                        , funcsubs, src)
        segments = subbed.split("###")
        g.write(segments[0])
        bits = random.sample(segments[1:], len(segments) - 1)

        for fun in bits:
            g.write(makefun())
            g.write(fun)
        g.write(makefun())
