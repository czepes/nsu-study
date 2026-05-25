void _start(void) {
  asm volatile("syscall" ::"a"(1), "D"(1), "S"("Hello, World!"), "d"(13));
  asm volatile("syscall" ::"a"(60), "D"(0));
}
