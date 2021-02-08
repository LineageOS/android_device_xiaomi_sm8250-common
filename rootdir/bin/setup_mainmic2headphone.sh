# setup main mic
tinymix 'TX DEC0 MUX' 'SWR_MIC'
tinymix 'TX SMIC MUX0' 'ADC0'
tinymix 'ADC1_MIXER Switch' 1
tinymix 'IIR0 INP0 MUX' 'DEC0'
tinymix 'IIR0 INP0 Volume' 84
tinymix 'ADC1 Volume' 12

# setup headphone
tinymix 'RX INT0 DEM MUX' 'CLSH_DSM_OUT'
tinymix 'RX INT1 DEM MUX' 'CLSH_DSM_OUT'
tinymix 'RX_CDC_DMA_RX_0 Channels' 'Two'
tinymix 'RX INT0_1 MIX1 INP0' 'IIR0'
tinymix 'RX INT1_1 MIX1 INP0' 'IIR0'
tinymix 'RX_HPH_PWR_MODE' 'LOHIFI'
tinymix 'RX HPH Mode' 'CLS_H_LOHIFI'
tinymix 'RX_COMP1 Switch' 0
tinymix 'RX_COMP2 Switch' 0
tinymix 'HPHL_COMP Switch' 0
tinymix 'HPHR_COMP Switch' 0
tinymix 'HPHL_RDAC Switch' 1
tinymix 'HPHR_RDAC Switch' 1
tinymix 'HPHL Volume' 20
tinymix 'HPHR Volume' 20

