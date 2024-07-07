
-include $(BL60X_SDK_PATH)/components/network/ble/ble_common.mk

COMPONENT_ADD_INCLUDEDIRS := ../../HomeAssistant-C
COMPONENT_ADD_INCLUDEDIRS += at_command device_state flash homeAssistant  ir_device wifi button led sht30 blufi_config blufi

COMPONENT_SRCDIRS := ../../HomeAssistant-C
COMPONENT_SRCDIRS += at_command device_state flash homeAssistant  ir_device wifi button led sht30 blufi_config blufi

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
