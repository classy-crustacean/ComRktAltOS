# location of code signing key, et al
 
JAVAROOT=classes
AM_JAVACFLAGS=$(JAVAC_VERSION_FLAGS) -encoding UTF-8 -Xlint:deprecation -Xlint:unchecked

man_MANS=telegps.1

altoslibdir=$(libdir)/altos

CLASSPATH_ENV=mkdir -p $(JAVAROOT); CLASSPATH=".:classes:../altoslib/*:../altosuilib/*:../libaltos:$(JCOMMON)/jcommon.jar:$(JFREECHART)/jfreechart.jar:$(FREETTS)/freetts.jar"

bin_SCRIPTS=telegps

telegpsdir=$(datadir)/java

telegps_JAVA= \
	TeleGPS.java \
	TeleGPSStatus.java \
	TeleGPSStatusUpdate.java \
	TeleGPSInfo.java \
	TeleGPSState.java \
	TeleGPSConfig.java \
	TeleGPSConfigUI.java \
	TeleGPSPreferences.java \
	TeleGPSGraphUI.java \
	TeleGPSDisplayThread.java

JFREECHART_CLASS= \
    jfreechart.jar

JCOMMON_CLASS=\
    jcommon.jar

FREETTS_CLASS= \
	cmudict04.jar \
	cmulex.jar \
	cmu_time_awb.jar \
	cmutimelex.jar \
	cmu_us_kal.jar \
	en_us.jar \
	freetts.jar

JAR=telegps.jar

FATJAR=telegps-fat.jar

if MULTI_ARCH
LIBALTOS_LINUX=libaltos_i686.so libaltos_amd64.so libaltos_aarch64.so libaltos_armel.so libaltos_armhf.so
else
LIBALTOS_LINUX=libaltos.so
endif

LIBALTOS= \
	$(LIBALTOS_LINUX) \
	libaltos.dylib \
	altos64.dll \
	altos.dll

ALTOSLIB_CLASS=\
	altoslib_$(ALTOSLIB_VERSION).jar

ALTOSUILIB_CLASS=\
	altosuilib_$(ALTOSUILIB_VERSION).jar

# Icons
ICONDIR=$(top_srcdir)/icon

JAVA_ICONS=\
	$(ICONDIR)/altusmetrum-telegps-16.png \
	$(ICONDIR)/altusmetrum-telegps-32.png \
	$(ICONDIR)/altusmetrum-telegps-48.png \
	$(ICONDIR)/altusmetrum-telegps-64.png \
	$(ICONDIR)/altusmetrum-telegps-128.png\
	$(ICONDIR)/altusmetrum-telegps-256.png

# icon base names for jar
ICONJAR= \
	-C $(ICONDIR) altusmetrum-telegps-16.png \
	-C $(ICONDIR) altusmetrum-telegps-32.png \
	-C $(ICONDIR) altusmetrum-telegps-48.png \
	-C $(ICONDIR) altusmetrum-telegps-64.png \
	-C $(ICONDIR) altusmetrum-telegps-128.png\
	-C $(ICONDIR) altusmetrum-telegps-256.png

WINDOWS_ICONS	=\
	../icon/altusmetrum-telegps.ico \
	../icon/altusmetrum-telegps.exe
	../icon/application-vnd.altusmetrum.eeprom.ico \
	../icon/application-vnd.altusmetrum.eeprom.exe \
	../icon/application-vnd.altusmetrum.telemetry.ico \
	../icon/application-vnd.altusmetrum.telemetry.exe

MACOSX_ICONS	=\
	../icon/altusmetrum-telegps.icns \
	../icon/application-vnd.altusmetrum.eeprom.icns \
	../icon/application-vnd.altusmetrum.telemetry.icns

LINUX_ICONS	=\
	$(ICONDIR)/altusmetrum-altosui.svg \
	$(ICONDIR)/application-vnd.altusmetrum.eeprom.svg \
	$(ICONDIR)/application-vnd.altusmetrum.telemetry.svg

LINUX_MIMETYPE	=\
	$(ICONDIR)/org-altusmetrum-mimetypes.xml

# Firmware
FIRMWARE_TD_3_0=$(top_srcdir)/src/teledongle-v3.0/teledongle-v3.0-$(VERSION).ihx
FIRMWARE_TD=$(FIRMWARE_TD_3_0)

