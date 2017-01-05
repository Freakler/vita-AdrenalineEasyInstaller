TITLE_ID = ADRINSTAL
TARGET   = EasyInstaller
OBJS     = main.o font.o graphics.o init.o file.o

PSVITAIP = 192.168.0.109

RESOURCES_PNG = materials/button_pad.png materials/button_cross.png materials/button_circle.png
OBJS += $(RESOURCES_PNG:.png=.o)

LIBS = -lvita2d -lSceKernel_stub -lScePower_stub -lSceDisplay_stub -lSceGxm_stub -lSceAppUtil_stub -lSceAppMgr_stub \
	-lSceSysmodule_stub -lSceCtrl_stub -lScePgf_stub -lSceHttp_stub -lSceNet_stub -lSceNetCtl_stub\
	-lSceCommonDialog_stub -lfreetype -lpng -ljpeg -lz -lm -lc

PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CFLAGS  = -Wl,-q -Wall -O3
ASFLAGS = $(CFLAGS)

all: $(TARGET).vpk

%.vpk: eboot.bin
	vita-mksfoex -s TITLE_ID=$(TITLE_ID) "$(TARGET)" param.sfo
	vita-pack-vpk -s param.sfo -b eboot.bin \
      -a livearea/icon0.png=sce_sys/icon0.png \
      -a livearea/template.xml=sce_sys/livearea/contents/template.xml \
      -a livearea/startup.png=sce_sys/livearea/contents/startup.png \
      -a livearea/bg0.png=sce_sys/livearea/contents/bg0.png \
      -a files/adrenaline.skprx=files/adrenaline.skprx \
      -a files/adrenaline.suprx=files/adrenaline.suprx \
      -a files/icon0.dds=files/icon0.dds \
      -a files/bg0.png=files/bg0.png \
      -a files/startup.png=files/startup.png \
      -a files/template.xml=files/template.xml \
      -a files/PBOOT.PBP=files/PBOOT.PBP \
      $@
	  
eboot.bin: $(TARGET).velf
	vita-make-fself $< $@
	#vita-make-fself -s $< $@

%.velf: %.elf
	vita-elf-create $< $@

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

%.o: %.png
	$(PREFIX)-ld -r -b binary -o $@ $^

clean:
	@rm -rf $(TARGET).vpk $(TARGET).velf $(TARGET).elf $(OBJS) \
		eboot.bin param.sfo

vpksend: $(TARGET).vpk
	curl -T $(TARGET).vpk ftp://$(PSVITAIP):1337/ux0:/
	@echo "Sent."

send: eboot.bin
	curl -T eboot.bin ftp://$(PSVITAIP):1337/ux0:/app/$(TITLE_ID)/
	@echo "Sent."
