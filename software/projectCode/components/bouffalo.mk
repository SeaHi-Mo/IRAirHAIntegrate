
COMPONENT_ADD_INCLUDEDIRS := ../../HomeAssistant-C
COMPONENT_ADD_INCLUDEDIRS += at_command device_state flash homeAssistant  ir_device wifi button led sht30

COMPONENT_SRCDIRS := ../../HomeAssistant-C
COMPONENT_SRCDIRS += at_command device_state flash homeAssistant  ir_device wifi button led sht30

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))