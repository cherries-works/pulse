#!/bin/sh
# ==============================================================================
# Organization  : cherries-works (https://github.com/cherries-works)
# Description   : Simple Installation script for cherries.works Pulse
# License       : MIT
# ==============================================================================

# Note that this installation script is not the
# complete installation script, instead it is just
# a one liner for to simplify install. For the
# full .sh file, go to;
#
# https://github.com/cherries-works/website
#
# Under src/install/* - you will find pulse.sh
#

set -e

curl -fsSL https://cherries.works/install/pulse.sh | sh