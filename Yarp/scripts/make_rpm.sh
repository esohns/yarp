#!/bin/sh
# author:      Erik Sohns <erik.sohns@web.de>
# this script creates a distribution RPM
# *TODO*: it is neither portable nor particularly stable !
# parameters:   - $1 ["tarball"] || "binary" || "source" || "all"
# return value: - 0 success, 1 failure

DEFAULT_PROJECT_DIR="${HOME}/Projects/RPG/Yarp"
PROJECT_DIR=${DEFAULT_PROJECT_DIR}
# sanity check(s)
[ ! -d ${PROJECT_DIR} ] && echo "ERROR: invalid project dir (was: \"${PROJECT_DIR}\"), aborting" && exit 1

# remember current dir...
pushd . >/dev/null 2>&1

cd ${PROJECT_DIR}

SPECFILE=./Yarp.spec
RPMBUILD_EXEC=$(which rpmbuild)

#sanity checks
[ ! -x ${RPMBUILD} ] && echo "ERROR: invalid \"rpmbuild\" executable \"${RPMBUILD}\" (not executable), aborting" && exit 1
[ ! -f ${SPECFILE} ] && echo "ERROR: invalid .spec file executable \"${SPECFILE}\" (not a file), aborting" && exit 1

DEFAULT_BUILD="tarball"
BUILD=${DEFAULT_BUILD}
[ "$#" -lt 1 ] && echo "using default build: \"${BUILD}\""

# parse any arguments
[ "$#" -ge 1 ] && BUILD=$1
# sanity check(s)
[ "${BUILD}" != "tarball" -a "${BUILD}" != "binary" -a "${BUILD}" != "source" -a "${BUILD}" != "all" ] && echo "ERROR: invalid build (was: \"${BUILD}\"), aborting" && exit 1

# set mode arguments for "rpmbuild"
BUILD_MODE="-ta"
[ "${BUILD}" == "binary" ] && BUILD_MODE="-bb"
[ "${BUILD}" == "source" ] && BUILD_MODE="-bs"
[ "${BUILD}" == "all" ] && BUILD_MODE="-ba"

# make distribution tarball
cd debug
echo "INFO: making distibution tarball..."
RETVAL=$(make dist >/dev/null 2>&1)
[ $? -ne 0 ] && echo "ERROR: failed to generate distribution tarball, aborting" && exit 1
for TARBALL in *.tar.gz; do
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
  mv -f ${RPM} rpm
done
echo "INFO: making rpm...DONE"

# clean up
echo "INFO: cleaning up source rpm..."
CMDLINE="${RPMBUILD_EXEC} --rmsource --rmspec ${SPECFILE}"
RETVAL=$(${CMDLINE} >/dev/null 2>&1)
[ $? -ne 0 ] && echo "ERROR: failed to clean up source rpm, aborting" && exit 1
echo "INFO: cleaning up source rpm...DONE"

# ...go back where we came from
popd >/dev/null 2>&1

exit 0
