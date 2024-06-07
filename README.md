
# The TaaJ

## Building Guide

Follow these steps to build TheTaaJ from source:

1. **Clone the Repository**:

git clone https://github.com/The-Jat/TheTaaJ.git

2. **Prerequisites**: 
sudo apt-get install nasm
sudo apt-get install build-essentials
sudo apt-get install mono-complete

3. **Compile the Bootloader**: 
make taaj

4. **Build the DiskUtility**: 
cd diskutility
make diskutility

5. **Generate the Image**: 
In the same diskutility directory:
make img

