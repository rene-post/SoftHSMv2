MAINTAINERCLEANFILES = 		$(srcdir)/Makefile.in

AM_CPPFLAGS = 			-I$(srcdir)/.. \
				-I$(srcdir)/../.. \
				-I$(srcdir)/../../common \
				-I$(srcdir)/../../cryptoki_compat \
				-I$(srcdir)/../../crypto \
				-I$(srcdir)/../../data_mgr \
				-I$(srcdir)/../../session_mgr \
				-I$(srcdir)/../../slot_mgr \
				-I$(srcdir) \
				@SQLITE3_INCLUDES@

check_PROGRAMS =		objstorespec

objstorespec_SOURCES =		objstorespec.cpp \
				ObjectStoreSpec.cpp \
				ObjectFileSpec.cpp \
				TokenDBSpec.cpp \
				DBObjectSpec.cpp \
				DBTokenSpec.cpp

objstorespec_LDADD =		../../libsofthsm_convarch.la 

objstorespec_LDFLAGS = 		 @SQLITE3_LIBS@ @CRYPTO_LIBS@ -no-install -pthread

TESTS = 			objstorespec

EXTRA_DIST =			$(srcdir)/*.h

