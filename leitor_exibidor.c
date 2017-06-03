#include "leitor_exibidor.h"
#include "util.h"

void load_magic(ClassFile* cf,FILE* fd) {
    cf->magic = u4Read(fd);
    /*if (cf->magic != 0xcafebabe)
		EXIT; //arquivo não eh .class
    */
}

void load_versions(ClassFile* cf,FILE* fd) {
    cf->minor_version = u2Read(fd);
    cf->major_version = u2Read(fd);
}

void load_constantpool(ClassFile* cf,FILE* fd) {
    cf->constant_pool_count = u2Read(fd);
    cf->constant_pool = (cp_info*) malloc(sizeof(cp_info)*(cf->constant_pool_count-1));
    cp_info *cp;
    for(cp = cf->constant_pool;cp<cf->constant_pool+cf->constant_pool_count-1;++cp) {
        cp->tag = u1Read(fd);
        switch(cp->tag) {
            case CLASS:
                cp->info.Class_info.name_index = u2Read(fd);
            break;
            case FIELDREF:
                cp->info.Fieldref_info.class_index = u2Read(fd);
                cp->info.Fieldref_info.name_and_type_index = u2Read(fd);
            break;
            case METHOD:
                cp->info.Method_info.class_index = u2Read(fd);
                cp->info.Method_info.name_and_type_index = u2Read(fd);
            break;
            case INTERFACE:
                cp->info.Interface_info.class_index = u2Read(fd);
                cp->info.Interface_info.name_and_type_index = u2Read(fd);
            break;
            case NAMEANDTYPE:
                cp->info.NameAndType_info.name_index = u2Read(fd);
                cp->info.NameAndType_info.descriptor_index = u2Read(fd);
            break;
            case UTF8:
                cp->info.Utf8_info.length = u2Read(fd);
                cp->info.Utf8_info.bytes = (u1*)malloc(sizeof(u1)*cp->info.Utf8_info.length); //length diz o numero de bytes UTF8 desse cp_info
                u1* b;
                for(b=cp->info.Utf8_info.bytes ; b < cp->info.Utf8_info.bytes + cp->info.Utf8_info.length ; ++b) { //laco para leitura desses bytes
                    *b = u1Read(fd);
                }
            break;
            case STRING:
                cp->info.String_info.string_index = u2Read(fd);
            break;
            case INTEGER:
                cp->info.Integer_info.bytes = u4Read(fd);
            break;
            case FLOAT:
                cp->info.Float_info.bytes = u4Read(fd);
            break;
            case LONG:
                cp->info.Long_info.high_bytes = u4Read(fd); //estrutura de 64 bytes
                cp->info.Long_info.low_bytes = u4Read(fd);
            break;
            case DOUBLE:
                cp->info.Double_info.high_bytes = u4Read(fd); //estrutura de 64 bytes
                cp->info.Double_info.low_bytes = u4Read(fd);
            break;
        }
    }
}

void load_classdata(ClassFile* cf,FILE* fd) {
    cf->access_flags = u2Read(fd);
    cf->this_class = u2Read(fd);
    cf->super_class = u2Read(fd);
}

void load_interfaces(ClassFile* cf,FILE* fd) {
    cf->interfaces_count = u2Read(fd);
    cf->interfaces = (u2*) malloc(sizeof(u2)*cf->interfaces_count);
    u2* bytes;
    for(bytes = cf->interfaces;bytes<cf->interfaces+cf->interfaces_count;++bytes) {
        *bytes = u2Read(fd);
    }
}

int findtype(char* type) {
    if(!strcmp(type,"ConstantValue")) {
        return CONSTANTVALUE;
    } else if(!strcmp(type,"Code")) {
        return CODE;
    } else if(!strcmp(type,"Exceptions")) {
        return EXCEPTIONS;
    } else if(!strcmp(type,"InnerClasses")) {
        return INNERCLASSES;
    } else {
        return OTHER;
    }
}

void load_constantvalue_attr(attribute_info* att, FILE* fd) {
    att->type.ConstantValue.constantvalue_index = u2Read(fd);
}

void load_code_attr(attribute_info* att, ClassFile* cf,FILE* fd) {
    att->type.Code.max_stack = u2Read(fd);
    att->type.Code.max_locals = u2Read(fd);
    att->type.Code.code_length = u4Read(fd);
    att->type.Code.code = (u1*)malloc(sizeof(u1)*att->type.Code.code_length);
    u1* byte;
    for(byte=att->type.Code.code;byte<att->type.Code.code+att->type.Code.code_length;++byte) {
        *byte = u2Read(fd);
    }
    att->type.Code.exception_table_length = u2Read(fd);
    att->type.Code.exception_table = (exception_table_info*)malloc(sizeof(exception_table_info)*att->type.Code.exception_table_length);
    exception_table_info* exp_aux;
    for(exp_aux = att->type.Code.exception_table;exp_aux<att->type.Code.exception_table+att->type.Code.exception_table_length;++exp_aux) {
        exp_aux->start_pc = u2Read(fd);
        exp_aux->end_pc = u2Read(fd);
        exp_aux->handler_pc = u2Read(fd);
        exp_aux->catch_type = u2Read(fd);
    }
    att->type.Code.attributes_count = u2Read(fd);
    att->type.Code.attributes = (attribute_info*)malloc(sizeof(attribute_info)*att->type.Code.attributes_count);
    attribute_info* att_aux;
    for(att_aux=att->type.Code.attributes;att_aux<att->type.Code.attributes+att->type.Code.attributes_count;++att_aux) {
        load_attribute(att_aux,cf,fd);
    }
}

