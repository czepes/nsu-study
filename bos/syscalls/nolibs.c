void _start(void) {
  asm volatile("syscall" ::"a"(1),   // %rax = write
               "D"(1),               // %rdi = stdout
               "S"("Hello, World!"), // %rsi = message
               "d"(13)               // %rdx = length
  );
  asm volatile("syscall" ::"a"(60), // %rax = exit
               "D"(0));             // %rdi = EXIT_SUCCESS
}
