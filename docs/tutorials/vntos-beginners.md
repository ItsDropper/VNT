# Build and try VNTOS: a beginner's tutorial

VNTOS is a tiny experimental x86-64 kernel whose main entry point is written in
VNT. It boots through GRUB's Multiboot2 protocol and opens a simple command
shell over a serial console. This is a learning kernel, not a daily-use OS.

## What you will need

Use a Linux machine or Ubuntu under WSL2. You do not need to install VNTOS on
your computer or change your boot settings: the steps below run it inside QEMU.

Install the compiler, boot-image tools, and emulator:

```bash
sudo apt-get update
sudo apt-get install --yes build-essential binutils grub-pc-bin grub-common xorriso mtools qemu-system-x86
```

Get the VNT source and enter its directory:

```bash
git clone https://github.com/ItsDropper/VNT.git
cd VNT
```

If you are following a pull request rather than the default branch, check out
that branch before building.

## Build the kernel and ISO

From the repository root, run:

```bash
bash os/build.sh
```

The script builds the VNT compiler from the repository's C sources, asks VNT's
freestanding backend to compile `os/kernel.vnt`, assembles the x86-64 boot
support in `os/boot.S`, and links the kernel without a hosted C runtime. GRUB
and xorriso then package it into a bootable ISO.

Expected artifacts:

- `build/kernel.elf` — the freestanding ELF64 kernel.
- `build/vntos.iso` — the bootable GRUB ISO.

If the script reports that the ISO tools are missing, install the packages
above and rerun it. A successful compile alone is not proof that the kernel
boots; the next step actually runs it.

## Start VNTOS in QEMU

Run the kernel with its shell attached to your terminal:

```bash
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -display none -serial stdio
```

Wait for the `VNTOS - freestanding x86-64 serial shell` banner and the
`VNTOS>` prompt. The terminal is acting as the guest's serial console. This
kernel's interactive shell is serial-based, so `-serial stdio` is intentional.

Try these commands one at a time:

| Command | What it does |
| --- | --- |
| `help` | Lists available commands. |
| `about` | Describes the kernel. |
| `echo hello` | Prints `hello`. |
| `mem` | Prints linked kernel boundaries and the current identity-map range. |
| `clear` | Sends ANSI clear-screen control codes to the terminal. |
| `halt` | Stops the guest CPU. |
| `reboot` | Requests a reset through the legacy keyboard controller. |

Press `Ctrl+C` in the terminal to stop QEMU if needed. You are only stopping the
emulator; you are not installing anything on the host computer.

## How the boot path works

1. GRUB loads the kernel ELF using the Multiboot2 protocol.
2. `os/boot.S` starts in 32-bit protected mode and builds page tables mapping
   the first 1 GiB with 2 MiB pages.
3. The bootstrap enables PAE and long mode, then jumps into 64-bit mode.
4. The generated VNT entry calls `kernel_main()` from `os/kernel.vnt`.
5. `kernel_main()` invokes the assembly serial shell.

The `mem` command prints the kernel's linker-defined start/end addresses. It
does **not** measure installed RAM. The current page-table setup maps the first
1 GiB of address space; it does **not** mean the kernel has detected or can use
1 GiB of physical RAM.

## Run the automated boot test

The repository has a dedicated GitHub Actions workflow:
[ VNTOS Boot and Serial Shell ](../../.github/workflows/vntos-boot.yml).

It builds the image, checks that the ELF is 64-bit and Multiboot2-compatible,
boots it in QEMU, waits for the shell-ready marker, sends commands over the
serial connection, and checks the resulting output plus the early boot
debug-console markers. You can also run it from the repository's **Actions**
tab with **VNTOS Boot and Serial Shell → Run workflow**.

## Current limitations

VNTOS is intentionally small. It currently has no filesystem, process/user
mode, scheduler, heap, networking stack, USB keyboard support, or general
device-driver framework. It uses a simple serial shell and an early identity
map; it is not a replacement for Windows or Linux. Keep experiments inside
QEMU rather than writing the image to a physical disk.