FIRMWARE_TBT_3_0=$(top_srcdir)/src/telebt-v3.0/telebt-v3.0-$(VERSION).ihx
FIRMWARE_TBT_4_0=$(top_srcdir)/src/telebt-v4.0/telebt-v4.0-$(VERSION).ihx
FIRMWARE_TBT=$(FIRMWARE_TBT_3_0) $(FIRMWARE_TBT_4_0)

FIRMWARE_TGPS_1_0=$(top_srcdir)/src/telegps-v1.0/telegps-v1.0-$(VERSION).ihx
FIRMWARE_TGPS_2_0=$(top_srcdir)/src/telegps-v2.0/telegps-v2.0-$(VERSION).ihx
FIRMWARE_TGPS_3_0=$(top_srcdir)/src/telegps-v3.0/telegps-v3.0-$(VERSION).ihx
FIRMWARE_TGPS=$(FIRMWARE_TGPS_1_0) $(FIRMWARE_TGPS_2_0) $(FIRMWARE_TGPS_3_0)

FIRMWARE=$(FIRMWARE_TGPS) $(FIRMWARE_TD) $(FIRMWARE_TBT)

desktopdir = $(datadir)/applications
desktop_file = altusmetrum-telegps.desktop
desktop_SCRIPTS = $(desktop_file)

all-local: telegps-test telegps-jdb $(JAR) $(MDWN)

clean-local:
	-rm -rf classes $(JAR) $(FATJAR) \
		TeleGPS-Linux-*.tar.bz2 TeleGPS-Mac-*.dmg TeleGPS-Windows-*.exe \
		altoslib_*.jar altosuilib_*.jar \
		$(JFREECHART_CLASS) $(JCOMMON_CLASS) $(FREETTS_CLASS) $(LIBALTOS) Manifest.txt Manifest-fat.txt \
		telegps telegps-test telegps-jdb macosx linux windows telegps-windows.log \
		telegps-windows.nsi *.desktop *.mdwn

EXTRA_DIST = $(desktop_file).in

$(desktop_file): $(desktop_file).in
	sed -e 's#%bindir%#@bindir@#' -e 's#%icondir%#$(datadir)/icons/hicolor/scalable/apps#' ${srcdir}/$(desktop_file).in > $@
	chmod +x $@

LINUX_DIST=TeleGPS-Linux-$(VERSION).tar.bz2
LINUX_SH=TeleGPS-Linux-$(VERSION).sh
MACOSX_DIST=TeleGPS-Mac-$(VERSION).dmg
WINDOWS_DIST=TeleGPS-Windows-$(VERSION_DASH).exe
MDWN=$(VERSION).mdwn
MDWNTMPL=mdwn.tmpl

TELEGPS_DOC=$(top_srcdir)/doc/telegps.pdf

DOC=$(TELEGPS_DOC)

FAT_FILES=$(FATJAR) $(ALTOSLIB_CLASS) $(ALTOSUILIB_CLASS) $(FREETTS_CLASS) $(JFREECHART_CLASS) $(JCOMMON_CLASS)

LINUX_FILES=$(FAT_FILES) $(LIBALTOS_LINUX) $(FIRMWARE) $(DOC) $(desktop_file).in $(LINUX_ICONS) $(LINUX_MIMETYPE)
LINUX_EXTRA=telegps-fat $(desktop_file).in

MACOSX_INFO_PLIST=Info.plist
MACOSX_README=ReadMe-Mac.rtf
MACOSX_INSTALL=../altosui/install-macosx ../altosui/ask-pass
MACOSX_FILES=$(FAT_FILES) libaltos.dylib $(MACOSX_INFO_PLIST) $(MACOSX_README) $(DOC) $(MACOSX_ICONS) $(MACOSX_INSTALL)
MACOSX_EXTRA=$(FIRMWARE)

WINDOWS_FILES=$(FAT_FILES) altos.dll altos64.dll $(top_srcdir)/altusmetrum.inf $(top_srcdir)/altusmetrum.cat $(DOC) $(WINDOWS_ICONS)

if FATINSTALL

FATTARGET=$(FATDIR)/TeleGPS/releases/$(VERSION)

LINUX_SH_TARGET=$(FATTARGET)/$(LINUX_SH)
MACOSX_DIST_TARGET=$(FATTARGET)/$(MACOSX_DIST)
WINDOWS_DIST_TARGET=$(FATTARGET)/$(WINDOWS_DIST)
MDWN_TARGET=$(FATDIR)/TeleGPS/releases/$(MDWN)
RELNOTES=release-notes-$(VERSION).html
RELNOTES_SRC=$(top_builddir)/doc/$(RELNOTES)
RELNOTES_TARGET=$(FATTARGET)/$(RELNOTES)

