# ASM flags
string(APPEND CMAKE_ASM_FLAGS " -D__STARTUP_CLEAR_BSS")
string(APPEND CMAKE_ASM_FLAGS " -D__STARTUP_INITIALIZE_NONCACHEDATA")
string(APPEND CMAKE_ASM_FLAGS " -mthumb")
string(APPEND CMAKE_ASM_FLAGS " -mcpu=cortex-m33")
string(APPEND CMAKE_ASM_FLAGS " -mapcs")
string(APPEND CMAKE_ASM_FLAGS " -mfloat-abi=hard")
string(APPEND CMAKE_ASM_FLAGS " -mfpu=fpv5-sp-d16")
string(APPEND CMAKE_ASM_FLAGS " -fdata-sections")
string(APPEND CMAKE_ASM_FLAGS " -ffreestanding")
string(APPEND CMAKE_ASM_FLAGS " -ffunction-sections")
string(APPEND CMAKE_ASM_FLAGS " -fno-builtin")
string(APPEND CMAKE_ASM_FLAGS " -fno-common")
string(APPEND CMAKE_ASM_FLAGS " -std=gnu99")
string(APPEND CMAKE_ASM_FLAGS " -Wall")

# ASM DEBUG flags
string(APPEND CMAKE_ASM_FLAGS_DEBUG " -O0")
string(APPEND CMAKE_ASM_FLAGS_DEBUG " -DDBG")
string(APPEND CMAKE_ASM_FLAGS_DEBUG " -DDEBUG")
string(APPEND CMAKE_ASM_FLAGS_DEBUG " -g")

# ASM RELEASE flags
string(APPEND CMAKE_ASM_FLAGS_RELEASE " -Os")

# C flags
string(APPEND CMAKE_C_FLAGS " -DCPU_MIMXRT798SGFOA_cm33_core0")
string(APPEND CMAKE_C_FLAGS " -DCPU_MIMXRT798SGFOA_cm33_core0=1")
string(APPEND CMAKE_C_FLAGS " -DSDK_OS_FREE_RTOS")
string(APPEND CMAKE_C_FLAGS " -DSTATIC_LINK=1")
string(APPEND CMAKE_C_FLAGS " -fdiagnostics-color")
string(APPEND CMAKE_C_FLAGS " -mthumb")
string(APPEND CMAKE_C_FLAGS " -mcpu=cortex-m33")
string(APPEND CMAKE_C_FLAGS " -mapcs")
string(APPEND CMAKE_C_FLAGS " -mfloat-abi=hard")
string(APPEND CMAKE_C_FLAGS " -mfpu=fpv5-sp-d16")
string(APPEND CMAKE_C_FLAGS " -Wall")
string(APPEND CMAKE_C_FLAGS " -Wno-enum-compare")
string(APPEND CMAKE_C_FLAGS " -fno-common")
string(APPEND CMAKE_C_FLAGS " -ffunction-sections")
string(APPEND CMAKE_C_FLAGS " -fdata-sections")
string(APPEND CMAKE_C_FLAGS " -ffreestanding")
string(APPEND CMAKE_C_FLAGS " -fno-builtin")
string(APPEND CMAKE_C_FLAGS " -std=gnu99")
string(APPEND CMAKE_C_FLAGS " -MMD")
string(APPEND CMAKE_C_FLAGS " -MP")

# C DEBUG flags
string(APPEND CMAKE_C_FLAGS_DEBUG " -O0")
string(APPEND CMAKE_C_FLAGS_DEBUG " -DDBG=1")
string(APPEND CMAKE_C_FLAGS_DEBUG " -DDEBUG=1")
string(APPEND CMAKE_C_FLAGS_DEBUG " -g")

# C RELEASE flags
string(APPEND CMAKE_C_FLAGS_RELEASE " -Os")
