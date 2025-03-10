#!/bin/sh
# //%%%FILE%%%////////////////////////////////////////////////////////////////////
# // File Name: exports.sh
# //
# // History:
# //   Date   |Name | Description of modification
# // ---------|-----|-------------------------------------------------------------
# // 20/02/06 | soh | Creation.
# //%%%FILE%%%////////////////////////////////////////////////////////////////////

# sanity check(s)
command -v perl >/dev/null 2>&1 || { echo "Perl is not installed. Aborting." >&2; exit 1; }
PERL_SCRIPT=${ACE_ROOT}/bin/generate_export_file.pl
[ ! -f ${PERL_SCRIPT} ] && echo "ERROR: invalid script file \"${PERL_SCRIPT}\" (not a file), aborting" && exit 1

# generate exports file
perl ${PERL_SCRIPT} -n RPG_Net > ./net/rpg_net_exports.h
[ $? -ne 0 ] && echo "ERROR: failed to perl, aborting" && exit 1
perl ${PERL_SCRIPT} -n RPG_Net_Client > ./net/client/rpg_net_client_exports.h
[ $? -ne 0 ] && echo "ERROR: failed to perl, aborting" && exit 1
perl ${PERL_SCRIPT} -n RPG_Net_Server > ./net/server/rpg_net_server_exports.h
if [ $? -ne 0 ]; then
 echo "ERROR: failed to perl, aborting"
 exit 1
fi
