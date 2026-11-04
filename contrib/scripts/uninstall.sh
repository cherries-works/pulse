#!/bin/sh
# ==============================================================================
# Organization  : cherries-works (https://github.com/cherries-works)
# Description   : Simple Uninstallation script for cherries.works Pulse
# License       : MIT
# ==============================================================================

set -e

echo "Removing binary!"
rm ~/.local/bin/pulse

echo "Removing directory!"
rm -rf ~/.cherries-works/pulse/ 

echo "Complete!"