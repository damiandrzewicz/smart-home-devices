#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := smart-home-device-template

EXTRA_COMPONENT_DIRS += ./../smart-home-device-libs/network_wrapper
EXTRA_COMPONENT_DIRS += ./../smart-home-device-libs/ota_wrapper
EXTRA_COMPONENT_DIRS += ./../smart-home-device-libs/mqtt_wrapper
EXTRA_COMPONENT_DIRS += ./../smart-home-device-libs/utils
EXTRA_COMPONENT_DIRS += ./../smart-home-device-libs/smart-device
$(info "EXTRA_COMPONENT_DIRS=" $(EXTRA_COMPONENTS_DIRS))

PROJECT_VER := $(shell git describe --always --tags --dirty)

include $(IDF_PATH)/make/project.mk



