PLUGIN_NAME = signal_generator

RTXI_INCLUDES = /usr/local/lib/rtxi_includes

HEADERS = signal-generator.h \
          $(RTXI_INCLUDES)/generator.h\
          $(RTXI_INCLUDES)/gen_biphase.h\
			 $(RTXI_INCLUDES)/gen_mono.h\
			 $(RTXI_INCLUDES)/gen_saw.h\
			 $(RTXI_INCLUDES)/gen_sine.h\
			 $(RTXI_INCLUDES)/gen_zap.h

SOURCES = signal-generator.cpp \
          moc_signal-generator.cpp\
          $(RTXI_INCLUDES)/generator.cpp\
          $(RTXI_INCLUDES)/gen_biphase.cpp\
			 $(RTXI_INCLUDES)/gen_mono.cpp\
			 $(RTXI_INCLUDES)/gen_saw.cpp\
			 $(RTXI_INCLUDES)/gen_sine.cpp\
			 $(RTXI_INCLUDES)/gen_zap.cpp

LIBS = -lgsl -lgslcblas

### Do not edit below this line ###

include $(shell rtxi_plugin_config --pkgdata-dir)/Makefile.plugin_compile