fat-install: $(LINUX_SH_TARGET) $(MACOSX_DIST_TARGET) $(WINDOWS_DIST_TARGET) $(MDWN_TARGET) $(RELNOTES_TARGET)

$(LINUX_SH_TARGET): $(LINUX_SH)
	mkdir -p $(FATTARGET)
	cp -p $< $@

$(MACOSX_DIST_TARGET): $(MACOSX_DIST)
	mkdir -p $(FATTARGET)
	cp -p $< $@

$(WINDOWS_DIST_TARGET): $(WINDOWS_DIST)
	mkdir -p $(FATTARGET)
	cp -p $< $@

$(MDWN_TARGET): $(MDWN)
	mkdir -p $(FATTARGET)
	cp -p $< $@

$(RELNOTES_TARGET): $(RELNOTES_SRC)
	mkdir -p $(FATTARGET)
	sh $(top_srcdir)/doc/install-html -d $(FATTARGET) $(RELNOTES_SRC)

endif

$(MDWN): $(MDWNTMPL)
	sed -e 's/%version%/$(VERSION)/g' -e 's/%version_dash%/$(VERSION_DASH)/g' $(MDWNTMPL) > $@

fat: $(LINUX_DIST) $(LINUX_SH) $(MACOSX_DIST) $(WINDOWS_DIST)


telegps: Makefile
	echo "#!/bin/sh" > $@
	echo 'exec java  -Djava.library.path="$(altoslibdir)" -jar "$(telegpsdir)/telegps.jar" "$$@"' >> $@
	chmod +x $@

telegps-jdb: Makefile
	echo "#!/bin/sh" > $@
	echo 'exec jdb -classpath "classes:./*:../libaltos:$(JCOMMON)/jcommon.jar:$(JFREECHART)/jfreechart.jar" -Djava.library.path="../libaltos/.libs" org.altusmetrum.telegps.TeleGPS "$$@"' >> $@
	chmod +x $@

telegps-test: Makefile
	echo "#!/bin/sh" > $@
	echo 'exec java -Djava.library.path="../libaltos/.libs" -jar telegps.jar "$$@"' >> $@
	chmod +x $@

install-telegpsJAVA: telegps.jar
	@$(NORMAL_INSTALL)
	test -z "$(telegpsdir)" || $(MKDIR_P) "$(DESTDIR)$(telegpsdir)"
	echo " $(INSTALL_DATA)" "$<" "'$(DESTDIR)$(telegpsdir)/telegps.jar'"; \
	$(INSTALL_DATA) "$<" "$(DESTDIR)$(telegpsdir)"

$(JAR): classtelegps.stamp Manifest.txt $(JAVA_ICONS) $(ALTOSLIB_CLASS) $(ALTOSUILIB_CLASS)
	jar cfm $@ Manifest.txt \
		$(ICONJAR) \
		-C classes org \
		-C ../libaltos libaltosJNI
if STRIP_NONDETERMINISM
	$(STRIP_NONDETERMINISM) $@
endif

$(FATJAR): classtelegps.stamp Manifest-fat.txt $(ALTOSLIB_CLASS) $(ALTOSUILIB_CLASS) $(JFREECHART_CLASS) $(JCOMMON_CLASS) $(JAVA_ICONS)
	jar cfm $@ Manifest-fat.txt \
		$(ICONJAR) \
		-C classes org \
		-C ../libaltos libaltosJNI
if STRIP_NONDETERMINISM
	$(STRIP_NONDETERMINISM) $@
endif

libaltos.so: build-libaltos
	-rm -f "$@"
	$(LN_S) ../libaltos/.libs/"$@" .

libaltos_i686.so: build-libaltos
	-rm -f "$@"
	$(LN_S) ../libaltos/.libs/"$@" .

libaltos_amd64.so: build-libaltos
	-rm -f "$@"
	$(LN_S) ../libaltos/.libs/"$@" .

libaltos_aarch64.so: build-libaltos
	-rm -f "$@"
	$(LN_S) ../libaltos/.libs/"$@" .

libaltos_armel.so: build-libaltos
	-rm -f "$@"
	$(LN_S) ../libaltos/.libs/"$@" .

libaltos_armhf.so: build-libaltos
	-rm -f "$@"
	$(LN_S) ../libaltos/.libs/"$@" .

libaltos.dylib:
	-rm -f "$@"
	$(LN_S) ../libaltos/"$@" .

altos.dll: build-libaltos
	-rm -f "$@"
	$(LN_S) ../libaltos/"$@" .

