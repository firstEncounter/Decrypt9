#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/ds_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
# SPECS is the directory containing the important build and link files
#---------------------------------------------------------------------------------
export TARGET	:= $(shell basename $(CURDIR))
BUILD		:=	build
SOURCES		:=	source source/fatfs source/decryptor source/abstraction
DATA		:=	data
INCLUDES	:=	include source source/fatfs

#---------------------------------------------------------------------------------
# Include AppInfo / define Loader
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
TOPDIR ?= $(CURDIR)
else
TOPDIR ?= $(CURDIR)/..
endif

include $(TOPDIR)/resources/AppInfo
LOADER			:= brahma_loader

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-mthumb -mthumb-interwork

CFLAGS	:=	-g -Wall -O2\
			-march=armv5te -mtune=arm946e-s -fomit-frame-pointer\
			-ffast-math -std=c99\
			$(ARCH)

CFLAGS	+=	$(INCLUDE) -DEXEC_$(EXEC_METHOD) -DARM9 -DAPP_TITLE=\"$(subst $(SPACE),,$(APP_TITLE))\"

CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions

ASFLAGS	:=	-g $(ARCH) -DEXEC_$(EXEC_METHOD)
LDFLAGS	=	-nostartfiles -g $(ARCH) -Wl,-Map,$(notdir $*.map)

ifeq ($(EXEC_METHOD),GATEWAY)
	LDFLAGS += --specs=../gateway.specs
else ifeq ($(EXEC_METHOD),BOOTSTRAP)
	LDFLAGS += --specs=../bootstrap.specs
endif

LIBS	:=

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT_D	:=	$(CURDIR)/output
export OUTPUT_N	:=	$(subst $(SPACE),,$(APP_TITLE))
export OUTPUT	:=	$(OUTPUT_D)/$(OUTPUT_N)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES	:= $(addsuffix .o,$(BINFILES)) \
			$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: common clean all gateway bootstrap brahma cakehax release

#---------------------------------------------------------------------------------
all: cakehax

common:
	@[ -d $(OUTPUT_D) ] || mkdir -p $(OUTPUT_D)
	@[ -d $(BUILD) ] || mkdir -p $(BUILD)

gateway: common
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile EXEC_METHOD=GATEWAY
	cp resources/LauncherTemplate.dat $(OUTPUT_D)/Launcher.dat
	dd if=$(OUTPUT).bin of=$(OUTPUT_D)/Launcher.dat bs=1497296 seek=1 conv=notrunc

bootstrap: common
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile EXEC_METHOD=BOOTSTRAP
	
brahma: bootstrap
	@[ -d $(TOPDIR)/$(LOADER)/data ] || mkdir -p $(TOPDIR)/$(LOADER)/data
	@cp $(OUTPUT).bin $(TOPDIR)/$(LOADER)/data/payload.bin
	@make --no-print-directory -C $(TOPDIR)/$(LOADER) -f $(TOPDIR)/$(LOADER)/Makefile
	@cp $(TOPDIR)/$(LOADER)/output/$(OUTPUT_N).3dsx $(OUTPUT).3dsx
	@cp $(TOPDIR)/$(LOADER)/output/$(OUTPUT_N).smdh $(OUTPUT).smdh

cakehax: common
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile EXEC_METHOD=GATEWAY
	@make dir_out=$(OUTPUT_D) name=$(TARGET).dat -C CakeHax bigpayload
	dd if=$(OUTPUT).bin of=$(OUTPUT).dat bs=512 seek=160
	
release:
	@rm -fr $(BUILD) $(OUTPUT_D) $(TOPDIR)/release
	@make --no-print-directory gateway
	@-make --no-print-directory cakehax
	@rm -fr $(BUILD) $(OUTPUT).bin $(OUTPUT).elf $(TOPDIR)/$(LOADER)/data
	@make --no-print-directory brahma
	@[ -d $(TOPDIR)/release ] || mkdir -p $(TOPDIR)/release
	@[ -d $(TOPDIR)/release/$(OUTPUT_N) ] || mkdir -p $(TOPDIR)/release/$(OUTPUT_N)
	@[ -d $(TOPDIR)/release/$(OUTPUT_N)/UI ] || mkdir -p $(TOPDIR)/release/$(OUTPUT_N)/UI
	@[ -d $(TOPDIR)/release/scripts ] || mkdir -p $(TOPDIR)/release/scripts
	@cp $(OUTPUT_D)/Launcher.dat $(TOPDIR)/release
	@-cp $(OUTPUT).dat $(TOPDIR)/release
	@cp $(OUTPUT).bin $(TOPDIR)/release
	@cp $(OUTPUT).3dsx $(TOPDIR)/release/$(OUTPUT_N)
	@cp $(OUTPUT).smdh $(TOPDIR)/release/$(OUTPUT_N)
	@cp $(TOPDIR)/scripts/*.py $(TOPDIR)/release/scripts
	@cp $(TOPDIR)/UI/*.bin $(TOPDIR)/release/$(OUTPUT_N)/UI
	
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@make clean --no-print-directory -C $(TOPDIR)/$(LOADER) -f $(TOPDIR)/$(LOADER)/Makefile
	@-make clean --no-print-directory -C $(TOPDIR)/CakeHax -f $(TOPDIR)/CakeHax/Makefile
	@rm -fr $(BUILD) $(OUTPUT_D) $(TOPDIR)/$(LOADER)/data


#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).bin	:	$(OUTPUT).elf
$(OUTPUT).elf	:	$(OFILES)


#---------------------------------------------------------------------------------
%.bin: %.elf
	@$(OBJCOPY) --set-section-flags .bss=alloc,load,contents -O binary $< $@
	@echo built ... $(notdir $@)


-include $(DEPENDS)


#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
