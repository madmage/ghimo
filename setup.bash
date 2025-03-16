SCRIPT_PATH=$(realpath "$(dirname "${BASH_SOURCE[0]}")")
export PYTHONPATH=$PYTHONPATH:$SCRIPT_PATH/ghimopy:$SCRIPT_PATH/install/python
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SCRIPT_PATH/install/lib
