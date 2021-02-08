# $1: wave file to write
# $2: audio source
#     0: default
#     1: main mic
#     2: top mic
#     3: back mic
#     4: front mic
# $3: sample rate(Hz)
# $4: sample bit
# $5: channel number
# $6: capture duration(s)
# tinycap file.wav [-D card] [-d device] [-c channels] [-r rate] [-b bits] [-p period_size] [-n n_periods] [-t duration]
# sample usage: capture_audio.sh /data/test1.wav 1 48000 16 2 10

sleep 10
rate=KHZ_48


function enable_headset_mic
{
	echo "enabling headset mic"
	tinymix 'TX DEC0 MUX' 'SWR_MIC'
	tinymix 'TX SMIC MUX0' 'ADC1'
	tinymix 'TX_CDC_DMA_TX_3 Channels' 'One'
	tinymix 'TX_AIF1_CAP Mixer DEC0' 1
	tinymix 'ADC2 Volume' 6
	tinymix 'ADC2_MIXER Switch' 1
	tinymix 'HDR12 MUX' 'NO_HDR12'
	tinymix 'ADC2 MUX' 'INP2'
}

function disable_headset_mic
{
	echo "disabling headset mic"
	tinymix 'TX DEC0 MUX' 'ZERO'
	tinymix 'TX SMIC MUX0' 'ZERO'
	tinymix 'TX_CDC_DMA_TX_3 Channels' 'One'
	tinymix 'TX_AIF1_CAP Mixer DEC0' 0
	tinymix 'ADC2_MIXER Switch' 0
	tinymix 'ADC2 MUX' 'ZERO'
}

# setup
tinymix 'MultiMedia1 Mixer TX_CDC_DMA_TX_3' 1
tinymix 'TX_CDC_DMA_TX_3 SampleRate' $rate

enable_headset_mic

# start recording
tinycap /data/test1.wav -r 48000 -b 16 -T 10

disable_headset_mic

tinymix 'MultiMedia1 Mixer TX_CDC_DMA_TX_3' 0
tinymix 'TX_CDC_DMA_TX_3 SampleRate' 'KHZ_48'
