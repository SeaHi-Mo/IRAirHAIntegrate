
COMPONENT_ADD_INCLUDEDIRS := ../../Ai-M6x_HomeAssistant-C/HomeAssistant-C
COMPONENT_ADD_INCLUDEDIRS += at_command device_state flash homeAssistant  ir_hxd019 wifi

COMPONENT_SRCDIRS := ../../Ai-M6x_HomeAssistant-C/HomeAssistant-C
COMPONENT_SRCDIRS += at_command device_state flash homeAssistant  ir_hxd019 wifi

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))