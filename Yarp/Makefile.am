METASOURCES = AUTO

#ACLOCAL_AMFLAGS = -I m4

SUBDIRS = \
  tools \
  . \
  chance \
  common \
  magic \
  item \
  character \
  combat \
  map \
  graphics \
  sound \
  engine \
  net \
  client \
  test_u \
  scripts

#DIST_SUBDIRS = $(SUBDIRS) scripts

.PHONY: libs test
libs: tools common chance character magic item combat map sound graphics engine client
	echo "finished making libraries"
test: libs test_u
	echo "finished making unit tests"
all-local: libs test

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = Yarp-${YARP_API_VERSION}.pc

