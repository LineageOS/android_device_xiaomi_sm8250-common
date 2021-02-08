# setup headsetmic
tinymix 'TX DEC0 MUX' 'SWR_MIC'
tinymix 'TX SMIC MUX0' 'ADC1'
tinymix 'ADC2 Volume' 9
tinymix 'ADC2_MIXER Switch' 1
tinymix 'HDR12 MUX' 'NO_HDR12'
tinymix 'ADC2 MUX' 'INP2'
tinymix 'IIR0 INP0 MUX' 'DEC0'
tinymix 'IIR0 INP0 Volume' 84
# setup earpiecea
echo "enabling receiver"
tinymix 'RX INT0_1 MIX1 INP0' 'IIR0'
tinymix 'RX INT0 DEM MUX' 'CLSH_DSM_OUT'
tinymix 'EAR_RDAC Switch' 1
tinymix 'RDAC3_MUX' 'RX1'
tinymix 'EAR PA Gain' 'G_6_DB'
tinymix 'RX_EAR Mode' 'ON'
