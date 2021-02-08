set -x
# $1: device for output
#     spk: stereo speaker
#     top-spk: top speaker
#     bot-spk: bottom speaker
#     rcv: receiver
#     spk_hp: speaker high power
#     top-spk_hp: top speaker high power
#     bot-spk_hp: bottom speaker high power
#     rcv_hp: headphone
#     us: ultrasound

# tinyplay file.wav [-D card] [-d device] [-p period_size] [-n n_periods]
# sample usage: playback.sh spk
# rcv.wav:-4.5dbfs   spk: -4.8dbfs  ultra: -4.5dbfs  spk_hp:-1.8dbfs

function enable_receiver
{
    echo "enabling receiver"
    tinymix 'RX_MACRO RX0 MUX' 'AIF1_PB'
    tinymix 'RX_CDC_DMA_RX_0 Channels' 'One'
    tinymix 'RX INT0_1 MIX1 INP0' 'RX0'
    tinymix 'RX INT0 DEM MUX' 'CLSH_DSM_OUT'
    tinymix 'RX_CDC_DMA_RX_0 Audio Mixer MultiMedia1' 1
    tinymix 'EAR_RDAC Switch' 1
    tinymix 'RDAC3_MUX' 'RX1'
    tinymix 'EAR PA Gain' 'G_6_DB'
    tinymix 'RX0 Digital Volume' 84
    tinymix 'RX_EAR Mode' 'ON'
}

function disable_receiver
{
    echo "disabling receiver"
    tinymix 'RX_MACRO RX0 MUX' 'ZERO'
    tinymix 'RX_CDC_DMA_RX_0 Channels' 'One'
    tinymix 'RX INT0_1 MIX1 INP0' 'ZERO'
    tinymix 'RX INT0 DEM MUX' 'NORMAL_DSM_OUT'
    tinymix 'RX_CDC_DMA_RX_0 Audio Mixer MultiMedia1' 0
    tinymix 'EAR_RDAC Switch' 0
    tinymix 'RDAC3_MUX' 'ZERO'
    tinymix 'RX_EAR Mode' 'OFF'
}

function enable_speaker
{
    echo "enabling speaker"
    tinymix 'PRI_MI2S_RX Audio Mixer MultiMedia1' 1
    tinymix 'PRIM_MI2S_RX Channels' 'Two'
    sleep 1
}

function disable_speaker
{
    echo "disabling speaker"
    tinymix 'PRI_MI2S_RX Audio Mixer MultiMedia1' 0
}

function enable_speaker_top
{
    echo "enabling speaker"
    tinymix 'PRI_MI2S_RX Audio Mixer MultiMedia1' 1
    tinymix 'PRIM_MI2S_RX Channels' 'Two'
    sleep 1
}

function disable_speaker_top
{
    echo "disabling speaker top"
    echo "disabling speaker"
    tinymix 'PRI_MI2S_RX Audio Mixer MultiMedia1' 0
}
function enable_speaker_bot
{
    echo "enabling speaker"
    tinymix 'PRI_MI2S_RX Audio Mixer MultiMedia1' 1
    tinymix 'PRIM_MI2S_RX Channels' 'Two'
    sleep 1
}

function disable_speaker_bot
{
    echo "disabling speaker"
    tinymix 'PRI_MI2S_RX Audio Mixer MultiMedia1' 0
}

function enable_headphone
{
    echo "enabling headphone"
    tinymix 'RX_CDC_DMA_RX_0 Audio Mixer MultiMedia1' 1
    tinymix 'RX_MACRO RX0 MUX' 'AIF1_PB'
    tinymix 'RX_MACRO RX1 MUX' 'AIF1_PB'
    tinymix 'RX_CDC_DMA_RX_0 Channels' 'Two'
    tinymix 'RX INT0_1 MIX1 INP0' 'RX0'
    tinymix 'RX INT1_1 MIX1 INP0' 'RX1'
    tinymix 'RX INT0 DEM MUX' 'CLSH_DSM_OUT'
    tinymix 'RX INT1 DEM MUX' 'CLSH_DSM_OUT'
    tinymix 'RX_HPH_PWR_MODE' 'LOHIFI'
    tinymix 'RX HPH Mode' 'CLS_H_LOHIFI'
    tinymix 'RX_COMP1 Switch' 1
    tinymix 'RX_COMP2 Switch' 1
    tinymix 'HPHL_COMP Switch' 1
    tinymix 'HPHR_COMP Switch' 1
    tinymix 'HPHL_RDAC Switch' 1
    tinymix 'HPHR_RDAC Switch' 1
}

