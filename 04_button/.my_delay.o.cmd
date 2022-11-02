cmd_/linux/drive/04_button/my_delay.o := arm-linux-gnueabihf-gcc -Wp,-MD,/linux/drive/04_button/.my_delay.o.d -nostdinc -isystem /linux/bsp/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/bin/../lib/gcc/arm-linux-gnueabihf/7.5.0/include -I./arch/arm/include -I./arch/arm/include/generated/uapi -I./arch/arm/include/generated  -I./include -I./arch/arm/include/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -std=gnu89 -fno-PIE -fno-dwarf2-cfi-asm -fno-ipa-sra -mabi=aapcs-linux -mno-thumb-interwork -mfpu=vfp -funwind-tables -marm -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-int-in-bool-context -O2 --param=allow-store-data-races=0 -DCC_HAVE_ASM_GOTO -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -Wno-unused-const-variable -fomit-frame-pointer -fno-var-tracking-assignments -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=implicit-int -Werror=strict-prototypes -Werror=date-time -Werror=incompatible-pointer-types  -DMODULE  -DKBUILD_BASENAME='"my_delay"'  -DKBUILD_MODNAME='"100ask_button_drv"' -c -o /linux/drive/04_button/.tmp_my_delay.o /linux/drive/04_button/my_delay.c

source_/linux/drive/04_button/my_delay.o := /linux/drive/04_button/my_delay.c

deps_/linux/drive/04_button/my_delay.o := \
  /linux/drive/04_button/my_delay.h \

/linux/drive/04_button/my_delay.o: $(deps_/linux/drive/04_button/my_delay.o)

$(deps_/linux/drive/04_button/my_delay.o):
