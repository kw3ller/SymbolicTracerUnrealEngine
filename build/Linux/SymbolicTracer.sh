#!/bin/sh
UE_TRUE_SCRIPT_NAME=$(echo \"$0\" | xargs readlink -f)
UE_PROJECT_ROOT=$(dirname "$UE_TRUE_SCRIPT_NAME")
chmod +x "$UE_PROJECT_ROOT/SymbolicTracer/Binaries/Linux/SymbolicTracer-Linux-Shipping"
"$UE_PROJECT_ROOT/SymbolicTracer/Binaries/Linux/SymbolicTracer-Linux-Shipping" SymbolicTracer "$@" 
