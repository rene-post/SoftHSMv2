MAINTAINERCLEANFILES = 			$(srcdir)/Makefile.in

AM_CPPFLAGS =				-I$(srcdir)/../cryptoki_compat \
					-I$(srcdir)/../crypto \
					-I$(srcdir)/../data_mgr \
					-I$(srcdir)/../common \
					-I$(srcdir)/.. \
					@SQLITE3_INCLUDES@

noinst_LTLIBRARIES =			libsofthsm_objectstore.la
libsofthsm_objectstore_la_SOURCES =	ObjectStore.cpp \
					UUID.cpp \
					Directory.cpp \
					File.cpp \
					OSAttribute.cpp \
					OSToken.cpp \
					FileToken.cpp \
					DBToken.cpp \
					ObjectFile.cpp \
					SessionObject.cpp \
					SessionObjectStore.cpp \
					FindOperation.cpp \
					TokenDB.cpp

libsofthsm_objectstore_la_LDFLAGS =	@SQLITE3_LIBS@

SUBDIRS = 				test spec

EXTRA_DIST =				$(srcdir)/*.h

