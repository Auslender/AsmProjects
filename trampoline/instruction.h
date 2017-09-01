#ifndef TRAMPOLINE_INSTRUCTION_H
#define TRAMPOLINE_INSTRUCTION_H

struct instruction {

    instruction(void* code) :
        code(code), pcode((char*)code)
    {};

    void add(const char* instruction) {
        for (const char *i = instruction; *i; i++) {
            *(pcode++) = *i;
        }
    }

    void add(const char* instruction, void* obj) {
        add(instruction);
        *(void**) pcode = obj;
        pcode += 8;
    }

    void add(const char* instruction, int32_t offset) {
        add(instruction);
        *(int32_t *) pcode = offset;
        pcode += 4;
    }

    char* get_ip() {
        return pcode;
    }

    void* jump_offset(size_t sz) {
        void* ip1 = pcode;
        pcode += sz;
        return ip1;
    }


private:
    void* code;
    char* pcode;
};

#endif //TRAMPOLINE_INSTRUCTION_H
