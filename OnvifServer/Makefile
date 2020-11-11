################OPTION###################

# PREFIX = arm-himix200-linux-
PREFIX = 

CCOMPILE = $(PREFIX)gcc
CPPCOMPILE =  $(PREFIX)g++
COMPILEOPTION = -g -c -O3
AR = $(PREFIX)ar

COMPILEOPTION += -DEPOLL
# COMPILEOPTION += -DHTTPS
COMPILEOPTION += -DHTTPD
COMPILEOPTION += -DDEVICEIO_SUPPORT
#COMPILEOPTION += -DPROFILE_G_SUPPORT
#COMPILEOPTION += -DPROFILE_C_SUPPORT
COMPILEOPTION += -DCREDENTIAL_SUPPORT     
#COMPILEOPTION += -DACCESS_RULES
#COMPILEOPTION += -DSCHEDULE_SUPPORT    
COMPILEOPTION += -DAUDIO_SUPPORT
COMPILEOPTION += -DMEDIA2_SUPPORT
COMPILEOPTION += -DPTZ_SUPPORT
#COMPILEOPTION += -DVIDEO_ANALYTICS
#COMPILEOPTION += -DTHERMAL_SUPPORT
#COMPILEOPTION += -DRECEIVER_SUPPORT
#COMPILEOPTION += -DIPFILTER_SUPPORT
#COMPILEOPTION += -DPROFILE_Q_SUPPORT
#COMPILEOPTION += -DLIBICAL

INCLUDEDIR = -I./bm -I./onvif -I./http
INCLUDEDIR += -I./openssl/include -I./openssl/include/linux
INCLUDEDIR += -I./libical/include
INCLUDEDIR += -I../camCtl
INCLUDEDIR += -I../camCtl/pelco_ptz 
INCLUDEDIR += -I../camCtl/visca

LINK = $(PREFIX)g++
LINKOPTION = -g -o onvifserver
LIBDIRS = -L./openssl/lib/linux -L./libical/lib/linux
OBJS = bm/word_analyse.o bm/util.o bm/sys_os.o bm/sys_log.o bm/sys_buf.o bm/ppstack.o bm/base64.o \
       bm/sha1.o bm/linked_list.o bm/hqueue.o bm/rfc_md5.o bm/xml_node.o bm/hxml.o http/http_srv.o \
       http/http_auth.o http/http_parse.o http/http_cln.o http/httpd.o onvif/soap.o onvif/onvif_probe.o \
       onvif/onvif_pkt.o onvif/onvif.o onvif/soap_parser.o onvif/onvif_doorcontrol.o onvif/onvif_device.o \
       onvif/onvif_timer.o onvif/onvif_event.o onvif/onvif_api.o onvif/onvif_ptz.o onvif/onvif_utils.o \
       onvif/onvif_media.o onvif/onvif_image.o onvif/onvif_cm.o onvif/onvif_recording.o onvif/onvif_analytics.o \
       onvif/onvif_cfg.o onvif/onvif_deviceio.o onvif/onvif_media2.o onvif/onvif_thermal.o \
       onvif/onvif_credential.o onvif/onvif_accessrules.o onvif/onvif_schedule.o onvif/onvif_receiver.o \
	   onvif_main.o	\
	  ../camCtl/pelco_ptz/ptz.o \
	   ../camCtl/visca/libvisca.o ../camCtl/visca/libvisca_posix.o ../camCtl/visca/rw_config.o ../camCtl/visca/visca_api.o \
	   ../camCtl/set_config.o \
	   main.o

OUTPUT = onvifserver
SHAREDLIB = -lpthread -ldl
#if define HTTPS
#SHAREDLIB += -lssl -lcrypto 
#if define LIBICAL
#SHAREDLIB += -lical -licalvcal

PROC_OPTION = DEFINE=_PROC_ MODE=ORACLE LINES=true CODE=CPP
ESQL_OPTION = -g
################OPTION END################
ESQL = esql
PROC = proc

$(OUTPUT):$(OBJS)
	$(LINK) $(LINKOPTION) $(LIBDIRS)  $(OBJS) $(SHAREDLIB)


clean: 
	rm -f $(OBJS)
	rm -f $(OUTPUT) $(STATIC_LIB)
all: clean $(OUTPUT)
.PRECIOUS:%.cpp %.c %.C
.SUFFIXES:
.SUFFIXES:  .c .o .cpp .ecpp .pc .ec .C .cc .cxx

.cpp.o:
	$(CPPCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR)  $*.cpp
	
.cc.o:
	$(CCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR)  $*.cpp

.cxx.o:
	$(CPPCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR)  $*.cpp

.c.o:
	$(CCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR) $*.c

.C.o:
	$(CPPCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR) $*.C	

.ecpp.C:
	$(ESQL) -e $(ESQL_OPTION) $(INCLUDEDIR) $*.ecpp 
	
.ec.c:
	$(ESQL) -e $(ESQL_OPTION) $(INCLUDEDIR) $*.ec
	
.pc.cpp:
	$(PROC)  CPP_SUFFIX=cpp $(PROC_OPTION)  $*.pc