void load_exceptions_attr(attribute_info* att,FILE* fd) {
    att->type.Exceptions.number_of_exceptions = u2Read(fd);
    att->type.Exceptions.exception_index_table = (u2*) malloc(sizeof(u2)*att->type.Exceptions.number_of_exceptions);
    u2* bytes;
    for(bytes = att->type.Exceptions.exception_index_table; bytes<att->type.Exceptions.exception_index_table+att->type.Exceptions.number_of_exceptions;++bytes) {
        *bytes = u2Read(fd);
    }
}

void load_innerclasses_attr(attribute_info* att,FILE* fd) {
    att->type.InnerClasses.number_of_classes = u2Read(fd);
    att->type.InnerClasses.classes = (classtype_info*) malloc(sizeof(classtype_info)*att->type.InnerClasses.number_of_classes);
    classtype_info* classtype_aux;
    for(classtype_aux = att->type.InnerClasses.classes;classtype_aux<att->type.InnerClasses.classes+att->type.InnerClasses.number_of_classes;++classtype_aux) {
        classtype_aux->inner_class_info_index = u2Read(fd);
        classtype_aux->outer_class_info_index = u2Read(fd);
        classtype_aux->inner_name_index = u2Read(fd);
        classtype_aux->inner_class_access_flags = u2Read(fd);
    }
}

void load_other_attr(attribute_info* att, FILE* fd) {
    att->type.Other.bytes = (u1*) malloc(sizeof(u1)*att->attribute_length);
    u1* bytes;
    for(bytes = att->type.Other.bytes;bytes<att->type.Other.bytes+att->attribute_length;++bytes) {
        *bytes = u1Read(fd);
    }
}

void load_attribute(attribute_info* att,ClassFile* cf,FILE* fd) {
    char* type;
    att->attribute_name_index = u2Read(fd);
    att->attribute_length = u4Read(fd);
    type = (char*)malloc(sizeof(char)*cf->constant_pool[att->attribute_name_index-1].info.Utf8_info.length);
    strcpy(type,(char*)cf->constant_pool[att->attribute_name_index-1].info.Utf8_info.bytes);
    int i=findtype(type);
    switch(i) {
        case CONSTANTVALUE:
            load_constantvalue_attr(att,fd);
        break;
        case CODE:
            load_code_attr(att,cf,fd);
        break;
        case EXCEPTIONS:
            load_exceptions_attr(att,fd);
        break;
        case INNERCLASSES:
            load_innerclasses_attr(att,fd);
        break;
        case OTHER:
            load_other_attr(att,fd);
        break;
    }
}

void load_fields(ClassFile* cf,FILE* fd) {
    cf->fields_count = u2Read(fd);
    cf->fields = (field_info*)malloc(sizeof(field_info)*cf->fields_count);
    field_info* field_aux;
    for(field_aux = cf->fields;field_aux<cf->fields+cf->fields_count;++field_aux) {
        field_aux->access_flags = u2Read(fd);
        field_aux->name_index = u2Read(fd);
        field_aux->descriptor_index = u2Read(fd);
        field_aux->attributes_count = u2Read(fd);
        field_aux->attributes = malloc(sizeof(attribute_info)*field_aux->attributes_count);
        attribute_info* attribute_aux;
        for(attribute_aux=field_aux->attributes;attribute_aux<field_aux->attributes+field_aux->attributes_count;++attribute_aux) {
            load_attribute(attribute_aux,cf,fd);
        }
    }
}

void load_methods(ClassFile* cf,FILE* fd) {
    cf->method_count = u2Read(fd);
    cf->methods = (method_info*) malloc(sizeof(method_info)*cf->method_count);
    method_info* method_aux;
    for(method_aux = cf->methods;method_aux<cf->methods+cf->method_count;++method_aux) {
        method_aux->access_flags = u2Read(fd);
        method_aux->name_index = u2Read(fd);
        method_aux->descriptor_index = u2Read(fd);
        method_aux->attributes_count = u2Read(fd);
        method_aux->attributes = (attribute_info*) malloc(sizeof(attribute_info)*method_aux->attributes_count);
        attribute_info* att_aux;
        for(att_aux = method_aux->attributes;att_aux<method_aux->attributes+method_aux->attributes_count;++att_aux) {
            load_attribute(att_aux,cf,fd);
        }
    }
}

void load_attributes(ClassFile* cf, FILE* fd) {
    cf->attributes_count = u2Read(fd);
    cf->attributes = (attribute_info*) malloc(sizeof(attribute_info)*cf->attributes_count);
    attribute_info* att_aux;
    for(att_aux = cf->attributes;att_aux<cf->attributes+cf->attributes_count;++att_aux) {
        load_attribute(att_aux,cf,fd);
    }
}

ClassFile* readClass(FILE* fd) {
    ClassFile* cf = (ClassFile*) malloc(sizeof(ClassFile));
    load_magic(cf,fd);
    load_versions(cf,fd);
    load_constantpool(cf,fd);
    load_classdata(cf,fd);
    // load_interfaces(cf,fd);
    // load_fields(cf,fd);
    // load_methods(cf,fd);
    // load_attributes(cf,fd);
    return cf;
}
