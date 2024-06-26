# OpenVG

OpenVG build steps for RT1170 targets:
1. Edit the `build-rt1170.sh` script and update the relevant paths and build target (`SDRAM_RELEASE` or `SDRAM_DEBUG`).
2. Run the script.
3. The libraries are generated in the `__build` directory.

OpenVG build steps for Linux targets:
1. Edit the `build_linux.sh` script and update the `KERNEL_DIR` and `VG_LITE_SDK` paths if needed.
2. Run the script to build OpenVG for your target. FPGA example:
```
./build_linux.sh PCIE-GEN6
```
3. The libraries are generated in the `build` directory. The VGLite dependencies are also copied to the same location.
