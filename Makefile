PLUGIN_NAME = signal_generator

HEADERS = signal-generator.h \
          /usr/local/lib/rtxi_includes/generator.h\
          /usr/local/lib/rtxi_includes/gen_biphase.h\
			 /usr/local/lib/rtxi_includes/gen_mono.h\
			 /usr/local/lib/rtxi_includes/gen_saw.h\
			 /usr/local/lib/rtxi_includes/gen_sine.h\
			 /usr/local/lib/rtxi_includes/gen_whitenoise.h\
			 /usr/local/lib/rtxi_includes/gen_zap.h

SOURCES = signal-generator.cpp \
          moc_signal-generator.cpp\
          /usr/local/lib/rtxi_includes/generator.cpp\
          /usr/local/lib/rtxi_includes/gen_biphase.cpp\
			 /usr/local/lib/rtxi_includes/gen_mono.cpp\
			 /usr/local/lib/rtxi_includes/gen_saw.cpp\
			 /usr/local/lib/rtxi_includes/gen_sine.cpp\
			 /usr/local/lib/rtxi_includes/gen_whitenoise.cpp\
			 /usr/local/lib/rtxi_includes/gen_zap.cpp

LIBS = -lgsl -lgslcblas

### Do not edit below this line ###

include $(shell rtxi_plugin_config --pkgdata-dir)/Makefile.plugin_compile
