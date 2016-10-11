PLUGIN_NAME = signal_generator

RTXI_INCLUDES =

HEADERS = signal-generator.h \

SOURCES = signal-generator.cpp \
          moc_signal-generator.cpp

LIBS = -lgsl -lgslcblas -lrtgen

### Do not edit below this line ###

include $(shell rtxi_plugin_config --pkgdata-dir)/Makefile.plugin_compile
