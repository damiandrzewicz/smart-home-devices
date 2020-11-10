#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)
CXXFLAGS += -std=c++14
CXXFLAGS += -std=c++17
CXXFLAGS += -fconcepts


# OTA certs
COMPONENT_EMBED_TXTFILES :=  ${PROJECT_PATH}/ota_certs/ca_cert_ota.pem