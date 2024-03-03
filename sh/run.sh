#!/bin/bash

if [ "$DESKTOP_SESSION" == "i3" ] && [ "$CORGIDE_I3" == "NEW_WORKSPACE" ]; then
    i3-msg workspace 4
fi

./corgide