//for some reason _init is called when ran from xinetd but not from the command line
//uClibc's version doesn't have a ret so it dies horribly. this avois it and i don't include crti.o as a result
void _init()
{
}

void _fini()
{
}