function disable_headphone
{
    echo "disabling headphone"
    tinymix 'RX_CDC_DMA_RX_0 Audio Mixer MultiMedia1' 0
    tinymix 'RX_MACRO RX0 MUX' 'ZERO'
    tinymix 'RX_MACRO RX1 MUX' 'ZERO'
    tinymix 'RX_CDC_DMA_RX_0 Channels' 'One'
    tinymix 'RX INT0_1 MIX1 INP0' 'ZERO'
    tinymix 'RX INT1_1 MIX1 INP0' 'ZERO'
    tinymix 'RX INT0 DEM MUX' 'NORMAL_DSM_OUT'
    tinymix 'RX INT1 DEM MUX' 'NORMAL_DSM_OUT'
    tinymix 'RX_COMP1 Switch' 0
    tinymix 'RX_COMP2 Switch' 0
    tinymix 'HPHL_COMP Switch' 0
    tinymix 'HPHR_COMP Switch' 0
    tinymix 'HPHL_RDAC Switch' 0
    tinymix 'HPHR_RDAC Switch' 0
}

function enable_ultrasound
{
    echo "enable ultrasound"
    tinymix 'RX_EAR Mode' 'ON'
    tinymix 'RX_CDC_DMA_RX_1 SampleRate' 'KHZ_96'
    tinymix 'RX_MACRO RX2 MUX' 'AIF2_PB'
    tinymix 'RX_CDC_DMA_RX_1 Channels' 'One'
    tinymix 'RX INT0_2 MUX' 'RX2'
    tinymix 'RX INT0 DEM MUX' 'CLSH_DSM_OUT'
    tinymix 'RX_CDC_DMA_RX_1 Audio Mixer MultiMedia1' 1
    tinymix 'EAR_RDAC Switch' 1
    tinymix 'RDAC3_MUX' 'RX1'
    tinymix 'EAR PA Gain' 'G_6_DB'
    tinymix 'RX_RX0 Digital Volume' 84
}

function disable_ultrasound
{
    echo "disable ultrasound"
    tinymix 'RX_CDC_DMA_RX_1 SampleRate' 'KHZ_48'
    tinymix 'RX_MACRO RX2 MUX' 'ZERO'
    tinymix 'RX_CDC_DMA_RX_1 Channels' 'One'
    tinymix 'RX INT0_2 MUX' 'ZERO'
    tinymix 'RX INT0 DEM MUX' 'NORMAL_DSM_OUT'
    tinymix 'RX_CDC_DMA_RX_1 Audio Mixer MultiMedia1' 0
    tinymix 'EAR_RDAC Switch' 0
    tinymix 'RDAC3_MUX' 'ZERO'
    tinymix 'RX_EAR Mode' 'OFF'
}

if [ "$1" = "spk" ]; then
    enable_speaker
    filename=/vendor/etc/spk.wav
elif [ "$1" = "top-spk" ]; then
    enable_speaker_top
    filename=/vendor/etc/spk.wav
elif [ "$1" = "bot-spk" ]; then
    enable_speaker_bot
    filename=/vendor/etc/spk.wav
elif [ "$1" = "rcv" ]; then
    enable_receiver
    filename=/vendor/etc/rcv.wav
elif [ "$1" = "spk_hp" ]; then
    enable_speaker
    filename=/vendor/etc/spk_hp.wav
elif [ "$1" = "top-spk_hp" ]; then
    enable_speaker_top
    filename=/vendor/etc/spk_hp.wav
elif [ "$1" = "bot-spk_hp" ]; then
    enable_speaker_bot
    filename=/vendor/etc/spk_hp.wav
elif [ "$1" = "rcv_hp" ]; then
    enable_headphone
    filename=/vendor/etc/rcv.wav
elif [ "$1" = "us" ]; then
    enable_ultrasound
    filename=/vendor/etc/ultrasound.wav
else
    echo "Usage: playback.sh device; device: spk or rcv or spk_hp or us"
fi

echo "start playing"
tinyplay $filename

if [ "$1" = "spk" ]; then
    disable_speaker
elif [ "$1" = "top-spk" ]; then
    disable_speaker_top
elif [ "$1" = "bot-spk" ]; then
    disable_speaker_bot
elif [ "$1" = "rcv" ]; then
    disable_receiver
elif [ "$1" = "spk_hp" ]; then
    disable_speaker
elif [ "$1" = "top-spk_hp" ]; then
    disable_speaker_top
elif [ "$1" = "bot-spk_hp" ]; then
    disable_speaker_bot
elif [ "$1" = "rcv_hp" ]; then
    disable_headphone
elif [ "$1" = "us" ]; then
    disable_ultrasound
fi

exit 0
