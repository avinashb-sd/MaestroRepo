LIB_PATH 			= ../../Include/
LIB_NAME_RELEASE	= libfmodex.dylib
LIB_NAME_LOGGING	= libfmodexL.dylib
ARCH				= -arch i386 -arch x86_64

release: GroupA.cpp
	g++ -O3 ${ARCH} -o GroupA GroupA.cpp ${LIB_PATH}/${LIB_NAME_RELEASE}
	install_name_tool -change ./${LIB_NAME_RELEASE} ${LIB_PATH}/${LIB_NAME_RELEASE} GroupA
	g++ -O3 ${ARCH} -o GroupB GroupB.cpp ${LIB_PATH}/${LIB_NAME_RELEASE}
	install_name_tool -change ./${LIB_NAME_RELEASE} ${LIB_PATH}/${LIB_NAME_RELEASE} GroupB

debug: GroupB.cpp
	g++ -O0 ${ARCH} -g -o GroupA GroupA.cpp ${LIB_PATH}/${LIB_NAME_LOGGING}
	install_name_tool -change ./${LIB_NAME_LOGGING} ${LIB_PATH}/${LIB_NAME_LOGGING} GroupA
		g++ -O0 ${ARCH} -g -o GroupB GroupB.cpp ${LIB_PATH}/${LIB_NAME_LOGGING}
	install_name_tool -change ./${LIB_NAME_LOGGING} ${LIB_PATH}/${LIB_NAME_LOGGING} GroupB

clean:
	rm -f GroupA
	rm -f GroupB
