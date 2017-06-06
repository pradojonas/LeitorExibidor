#include "leitor_exibidor.h"
#include "exibidor.h"
#include "util.h"


int main(int argc, char* argv[]) {
    char nomearquivo[1024];
    FILE* fd = NULL;
    FILE* fout = NULL;// = fopen(saidaarquivo, "w");


    fd = io_handler(argc, argv, nomearquivo, &fout);
    ClassFile* cf = readClass(fd);
    print_class(cf, nomearquivo, fout);
    shutdown(fd, fout, cf);
    return 0;
}
