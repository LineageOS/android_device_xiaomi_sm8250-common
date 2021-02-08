# teardown mic
tinymix 'TX DEC0 MUX' 'MSM_DMIC'
tinymix 'TX SMIC MUX0' 'ZERO'
tinymix 'TX_AIF1_CAP Mixer DEC0' 0
tinymix 'ADC2_MIXER Switch' 0
tinymix 'IIR0 INP0 MUX' 'ZERO'
tinymix 'IIR0 INP0 Volume' 84
tinymix 'TX_DEC0 Volume' 84
echo "disable mic"
# teardown headphone
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
echo "disable headphone"
#teardown earpiece
tinymix 'RX INT0 DEM MUX' 'NORMAL_DSM_OUT'
tinymix 'RX INT0_1 MIX1 INP0' 'ZERO'
tinymix 'EAR_RDAC Switch' 0
tinymix 'RDAC3_MUX' 'RX1'
tinymix 'RX_EAR Mode' 'OFF'
tinymix 'RX_CDC_DMA_RX_0 Channels' 'One'
echo "disable earpiece"

