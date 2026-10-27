#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build

sources=(
  src/main.c src/modules.c src/lexer.c
  src/parser/parser.c src/parser/primary.c src/parser/unary.c
  src/parser/binary.c src/parser/functions.c src/parser/statements.c
  src/parser/simple_statements.c src/parser/control.c src/parser/loops.c
  src/parser/assignments.c src/ast.c src/compiler/compiler.c
  src/compiler/typecheck.c src/compiler/ir.c src/compiler/ir_lower.c
  src/compiler/cfg.c src/compiler/x86_backend.c src/compiler/freestanding_x86.c
)
gcc -O2 -std=gnu11 -Iinclude -Isrc/compiler "${sources[@]}" -o build/vnt
./build/vnt --emit-freestanding-asm os/kernel.vnt -o build/kernel.s
gcc -c -m64 -ffreestanding -fno-pic -fno-stack-protector -mno-red-zone build/kernel.s -o build/kernel.o
gcc -c -m64 -ffreestanding -fno-pic -fno-stack-protector -mno-red-zone os/arch/x86_64/boot.S -o build/boot.o
gcc -c -m64 -ffreestanding -fno-pic -fno-stack-protector -mno-red-zone os/arch/x86_64/interrupts.S -o build/interrupts.o
ld -nostdlib -static -z max-page-size=0x1000 -T os/linker.ld -o build/kernel.elf build/boot.o build/kernel.o build/interrupts.o

if command -v grub-file >/dev/null 2>&1; then
  grub-file --is-x86-multiboot2 build/kernel.elf
fi

if command -v grub-mkrescue >/dev/null 2>&1 && command -v xorriso >/dev/null 2>&1; then
  mkdir -p build/iso/boot/grub
  cp build/kernel.elf build/iso/boot/kernel.elf
  cp os/grub.cfg build/iso/boot/grub/grub.cfg
  grub-mkrescue -o build/vntos.iso build/iso
  echo "Bootable ISO created: build/vntos.iso"
else
  echo "Kernel created: build/kernel.elf"
  echo "Install grub-pc-bin grub-common xorriso to create build/vntos.iso."
fi
