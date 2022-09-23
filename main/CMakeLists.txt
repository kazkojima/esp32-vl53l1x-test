idf_component_register(
	SRC_DIRS "." "./symlink-STM32CubeExpansion_53L1A1_V2.0.1/Drivers/BSP/Components/vl53l1x"

	INCLUDE_DIRS "./symlink-STM32CubeExpansion_53L1A1_V2.0.1/Drivers/BSP/X-NUCLEO-53L1A1" "./symlink-STM32CubeExpansion_53L1A1_V2.0.1/Drivers/BSP/Components/vl53l1x" "./symlink-STM32CubeExpansion_53L1A1_V2.0.1/Projects/STM32L476RG-Nucleo/Examples/53L1A1/SimpleRanging/Inc")

idf_build_set_property(COMPILE_OPTIONS "-Wno-unused-variable" "-Wno-maybe-uninitialized" APPEND)

idf_build_set_property(COMPILE_DEFINITIONS "-DI2C_HandleTypeDef=int" APPEND)
