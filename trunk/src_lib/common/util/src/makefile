include ../../../makefile.comm
MYINC= -I../include/ -I./ -I../../app/include -I${BOSS_BASE_PATH}/thirdparty/mgw/include
#自己的可执行文件
LIB_OBJ =  textlink_mgr.o url_decode.o url_encode.o sem_lock.o buffered_file_reader.o \
		   client_key.o datetime.o datetimev2.o net.o  base64.o lib_boss_net.o \
		   inifile.o lib_boss_sockstack.o tcplib.o \
		   qq_agent_login.o httpfun.o facility.o \
		   client.o base64_2.o ini.o md5c.o file_rwlock.o

LIB_OBJ_CACHE = cacheoper.o shmqueue.o cacheoperV2.o
LIB_OBJ_REACTOR = reactor.o
LIB_OBJ_OIDB = get_qqdetail.o get_qqfriends.o send_qqmsg.o oidb_client.o
LIB_OBJ_SM = send_qqmsg.o
LIB_OBJ_SM2 = send_qqmsg_v2.o send_qqmsg.o
LIB_OBJ_CLASSCACHE = commodity_class_cache.o commodity_class_cache_v2.o
LIB_OBJ_BDBSTORAGE = bdb_storage.o
LIB_OBJ_ICONV_STRING = iconv_string.o
OUTPUT = libqueuenolock.a libbitmap.a libbossutil.a libcache.a libreactor.a liboidb.a libclasscache.a libsend_message.a libsend_message2.a libbdbstorage.a libiconvstring.a
all:$(OUTPUT)
	$(AR) -crl libbossutil.a  $(LIB_OBJ)
	$(AR) -crl libcache.a $(LIB_OBJ_CACHE)
	$(AR) -crl libreactor.a $(LIB_OBJ_REACTOR)
	$(AR) -crl liboidb.a $(LIB_OBJ_OIDB)
	$(AR) -crl libsend_message.a $(LIB_OBJ_SM)
	$(AR) -crl libsend_message2.a $(LIB_OBJ_SM2)
	$(AR) -crl libclasscache.a $(LIB_OBJ_CLASSCACHE)
	$(AR) -crl libbdbstorage.a $(LIB_OBJ_BDBSTORAGE)
	$(AR) -crl libbitmap.a bitmap_single.o
	$(AR) -crl libiconvstring.a $(LIB_OBJ_ICONV_STRING)
	cp -f $(OUTPUT) $(LIB_COMM_PATH)

libreactor.a: $(LIB_OBJ_REACTOR)
liboidb.a: $(LIB_OBJ_OIDB)
libsend_message.a: $(LIB_OBJ_SM)
libsend_message2.a: $(LIB_OBJ_SM2)
libclasscache.a:$(LIB_OBJ_CLASSCACHE)
libbossutil.a: $(LIB_OBJ)
libbdbstorage.a: $(LIB_OBJ_BDBSTORAGE)
libbitmap.a: bitmap_single.o
libiconvstring.a: $(LIB_OBJ_ICONV_STRING)

libcache.a:$(LIB_OBJ_CACHE)

libqueuenolock.a:shmqueuenolock.o
	$(AR) -crl $@ $^
	cp -f $@ $(LIB_COMM_PATH)

clean:
	rm -f *.o *.a $(OUTPUT)