altos64.dll: build-libaltos
	-rm -f "$@"
	$(LN_S) ../libaltos/"$@" .

build-libaltos:
	+cd ../libaltos && make

$(ALTOSLIB_CLASS):
	-rm -f "$@"
	$(LN_S) ../altoslib/"$@" .

$(ALTOSUILIB_CLASS):
	-rm -f "$@"
	$(LN_S) ../altosuilib/"$@" .

$(FREETTS_CLASS):
	-rm -f "$@"
	$(LN_S) "$(FREETTS)"/"$@" .

$(JFREECHART_CLASS):
	-rm -f "$@"
	$(LN_S) "$(JFREECHART)"/"$@" .

$(JCOMMON_CLASS):
	-rm -f "$@"
	$(LN_S) "$(JCOMMON)"/"$@" .

$(LINUX_DIST): $(LINUX_FILES) $(LINUX_EXTRA)
	-rm -f $@
	-rm -rf linux
	mkdir -p linux/TeleGPS
	cp -p $(LINUX_FILES) linux/TeleGPS
	cp -p telegps-fat linux/TeleGPS/telegps
	chmod +x linux/TeleGPS/telegps
	tar cjf $@ -C linux TeleGPS

$(LINUX_SH): $(LINUX_DIST) $(srcdir)/../altosui/linux-install.sh
	sed 's/AltOS/TeleGPS/g' $(srcdir)/../altosui/linux-install.sh | cat - $(LINUX_DIST) > $@
	chmod +x $@

$(MACOSX_DIST): $(MACOSX_FILES) $(MACOSX_EXTRA) Makefile
	-rm -f $@
	-rm -rf macosx
	mkdir macosx
	cp -a TeleGPS.app macosx/
	cp -a $(MACOSX_README) macosx/ReadMe-TeleGPS.rtf
	cp -a $(MACOSX_INSTALL) macosx
	mkdir -p macosx/Doc
	cp -a $(DOC) macosx/Doc
	cp -p Info.plist macosx/TeleGPS.app/Contents
	mkdir -p macosx/AltOS-$(VERSION) macosx/TeleGPS.app/Contents/Resources/Java
	cp -p $(MACOSX_ICONS) macosx/TeleGPS.app/Contents/Resources
	cp -p $(FATJAR) macosx/TeleGPS.app/Contents/Resources/Java/telegps.jar
	cp -p libaltos.dylib macosx/TeleGPS.app/Contents/Resources/Java
	cp -p $(ALTOSLIB_CLASS) macosx/TeleGPS.app/Contents/Resources/Java
	cp -p $(ALTOSUILIB_CLASS) macosx/TeleGPS.app/Contents/Resources/Java
	cp -p $(FREETTS_CLASS) macosx/TeleGPS.app/Contents/Resources/Java
	cp -p $(JFREECHART_CLASS) macosx/TeleGPS.app/Contents/Resources/Java
	cp -p $(JCOMMON_CLASS) macosx/TeleGPS.app/Contents/Resources/Java
	cp -p $(MACOSX_EXTRA) macosx/AltOS-$(VERSION)
	genisoimage -D -V TeleGPS-$(VERSION) -no-pad -r -apple -o $@ macosx

$(WINDOWS_DIST): $(WINDOWS_FILES) telegps-windows.nsi
	-rm -f $@
	makensis -Otelegps-windows.log "-XOutFile $@" "-DVERSION=$(VERSION)" telegps-windows.nsi || (cat telegps-windows.log && exit 1)
	if [ "$(HAVE_WINDOWS_KEY)" = "yes" ]; then \
	jsign --keystore "$(WINDOWSKEYFILE)" --alias 1 \
		--storetype PKCS12 --storepass `cat "$(WINDOWSKEYPASSFILE)"` \
		--tsaurl http://ts.ssl.com --tsmode RFC3161 $@ ; \
	fi

Manifest.txt: Makefile
	echo 'Main-Class: org.altusmetrum.telegps.TeleGPS' > $@
	echo "Class-Path: $(ALTOSLIB_CLASS) $(ALTOSUILIB_CLASS) $(FREETTS)/freetts.jar $(JCOMMON)/jcommon.jar $(JFREECHART)/jfreechart.jar" >> $@

Manifest-fat.txt:
	echo 'Main-Class: org.altusmetrum.telegps.TeleGPS' > $@
	echo "Class-Path: $(ALTOSLIB_CLASS) $(ALTOSUILIB_CLASS) freetts.jar jcommon.jar jfreechart.jar" >> $@

