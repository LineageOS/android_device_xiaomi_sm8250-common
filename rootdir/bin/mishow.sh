local chg_en=1

if [ -n "$1" ]; then
    chg_en=$1
fi

#echo "MIShow Control"

if [ "$chg_en" -eq "0" ]; then
# Disable charging
    echo 1 > /sys/class/power_supply/battery/input_suspend
else
# enable charging
    echo 0 > /sys/class/power_supply/battery/input_suspend
fi
