# Project Name
PROJECT := ch32v317-minimal

# Toolchain and debugger locations
TOOLCHAIN_ROOT ?= /home/aaron/code/MRS_Toolchain/rv_embedded_gcc/bin/
OPENOCD_ROOT ?= /home/aaron/code/MRS_OpenOCD/bin/

# WCH vendor tree
VENDOR_ROOT := ./vendor/
SRC_DIR := ./src/
CORE_DIR := $(VENDOR_ROOT)Core/
DEBUG_DIR := $(VENDOR_ROOT)Debug/
PERIPHERAL_DIR := $(VENDOR_ROOT)Peripheral/
STARTUP_DIR := $(VENDOR_ROOT)Startup/
LD_SCRIPT := $(VENDOR_ROOT)Ld/Link.ld

LWIP_ROOT = ./src/lib/lwip/src/
LWIP_QUIRKS_ROOT = ./src/quirks/lwip/src/

# Toolchain binaries
CC := $(TOOLCHAIN_ROOT)riscv-none-embed-gcc
SIZE := $(TOOLCHAIN_ROOT)riscv-none-embed-size
OBJCOPY := $(TOOLCHAIN_ROOT)riscv-none-embed-objcopy

# Project source files
SRC_FILES := $(wildcard $(SRC_DIR)*.c)
ASM_FILES := $(STARTUP_DIR)startup_ch32v30x_D8C.S

# Optional lwIP support. It is disabled by default because the project does not
# include a valid lwipopts.h and the vendor port has not been fully wired in yet.
LWIP_ENABLED ?= 1

ifeq ($(LWIP_ENABLED),1)
  # lwIP sources to compile only when explicitly enabled.
  SRC_FILES += $(LWIP_ROOT)core/def.c
  SRC_FILES += $(LWIP_ROOT)core/inet_chksum.c
  SRC_FILES += $(LWIP_ROOT)core/init.c
  SRC_FILES += $(LWIP_ROOT)core/ip.c
  SRC_FILES += $(LWIP_ROOT)core/mem.c
  SRC_FILES += $(LWIP_ROOT)core/memp.c
  SRC_FILES += $(LWIP_ROOT)core/netif.c
  SRC_FILES += $(LWIP_ROOT)core/pbuf.c
  SRC_FILES += $(LWIP_ROOT)core/raw.c
  SRC_FILES += $(LWIP_ROOT)core/stats.c
  SRC_FILES += $(LWIP_ROOT)core/sys.c
  SRC_FILES += $(LWIP_ROOT)core/tcp_in.c
  SRC_FILES += $(LWIP_ROOT)core/tcp_out.c
  SRC_FILES += $(LWIP_ROOT)core/tcp.c
  SRC_FILES += $(LWIP_ROOT)core/timeouts.c
  SRC_FILES += $(LWIP_ROOT)core/udp.c
  SRC_FILES += $(LWIP_ROOT)core/ipv4/acd.c
  SRC_FILES += $(LWIP_ROOT)core/ipv4/dhcp.c
  SRC_FILES += $(LWIP_ROOT)core/ipv4/etharp.c
  SRC_FILES += $(LWIP_ROOT)core/ipv4/icmp.c
  SRC_FILES += $(LWIP_ROOT)core/ipv4/ip4_addr.c
  SRC_FILES += $(LWIP_ROOT)core/ipv4/ip4_frag.c
  SRC_FILES += $(LWIP_ROOT)core/ipv4/ip4.c
  SRC_FILES += $(LWIP_ROOT)netif/ethernet.c
  SRC_FILES += $(LWIP_ROOT)api/api_lib.c
  SRC_FILES += $(LWIP_ROOT)api/api_msg.c
  SRC_FILES += $(LWIP_ROOT)api/err.c
  SRC_FILES += $(LWIP_ROOT)api/if_api.c
  SRC_FILES += $(LWIP_ROOT)api/netbuf.c
  SRC_FILES += $(LWIP_ROOT)api/netdb.c
  SRC_FILES += $(LWIP_ROOT)api/netifapi.c
  SRC_FILES += $(LWIP_ROOT)api/sockets.c
  SRC_FILES += $(LWIP_ROOT)core/altcp.c
  SRC_FILES += $(LWIP_ROOT)core/altcp_tcp.c
  SRC_FILES += $(LWIP_ROOT)apps/http/httpd.c
  SRC_FILES += $(LWIP_ROOT)apps/http/fs.c
  SRC_FILES += $(SRC_DIR)arch/sys_arch.c
endif

# Project includes
INCLUDES  = -I$(SRC_DIR)
INCLUDES += -I$(CORE_DIR)
INCLUDES += -I$(DEBUG_DIR)
INCLUDES += -I$(PERIPHERAL_DIR)inc/

ifeq ($(LWIP_ENABLED),1)
  INCLUDES += -I$(LWIP_ROOT)
  INCLUDES += -I$(LWIP_ROOT)include
  INCLUDES += -I$(LWIP_QUIRKS_ROOT)
endif

# Vendor sources
SRC_FILES += $(CORE_DIR)core_riscv.c
SRC_FILES += $(DEBUG_DIR)debug.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_adc.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_bkp.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_can.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_crc.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_dac.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_dbgmcu.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_dma.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_dvp.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_eth.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_exti.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_flash.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_fsmc.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_gpio.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_i2c.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_iwdg.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_misc.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_opa.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_pwr.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_rcc.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_rng.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_rtc.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_sdio.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_spi.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_tim.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_usart.c
SRC_FILES += $(PERIPHERAL_DIR)src/ch32v30x_wwdg.c

# Compiler flags
CFLAGS := -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -O2
CFLAGS += -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -g
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"
CFLAGS += $(INCLUDES)

ASMFLAGS := -x assembler-with-cpp

# Linker flags
LFLAGS := -T $(LD_SCRIPT) -u _printf_float -nostartfiles -Xlinker --gc-sections -Wl,-Map,$(PROJECT).map --specs=nano.specs --specs=nosys.specs -static

TARGET := $(PROJECT).elf
OBJECTS := $(SRC_FILES:.c=.o) $(ASM_FILES:.S=.o)

.PHONY: all clean flash gdb

all: $(TARGET) $(PROJECT).hex

$(TARGET): $(OBJECTS)
	@echo "[LD] $@"
	@$(CC) $(CFLAGS) $(LFLAGS) $^ -o $@
	@$(SIZE) $@

%.o: %.c
	@echo "[CC] $@"
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	@echo "[ASM CC] $@"
	@$(CC) $(CFLAGS) $(ASMFLAGS) -c $< -o $@

%.hex: %.elf
	@echo "[OBJCOPY] $@"
	@$(OBJCOPY) -O ihex "$*.elf" "$@"

clean:
	@rm -f $(OBJECTS) $(OBJECTS:.o=.d) $(TARGET) $(PROJECT).hex $(PROJECT).map

flash: $(TARGET)
	sudo $(OPENOCD_ROOT)openocd -f $(OPENOCD_ROOT)wch-riscv.cfg -c init -c halt -c "program $(PROJECT).hex" -c "resume 0" -c exit

gdb:
	sudo $(OPENOCD_ROOT)openocd -f $(OPENOCD_ROOT)wch-riscv.cfg
