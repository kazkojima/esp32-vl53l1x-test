STM32CUBEEXPANSION_53L1A1 = symlink-STM32CubeExpansion_53L1A1_V2.0.1

COMPONENT_SRCDIRS := . \
	$(STM32CUBEEXPANSION_53L1A1)/Drivers/BSP/Components/vl53l1x

COMPONENT_ADD_INCLUDEDIRS = \
	$(STM32CUBEEXPANSION_53L1A1)/Drivers/BSP/X-NUCLEO-53L1A1 \
	$(STM32CUBEEXPANSION_53L1A1)/Drivers/BSP/Components/vl53l1x \
	$(STM32CUBEEXPANSION_53L1A1)/Projects/STM32L476RG-Nucleo/Examples/53L1A1/SimpleRanging/Inc

CFLAGS += -Wno-unused-variable -Wno-maybe-uninitialized -DI2C_HandleTypeDef=int
