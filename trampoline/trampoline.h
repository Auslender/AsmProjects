#ifndef TRAMPOLINE_TRAMPOLINE_H
#define TRAMPOLINE_TRAMPOLINE_H

#include <unistd.h>
#include <sys/mman.h>
#include <cstdio>
#include <sys/user.h>

#include "argument_classes.h"
#include "bytecode.h"
#include "instruction.h"

template <typename T>
struct trampoline;

const char* args_shift[5] = {
        mov_rsi_rdi,
        mov_rdx_rsi,
        mov_rcx_rdx,
        mov_r8_rcx,
        mov_r9_r8
};

struct allocator {
private:
    static char** code;
public:
    static void* get() {
        if (code == nullptr) {
            code = (char**)mmap(0, 4096, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        }
        void* ret = (void*)code;
        code = (char**)* code;
        return ret;
    }
    static void free(void* ptr) {
        *(void**)ptr = (void *) code;
        code = (char**) ptr;
    }
};

char** allocator::code = nullptr;

template <typename R, typename ... Args>
struct trampoline<R (Args ...)>
{
    template <typename F>
    trampoline(F const& func)
            : func_obj(new F(func))
            , caller(&do_call<F>),
              deleter(&do_delete<F>)
    {
        code = allocator::get();
        instruction frame(code);

        /**
         *  x86_64 ABI declares, that first 6 arguments of INTEGER class
         *  are passed in registers : rdi (1st arg), rsi (2nd arg), rdx, rcx, r8, r9.
         *  So, to pass func_obj* as the first argument of do_call,
         *  we should free rdi register by shifting func_obj arguments right.
        **/

        if (argument_classes<Args...>::INTEGER < 6) {
            for (int i = argument_classes<Args...>::INTEGER - 1; i >= 0; i--) {
                frame.add(args_shift[i]);
            }
            frame.add(mov_rdi_imm, func_obj); //now we can put func_obj to rdi
            frame.add(mov_rax_imm, (void*) caller);
            frame.add(jmp_rax);
        } else {
            /**
             * 6 INTEGER arguments
             * 8 SSE arguments (%xmm0...%xmm7)
             * all the rest args are on stack
             */
            int stack_size = 8 * (argument_classes<Args...>::INTEGER - 6 +
                              std::max(argument_classes<Args...>::SSE - 8, 0));
            frame.add(mov_r11_val_rsp); //put return address to r11 (when we return from this function call frame, we will put this return address onto the bottom of the stack again)
            frame.add(push_r9);         //push 6th argument on stack
            for (int i = 4; i >= 0; i--) {
                frame.add(args_shift[i]); //and shift 5 other arguments again, to free rdi for func_obj
            }

            frame.add(mov_rax_rsp);
            frame.add(add_rax_imm, stack_size + 8); //rax points to the last argument on stack
            frame.add(add_rsp_imm, 8); //rsp points to return address

            //.loop for shifting all the arguments down
            char* label_1 = frame.get_ip();
            frame.add(cmp_rax_rsp); // all arguments are shifted
            frame.add(je); //if true, go to label_2

            char* label_2 = (char*)frame.jump_offset(1);

            //shifting
            frame.add(add_rsp_imm, 8);
            frame.add(mov_rdi_val_rsp);
            frame.add(mov_val_sub_rsp_8_rdi);
            frame.add(jmp);

            char* loop = (char*) frame.jump_offset(1);
            *loop = (char) (label_1 - frame.get_ip());
            *label_2 = (char) (frame.get_ip() - label_2 - 1);

            frame.add(mov_val_rsp_r11); //put saved return address onto the bottom of the stack again
            frame.add(sub_rsp_imm, stack_size + 8); //rsp points to the top of the stack
            frame.add(mov_rdi_imm, func_obj); //func_obj is the first argument of do_call now
            frame.add(mov_rax_imm, (void*) caller); //rax points to do_call

            frame.add(call_rax); //call the function
            frame.add(pop_r9); //remove 6th argument from stack
            frame.add(mov_r11_val_add_rsp_stack_size, stack_size);
            frame.add(mov_val_rsp_r11);
            frame.add(ret);
        }

    }

    R (*get() const)(Args... args)
    {
        return (R (*)(Args...))code;
    }

    trampoline(trampoline&& other) {
        func_obj = other.func_obj;
        code = other.code;
        deleter = other.deleter;
        other.func_obj = nullptr;
    }

    trampoline& operator=(trampoline&& other) {
        func_obj = other.func_obj;
        code = other.code;
        deleter = other.deleter;
        other.func_obj = nullptr;
        return *this;
    }

    ~trampoline() {
        if (func_obj) {
            deleter(func_obj);
        }
        allocator::free(code);
    }

private:

    template <typename F>
    static R do_call(void* obj, Args... args)
    {
        return (*(F*)obj)(args ...);
    }

    template <typename F>
    static void do_delete(void* func_obj) {
        delete (F*)func_obj;
    }

    void* func_obj;
    void* code;
    R (*caller)(void* obj, Args... args);
    void (*deleter) (void* obj);
};

#endif //TRAMPOLINE_TRAMPOLINE_H
