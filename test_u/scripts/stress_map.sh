#!/bin/bash
# author:      Erik Sohns <erik.sohns@web.de>
# this script stress tests the map_generator
# parameters:   - $1 number of runs
# return value: - 0 success, 1 failure

MAP_GENERATOR_EXEC=$(which map_generator)
#PARAM_LIST="-a 0 -m -r 10 -s -t -x 40 -y 20"
PARAM_LIST="-a 0 -m -r 15 -s -x 80 -y 40"

# set defaults
DEF_NUM_RUNS=100
NUM_RUNS=${DEF_NUM_RUNS}
[ "$#" -lt "1" ] && echo "using default #runs: ${NUM_RUNS}"
DEF_OUT="/var/tmp/map_generator.out"
DEF_LOG="/var/tmp/map_generator.log"

# parse any arguments
[ "$#" -ge "1" ] && NUM_RUNS=$1 && echo "#runs: ${NUM_RUNS}"

# sanity check(s)
[ ! -x ${MAP_GENERATOR_EXEC} ] && echo "ERROR: invalid map_generator executable \"${MAP_GENERATOR_EXEC}\" (not executable), aborting" && exit 1

CMDLINE="${MAP_GENERATOR_EXEC} ${PARAM_LIST}"
for (( i=0; i<${NUM_RUNS}; i++ )) ; do
  echo "INFO: running iteration #${i}..."
  RETVAL=$(${CMDLINE} >>${DEF_OUT} 2>>${DEF_LOG})
  [ $? -ne 0 ] && echo "ERROR: failed run (${RETVAL}), aborting" && exit 1
done

exit 0
