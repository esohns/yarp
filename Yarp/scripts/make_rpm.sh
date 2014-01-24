#!/bin/sh
# author:      Erik Sohns <erik.sohns@web.de>
# this script creates a distribution RPM
# *TODO*: it is neither portable nor particularly stable !
# parameters:   - $1 ["tarball"] || "binary" || "source" || "all"
# return value: - 0 success, 1 failure

RPMBUILD_EXEC=$(which rpmbuild)
#sanity checks
[ ! -x ${RPMBUILD_EXEC} ] && echo "ERROR: invalid \"rpmbuild\" executable \"${RPMBUILD_EXEC}\" (not executable), aborting" && exit 1

DEFAULT_PROJECT_DIR="${HOME}/Projects/yarp/Yarp"
PROJECT_DIR=${DEFAULT_PROJECT_DIR}
# sanity check(s)
[ ! -d ${PROJECT_DIR} ] && echo "ERROR: invalid project dir (was: \"${PROJECT_DIR}\"), aborting" && exit 1

TARGET_DIR=${PROJECT_DIR}/releases
# sanity check(s)
[ ! -d ${TARGET_DIR} ] && echo "ERROR: invalid target dir (was: \"${TARGET_DIR}\"), aborting" && exit 1

SPECFILE=${PROJECT_DIR}/scripts/Yarp.spec
[ ! -f ${SPECFILE} ] && echo "ERROR: invalid .spec file \"${SPECFILE}\" (not a file), aborting" && exit 1

DEFAULT_BUILD="tarball"
BUILD=${DEFAULT_BUILD}
[ "$#" -lt 1 ] && echo "using default build: \"${BUILD}\""

# parse any arguments
[ "$#" -ge 1 ] && BUILD=$1
# sanity check(s)
[ "${BUILD}" != "tarball" -a "${BUILD}" != "binary" -a "${BUILD}" != "source" -a "${BUILD}" != "all" ] && echo "ERROR: invalid build (was: \"${BUILD}\"), aborting" && exit 1

# set build mode arguments for "rpmbuild"
BUILD_MODE="-ta"
[ "${BUILD}" == "binary" ] && BUILD_MODE="-bb"
[ "${BUILD}" == "source" ] && BUILD_MODE="-bs"
[ "${BUILD}" == "all" ] && BUILD_MODE="-ba"

# remember current dir...
pushd . >/dev/null 2>&1

# make distribution tarball
BUILD_DIR=${PROJECT_DIR}/build/debug
# sanity check(s)
[ ! -d ${BUILD_DIR} ] && echo "ERROR: invalid build dir (was: \"${BUILD_DIR}\"), aborting" && exit 1
cd ${BUILD_DIR}
echo "INFO: making distibution tarball..."
RETVAL=$(make dist >/dev/null 2>&1)
[ $? -ne 0 ] && echo "ERROR: failed to generate distribution tarball, aborting" && exit 1
for TARBALL in *.tar.gz; do
#*NOTE*: rename the tarball to lower-case - necessary for rpmbuild :(, see below
#  LOWER_TARBALL=$(echo ${TARBALL} | tr [:upper:] [:lower:])
#  mv ${TARBALL} ${LOWER_TARBALL}
#  echo "INFO: created \"${LOWER_TARBALL}\"..."
#  TARBALL=${LOWER_TARBALL}
  echo "INFO: created \"${TARBALL}\"..."
done
echo "INFO: making distribution tarball...DONE"

# make rpm
if [ "${BUILD}" != "tarball" ]; then
  CMDLINE="${RPMBUILD_EXEC} ${BUILD_MODE} ${SPECFILE}"
else
  CMDLINE="${RPMBUILD_EXEC} ${BUILD_MODE} ${TARBALL}"
fi
echo "INFO: making rpm..."
RETVAL=$(${CMDLINE} >/dev/null 2>&1)
[ $? -ne 0 ] && echo "ERROR: failed to make rpm, aborting" && exit 1
for RPM in ${HOME}/rpmbuild/RPMS/i686/*.rpm; do
  echo "INFO: created \"`basename ${RPM}`\"..."
  mv -f ${RPM} ${TARGET_DIR}
  [ $? -ne 0 ] && echo "ERROR: failed to move rpm, aborting" && exit 1
done
echo "INFO: making rpm...DONE"

# clean up
echo "INFO: cleaning up ..."
if [ "${BUILD}" != "tarball" ]; then
  CMDLINE="${RPMBUILD_EXEC} --rmsource --rmspec ${SPECFILE}"
  RETVAL=$(${CMDLINE} >/dev/null 2>&1)
  [ $? -ne 0 ] && echo "ERROR: failed to clean up source, aborting" && exit 1
else
  rm -f ${TARBALL}
  [ $? -ne 0 ] && echo "ERROR: failed to clean up tarball, aborting" && exit 1
fi
echo "INFO: cleaning up ...DONE"

# ...go back
popd >/dev/null 2>&1
