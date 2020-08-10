#======================================================================#
#Output files
EXECUTABLE=stm32_executable.elf
BIN_IMAGE=stm32_bin_image.bin

#======================================================================#
#Cross Compiler
CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

#======================================================================#
#Flags
CFLAGS=-g -mlittle-endian -mthumb \
	-mcpu=cortex-m4 \
	-mfpu=fpv4-sp-d16 -mfloat-abi=hard \
	--specs=nano.specs \
	--specs=nosys.specs \
	-u _printf_float
CFLAGS+=-D USE_STDPERIPH_DRIVER
CFLAGS+=-D STM32F4xx
CFLAGS+=-D __FPU_PRESENT=1 \
        -D ARM_MATH_CM4 \
	#-D __FPU_USED=1
CFLAGS+=-I./

LDFLAGS+=-Wl,--start-group -lm -Wl,--end-group

#stm32-flash
CFLAGS+=-Wl,-T,stm32_flash.ld

#======================================================================#
#lib

#Stm32 libraries
ST_LIB=./lib/STM32F4xx_StdPeriph_Driver
CFLAGS+=-I./lib/CMSIS/ST/STM32F4xx/Include

#CMSIS libraries
CFLAGS+=-I./lib/CMSIS/Include

#StdPeriph includes
CFLAGS+=-I$(ST_LIB)/inc

#SOURCE
CFLAGS+=-I./common
CFLAGS+=-I./core/estimators
CFLAGS+=-I./core/controller
CFLAGS+=-I./drivers/device
CFLAGS+=-I./drivers/interface
CFLAGS+=-I./drivers/perigh

#======================================================================#
#Source code
SRC=./lib/CMSIS/system_stm32f4xx.c

# DSP_LIB
SRC+=lib/CMSIS/DSP_Lib/Source/CommonTables/arm_common_tables.c \
	lib/CMSIS/DSP_Lib/Source/FastMathFunctions/arm_cos_f32.c \
	lib/CMSIS/DSP_Lib/Source/FastMathFunctions/arm_sin_f32.c \
	lib/CMSIS/DSP_Lib/Source/StatisticsFunctions/arm_power_f32.c \
	lib/CMSIS/DSP_Lib/Source/StatisticsFunctions/arm_max_f32.c \
	lib/CMSIS/DSP_Lib/Source/BasicMathFunctions/arm_sub_f32.c \
	lib/CMSIS/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_f32.c \
	lib/CMSIS/DSP_Lib/Source/SupportFunctions/arm_copy_f32.c \
	lib/CMSIS/DSP_Lib/Source/MatrixFunctions/arm_mat_init_f32.c \
	lib/CMSIS/DSP_Lib/Source/MatrixFunctions/arm_mat_scale_f32.c \
	lib/CMSIS/DSP_Lib/Source/MatrixFunctions/arm_mat_add_f32.c \
	lib/CMSIS/DSP_Lib/Source/MatrixFunctions/arm_mat_sub_f32.c \
	lib/CMSIS/DSP_Lib/Source/MatrixFunctions/arm_mat_mult_f32.c \
	lib/CMSIS/DSP_Lib/Source/MatrixFunctions/arm_mat_trans_f32.c \
	lib/CMSIS/DSP_Lib/Source/MatrixFunctions/arm_mat_inverse_f32.c

#StdPeriph
SRC+=$(ST_LIB)/src/misc.c \
	$(ST_LIB)/src/stm32f4xx_rcc.c \
	$(ST_LIB)/src/stm32f4xx_dma.c \
	$(ST_LIB)/src/stm32f4xx_flash.c \
	$(ST_LIB)/src/stm32f4xx_gpio.c \
	$(ST_LIB)/src/stm32f4xx_usart.c \
	$(ST_LIB)/src/stm32f4xx_tim.c\
	$(ST_LIB)/src/stm32f4xx_spi.c\
	$(ST_LIB)/src/stm32f4xx_i2c.c

#Major programs
SRC+=./main.c \
     ./common/delay.c \
     ./drivers/interface/imu.c \
     ./core/estimators/lpf.c \
     ./drivers/device/mpu6500.c \
     ./drivers/perigh/spi.c \
     ./drivers/perigh/uart.c \
     ./drivers/perigh/timer.c \
     ./core/estimators/madgwick_ahrs.c \
     ./common/quaternion.c \
     ./common/se3_math.c \
     ./core/estimators/ahrs.c \
     ./core/controller/pid_control.c \
     ./common/matrix.c \
     ./drivers/perigh/gpio.c \
     ./drivers/perigh/pwm.c \
     ./drivers/device/motor.c

#======================================================================#
#STM32 startup file
STARTUP=./startup_stm32f4xx.s

#======================================================================#
#Make rules

OBJS=${SRC:.c=.o}

#Make all
all:$(BIN_IMAGE)

$(BIN_IMAGE):$(EXECUTABLE)
	$(OBJCOPY) -O binary $^ $@

STARTUP_OBJ = startup_stm32f4xx.o

$(STARTUP_OBJ): $(STARTUP)
	$(CC) $(CFLAGS) $^ -c $(STARTUP)

$(EXECUTABLE):$(OBJS) $(STARTUP_OBJ)
	$(CC) $(CFLAGS) ${OBJS} ${STARTUP_OBJ} ${LDFLAGS} -o $@

%.o: %.s
	${CC} $(CFLAGS) $^ $(LDFLAGS) -c $<

%.o: %.c
	$(CC) $(CFLAGS) -c $< $(LDFLAGS) -o $@

#Make clean
clean:
	rm -rf $(EXECUTABLE)
	rm -rf $(BIN_IMAGE)
	rm -rf ${OBJS}
#Make flash
flash:
	st-flash write $(BIN_IMAGE) 0x8000000

#======================================================================
.PHONY:all clean flash
