#include	/home/mServer/CommonLib/CommonLibSrc/make/makefile
#../../../CommonLib/CommonLibSrc/make/

#	CommonLib
	CommonPath	=	/home/mServer/CommonLib/CommonLibSrc
	ThirdParty	=	/home/Third-Party

main:
#
#
	g++	-c	$(CommonPath)/SString/String.cpp	$(CommonPath)/SString/StrImg.cpp	$(CommonPath)/SCommon/SClib.cpp	$(CommonPath)/SFile/SFile.cpp	$(CommonPath)/SFile/SConfig.cpp	$(CommonPath)/SFile/SLog.cpp	$(CommonPath)/SSystem/System.cpp	$(CommonPath)/STime/STime.cpp	$(CommonPath)/SGZip/SGZip.cpp	$(CommonPath)/SString/SConvert.cpp	$(CommonPath)/SJson/SJson.cpp
	ar	-crv	SCommon.a	sclib.o		strimg.o	string.o	sfile.o	sconfig.o	sconvert.o	sgzip.o	system.o	slog.o	sfile.o	stime.o	sjson.o
#	CMDir=../../../commonlib/commonsrc
	g++	-DEBUG	-o	test.exe	../main.cpp	../TestSConv.cpp	../TestSJson.cpp	../TestCString.cpp	../TestGZip.cpp	-L.	./SCommon.a	-L.		$(ThirdParty)/ZlibLinux/libz.a

clean:
	rm	-f	*.o	*.a	*.exe

#	g++	-o	main.exe	sclib.o	strimg.o	string.o	../main.cpp
#	../../../CommonLib/CommonLibSrc/make/scommon.a
