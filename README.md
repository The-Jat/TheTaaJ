
# The TaaJ

## Building Guide

Follow these steps to build TheTaaJ from source:

1. **Clone the Repository**:

```sh
git clone https://github.com/The-Jat/TheTaaJ.git
```

2. **Prerequisites**:

You need the following utilities:
- NASM (The Netwide Assembler)
- QEMU (Quick Emulator)

You can install NASM and QEMU using the package manager for your operating system. For example, on Ubuntu, you can use `apt`:

```sh
sudo apt-get install build-essential
sudo apt-get install git
sudo apt-get install nasm
sudo apt-get install qemu
```

3. **Compile the Bootloader**:

To compile the bootloader, navigate to the root of the repository and run:

```sh
make all
```

4. **Test the Bootloader**: 

You can test the compiled bootloader using QEMU. Run the following command:

```sh
make run
```

5. **Clean Up the Build**:
```sh
make clean
```
