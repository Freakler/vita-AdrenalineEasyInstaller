TITLE_ID = ADRINSTAL
TARGET   = EasyInstaller
NAME     = Adrenaline EasyInstaller
OBJS     = main.o font.o graphics.o init.o file.o vita_sqlite.o sqlite-3.6.23.1/sqlite3.o

PSVITAIP = 192.168.0.114

RESOURCES_PNG = materials/button_pad.png materials/button_cross.png materials/button_circle.png
OBJS += $(RESOURCES_PNG:.png=.o)

LIBS = -lvita2d -lScePower_stub -lSceLibKernel_stub -lSceDisplay_stub -lSceGxm_stub -lSceAppUtil_stub -lSceAppMgr_stub \
	-lSceSysmodule_stub -lSceCtrl_stub -lScePgf_stub -lSceHttp_stub -lSceNet_stub -lSceNetCtl_stub\
	-lSceCommonDialog_stub -lSceVshBridge_stub -lSceShellSvc_stub -lfreetype -lpng -ljpeg -lz -lm -lc -ltaihen_stub -lScePromoterUtil_stub 

DEFINES = -DSQLITE_OS_OTHER=1 -DSQLITE_TEMP_STORE=3 -DSQLITE_THREADSAFE=0
	
PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CFLAGS  = -Wl,-q -Wall -O3 $(DEFINES)
ASFLAGS = $(CFLAGS)

all: $(TARGET).vpk

%.vpk: eboot.bin
	#vita-mksfoex -s TITLE_ID=$(TITLE_ID) "$(TARGET)" param.sfo
	vita-mksfoex -s TITLE_ID=$(TITLE_ID) "$(NAME)" param.sfo
	vita-pack-vpk -s param.sfo -b eboot.bin \
      -a livearea/icon0.png=sce_sys/icon0.png \
      -a livearea/template.xml=sce_sys/livearea/contents/template.xml \
      -a livearea/startup.png=sce_sys/livearea/contents/startup.png \
      -a livearea/bg0.png=sce_sys/livearea/contents/bg0.png \
      -a livearea/changeinfo.xml=sce_sys/changeinfo/changeinfo.xml \
      -a files/releases/adrenaline/adrenaline.skprx=files/releases/adrenaline/adrenaline.skprx \
      -a files/releases/adrenaline/adrenaline.suprx=files/releases/adrenaline/adrenaline.suprx \
      -a files/releases/adrenaline_fix2/adrenaline.skprx=files/releases/adrenaline_fix2/adrenaline.skprx \
      -a files/releases/adrenaline_fix2/adrenaline.suprx=files/releases/adrenaline_fix2/adrenaline.suprx \
      -a files/releases/adrenaline_v2/adrenaline.skprx=files/releases/adrenaline_v2/adrenaline.skprx \
      -a files/releases/adrenaline_v2/adrenaline.suprx=files/releases/adrenaline_v2/adrenaline.suprx \
      -a files/releases/adrenaline_v3/adrenaline.skprx=files/releases/adrenaline_v3/adrenaline.skprx \
      -a files/releases/adrenaline_v3/adrenaline.suprx=files/releases/adrenaline_v3/adrenaline.suprx \
      -a files/releases/adrenaline_v3/usbdevice.skprx=files/releases/adrenaline_v3/usbdevice.skprx \
      -a files/releases/adrenaline_v3_fix/adrenaline.skprx=files/releases/adrenaline_v3_fix/adrenaline.skprx \
      -a files/releases/adrenaline_v3_fix/adrenaline.suprx=files/releases/adrenaline_v3_fix/adrenaline.suprx \
      -a files/releases/adrenaline_v3_fix/usbdevice.skprx=files/releases/adrenaline_v3_fix/usbdevice.skprx \
      -a files/releases/adrenaline_v3.1/adrenaline.skprx=files/releases/adrenaline_v3.1/adrenaline.skprx \
      -a files/releases/adrenaline_v3.1/adrenaline.suprx=files/releases/adrenaline_v3.1/adrenaline.suprx \
      -a files/releases/adrenaline_v3.1/usbdevice.skprx=files/releases/adrenaline_v3.1/usbdevice.skprx \
      -a files/releases/adrenaline_v4/adrenaline.skprx=files/releases/adrenaline_v4/adrenaline.skprx \
      -a files/releases/adrenaline_v4/adrenaline.suprx=files/releases/adrenaline_v4/adrenaline.suprx \
      -a files/releases/adrenaline_v4/usbdevice.skprx=files/releases/adrenaline_v4/usbdevice.skprx \
      -a files/releases/adrenaline_v4.1/adrenaline.skprx=files/releases/adrenaline_v4.1/adrenaline.skprx \
      -a files/releases/adrenaline_v4.1/adrenaline.suprx=files/releases/adrenaline_v4.1/adrenaline.suprx \
      -a files/releases/adrenaline_v4.1/usbdevice.skprx=files/releases/adrenaline_v4.1/usbdevice.skprx \
      -a files/releases/adrenaline_v4.2/adrenaline.skprx=files/releases/adrenaline_v4.2/adrenaline.skprx \
      -a files/releases/adrenaline_v4.2/adrenaline.suprx=files/releases/adrenaline_v4.2/adrenaline.suprx \
      -a files/releases/adrenaline_v4.2/usbdevice.skprx=files/releases/adrenaline_v4.2/usbdevice.skprx \
      -a files/updatehistory.txt=files/updatehistory.txt \
      -a files/icon0.dds=files/icon0.dds \
      -a files/bg0.png=files/bg0.png \
      -a files/startup.png=files/startup.png \
      -a files/PBOOT.PBP=files/PBOOT.PBP \
      -a files/basegame/d0.pdb=files/basegame/bgdl/d0.pdb \
      -a files/basegame/d1.pdb=files/basegame/bgdl/d1.pdb \
      -a files/basegame/f0.pdb=files/basegame/bgdl/f0.pdb \
      -a files/basegame/icon.png=files/basegame/bgdl/icon.png \
      -a files/basegame/body.bin=files/basegame/pspemu/body.bin \
      -a files/basegame/head.bin=files/basegame/pspemu/head.bin \
      -a files/basegame/stat.bin=files/basegame/pspemu/stat.bin \
      -a files/basegame/tail.bin=files/basegame/pspemu/tail.bin \
      -a files/basegame/work.bin=files/basegame/pspemu/work.bin \
      -a files/basegame/EBOOT.PBP=files/basegame/pspemu/EBOOT.PBP \
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
