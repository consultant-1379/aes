#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       cdhls.sh
# Description:
#       A script to wrap the invocation of cdhls from the COM CLI.
# Note:
#       None.
##
# Usage:
#       None.
##
# Output:
#       None.
##
# Changelog:
# - Wed Oct 17 2012 - Tanu Aggarwal (xtanagg)
#       First version.
##

/usr/bin/sudo /opt/ap/aes/bin/cdhls "$@"

exit $?

