#include "fsl_common.h"
#include "fsl_rdc.h"
#include "rdc_dbg.h"
#include "fsl_debug_console.h"


static rdc_profile_t RDC_Master_profile[] = {
    {.id=kRDC_Master_A53, .name="kRDC_Master_A53"},
    {.id=kRDC_Master_M7, .name="kRDC_Master_M7"},
    {.id=kRDC_PCIE_CTRL1, .name="kRDC_PCIE_CTRL1"},
    {.id=kRDC_Master_SDMA3_PERIPH, .name="kRDC_Master_SDMA3_PERIPH"},
    {.id=kRDC_Master_SDMA3_BURST, .name="kRDC_Master_SDMA3_BURST"},
    {.id=kRDC_Master_LCDIF1, .name="kRDC_Master_LCDIF1"},
    {.id=kRDC_Master_ISI, .name="kRDC_Master_ISI"},
    {.id=kRDC_Master_NPU, .name="kRDC_Master_NPU"},
    {.id=kRDC_Master_Coresight, .name="kRDC_Master_Coresight"},
    {.id=kRDC_Master_DAP, .name="kRDC_Master_DAP"},
    {.id=kRDC_Master_CAAM, .name="kRDC_Master_CAAM"},
    {.id=kRDC_Master_SDMA1_PERIPH, .name="kRDC_Master_SDMA1_PERIPH"},
    {.id=kRDC_Master_SDMA1_BURST, .name="kRDC_Master_SDMA1_BURST"},
    {.id=kRDC_Master_APBHDMA, .name="kRDC_Master_APBHDMA"},
    {.id=kRDC_Master_RAWNAND, .name="kRDC_Master_RAWNAND"},
    {.id=kRDC_Master_USDHC1, .name="kRDC_Master_USDHC1"},
    {.id=kRDC_Master_USDHC2, .name="kRDC_Master_USDHC2"},
    {.id=kRDC_Master_USDHC3, .name="kRDC_Master_USDHC3"},
    {.id=kRDC_Master_AUDIO_PROCESSOR, .name="kRDC_Master_AUDIO_PROCESSOR"},
    {.id=kRDC_Master_USB1, .name="kRDC_Master_USB1"},
    {.id=kRDC_Master_USB2, .name="kRDC_Master_USB2"},
    {.id=kRDC_Master_TESTPORT, .name="kRDC_Master_TESTPORT"},
    {.id=kRDC_Master_ENET1TX, .name="kRDC_Master_ENET1TX"},
    {.id=kRDC_Master_ENET1RX, .name="kRDC_Master_ENET1RX"},
    {.id=kRDC_Master_SDMA2_PERIPH, .name="kRDC_Master_SDMA2_PERIPH"},
    {.id=kRDC_Master_SDMA2_BURST, .name="kRDC_Master_SDMA2_BURST"},
    {.id=kRDC_Master_SDMA2_SPBA2, .name="kRDC_Master_SDMA2_SPBA2"},
    {.id=kRDC_Master_SDMA3_SPBA2, .name="kRDC_Master_SDMA3_SPBA2"},
    {.id=kRDC_Master_SDMA1_SPBA1, .name="kRDC_Master_SDMA1_SPBA1"},
    {.id=kRDC_Master_LCDIF2, .name="kRDC_Master_LCDIF2"},
    {.id=kRDC_Master_HDMI_TX, .name="kRDC_Master_HDMI_TX"},
    {.id=kRDC_Master_ENET2, .name="kRDC_Master_ENET2"},
    {.id=kRDC_Master_GPU3D, .name="kRDC_Master_GPU3D"},
    {.id=kRDC_Master_GPU2D, .name="kRDC_Master_GPU2D"},
    {.id=kRDC_Master_VPU_G1, .name="kRDC_Master_VPU_G1"},
    {.id=kRDC_Master_VPU_G2, .name="kRDC_Master_VPU_G2"},
    {.id=kRDC_Master_VPU_VC8000E, .name="kRDC_Master_VPU_VC8000E"},
    {.id=kRDC_Master_AUDIO_EDMA, .name="kRDC_Master_AUDIO_EDMA"},
    {.id=kRDC_Master_ISP1, .name="kRDC_Master_ISP1"},
    {.id=kRDC_Master_ISP2, .name="kRDC_Master_ISP2"},
    {.id=kRDC_Master_DEWARP, .name="kRDC_Master_DEWARP"},
    {.id=kRDC_Master_GIC500, .name="kRDC_Master_GIC500"},
};


static rdc_profile_t RDC_Mem_profile[] = {
    {.id=kRDC_Mem_MRC0_0, .name="kRDC_Mem_MRC0_0"},
    {.id=kRDC_Mem_MRC0_1, .name="kRDC_Mem_MRC0_1"},
    {.id=kRDC_Mem_MRC0_2, .name="kRDC_Mem_MRC0_2"},
    {.id=kRDC_Mem_MRC0_3, .name="kRDC_Mem_MRC0_3"},
    {.id=kRDC_Mem_MRC1_0, .name="kRDC_Mem_MRC1_0"},
    {.id=kRDC_Mem_MRC1_1, .name="kRDC_Mem_MRC1_1"},
    {.id=kRDC_Mem_MRC1_2, .name="kRDC_Mem_MRC1_2"},
    {.id=kRDC_Mem_MRC1_3, .name="kRDC_Mem_MRC1_3"},
    {.id=kRDC_Mem_MRC1_4, .name="kRDC_Mem_MRC1_4"},
    {.id=kRDC_Mem_MRC1_5, .name="kRDC_Mem_MRC1_5"},
    {.id=kRDC_Mem_MRC1_6, .name="kRDC_Mem_MRC1_6"},
    {.id=kRDC_Mem_MRC1_7, .name="kRDC_Mem_MRC1_7"},
    {.id=kRDC_Mem_MRC2_0, .name="kRDC_Mem_MRC2_0"},
    {.id=kRDC_Mem_MRC2_1, .name="kRDC_Mem_MRC2_1"},
    {.id=kRDC_Mem_MRC2_2, .name="kRDC_Mem_MRC2_2"},
    {.id=kRDC_Mem_MRC2_3, .name="kRDC_Mem_MRC2_3"},
    {.id=kRDC_Mem_MRC2_4, .name="kRDC_Mem_MRC2_4"},
    {.id=kRDC_Mem_MRC3_0, .name="kRDC_Mem_MRC3_0"},
    {.id=kRDC_Mem_MRC3_1, .name="kRDC_Mem_MRC3_1"},
    {.id=kRDC_Mem_MRC3_2, .name="kRDC_Mem_MRC3_2"},
    {.id=kRDC_Mem_MRC3_3, .name="kRDC_Mem_MRC3_3"},
    {.id=kRDC_Mem_MRC3_4, .name="kRDC_Mem_MRC3_4"},
    {.id=kRDC_Mem_MRC4_0, .name="kRDC_Mem_MRC4_0"},
    {.id=kRDC_Mem_MRC4_1, .name="kRDC_Mem_MRC4_1"},
    {.id=kRDC_Mem_MRC4_2, .name="kRDC_Mem_MRC4_2"},
    {.id=kRDC_Mem_MRC4_3, .name="kRDC_Mem_MRC4_3"},
    {.id=kRDC_Mem_MRC4_4, .name="kRDC_Mem_MRC4_4"},
    {.id=kRDC_Mem_MRC5_0, .name="kRDC_Mem_MRC5_0"},
    {.id=kRDC_Mem_MRC5_1, .name="kRDC_Mem_MRC5_1"},
    {.id=kRDC_Mem_MRC5_2, .name="kRDC_Mem_MRC5_2"},
    {.id=kRDC_Mem_MRC5_3, .name="kRDC_Mem_MRC5_3"},
    {.id=kRDC_Mem_MRC6_0, .name="kRDC_Mem_MRC6_0"},
    {.id=kRDC_Mem_MRC6_1, .name="kRDC_Mem_MRC6_1"},
    {.id=kRDC_Mem_MRC6_2, .name="kRDC_Mem_MRC6_2"},
    {.id=kRDC_Mem_MRC6_3, .name="kRDC_Mem_MRC6_3"},
    {.id=kRDC_Mem_MRC6_4, .name="kRDC_Mem_MRC6_4"},
    {.id=kRDC_Mem_MRC6_5, .name="kRDC_Mem_MRC6_5"},
    {.id=kRDC_Mem_MRC6_6, .name="kRDC_Mem_MRC6_6"},
    {.id=kRDC_Mem_MRC6_7, .name="kRDC_Mem_MRC6_7"},
    {.id=kRDC_Mem_MRC7_0, .name="kRDC_Mem_MRC7_0"},
    {.id=kRDC_Mem_MRC7_1, .name="kRDC_Mem_MRC7_1"},
    {.id=kRDC_Mem_MRC7_2, .name="kRDC_Mem_MRC7_2"},
    {.id=kRDC_Mem_MRC7_3, .name="kRDC_Mem_MRC7_3"},
    {.id=kRDC_Mem_MRC7_4, .name="kRDC_Mem_MRC7_4"},
    {.id=kRDC_Mem_MRC7_5, .name="kRDC_Mem_MRC7_5"},
    {.id=kRDC_Mem_MRC7_6, .name="kRDC_Mem_MRC7_6"},
    {.id=kRDC_Mem_MRC7_7, .name="kRDC_Mem_MRC7_7"},
    {.id=kRDC_Mem_MRC8_0, .name="kRDC_Mem_MRC8_0"},
    {.id=kRDC_Mem_MRC8_1, .name="kRDC_Mem_MRC8_1"},
    {.id=kRDC_Mem_MRC8_2, .name="kRDC_Mem_MRC8_2"},
    {.id=kRDC_Mem_MRC8_3, .name="kRDC_Mem_MRC8_3"},
    {.id=kRDC_Mem_MRC8_4, .name="kRDC_Mem_MRC8_4"},
    {.id=kRDC_Mem_MRC9_0, .name="kRDC_Mem_MRC9_0"},
    {.id=kRDC_Mem_MRC9_1, .name="kRDC_Mem_MRC9_1"},
    {.id=kRDC_Mem_MRC9_2, .name="kRDC_Mem_MRC9_2"},
    {.id=kRDC_Mem_MRC9_3, .name="kRDC_Mem_MRC9_3"},
    {.id=kRDC_Mem_MRC9_4, .name="kRDC_Mem_MRC9_4"},
    {.id=kRDC_Mem_MRC9_5, .name="kRDC_Mem_MRC9_5"},
    {.id=kRDC_Mem_MRC9_6, .name="kRDC_Mem_MRC9_6"},
    {.id=kRDC_Mem_MRC9_7, .name="kRDC_Mem_MRC9_7"},
    {.id=kRDC_Mem_MRC10_0, .name="kRDC_Mem_MRC10_0"},
    {.id=kRDC_Mem_MRC10_1, .name="kRDC_Mem_MRC10_1"},
    {.id=kRDC_Mem_MRC10_2, .name="kRDC_Mem_MRC10_2"},
    {.id=kRDC_Mem_MRC10_3, .name="kRDC_Mem_MRC10_3"},
    {.id=kRDC_Mem_MRC11_0, .name="kRDC_Mem_MRC11_0"},
    {.id=kRDC_Mem_MRC11_1, .name="kRDC_Mem_MRC11_1"},
    {.id=kRDC_Mem_MRC11_2, .name="kRDC_Mem_MRC11_2"},
    {.id=kRDC_Mem_MRC11_3, .name="kRDC_Mem_MRC11_3"},
    {.id=kRDC_Mem_MRC12_0, .name="kRDC_Mem_MRC12_0"},
    {.id=kRDC_Mem_MRC12_1, .name="kRDC_Mem_MRC12_1"},
    {.id=kRDC_Mem_MRC12_2, .name="kRDC_Mem_MRC12_2"},
    {.id=kRDC_Mem_MRC12_3, .name="kRDC_Mem_MRC12_3"},
    {.id=kRDC_Mem_MRC13_0, .name="kRDC_Mem_MRC13_0"},
    {.id=kRDC_Mem_MRC13_1, .name="kRDC_Mem_MRC13_1"},
    {.id=kRDC_Mem_MRC13_2, .name="kRDC_Mem_MRC13_2"},
    {.id=kRDC_Mem_MRC13_3, .name="kRDC_Mem_MRC13_3"},
    {.id=kRDC_Mem_MRC13_4, .name="kRDC_Mem_MRC13_4"},
};

static rdc_profile_t RDC_Periph_profile[] = {
    {.id=kRDC_Periph_GPIO1, .name="kRDC_Periph_GPIO1"},
    {.id=kRDC_Periph_GPIO2, .name="kRDC_Periph_GPIO2"},
    {.id=kRDC_Periph_GPIO3, .name="kRDC_Periph_GPIO3"},
    {.id=kRDC_Periph_GPIO4, .name="kRDC_Periph_GPIO4"},
    {.id=kRDC_Periph_GPIO5, .name="kRDC_Periph_GPIO5"},
    {.id=kRDC_Periph_MU2_A, .name="kRDC_Periph_MU2_A"},
    {.id=kRDC_Periph_ANA_TSENSOR, .name="kRDC_Periph_ANA_TSENSOR"},
    {.id=kRDC_Periph_ANA_OSC, .name="kRDC_Periph_ANA_OSC"},
    {.id=kRDC_Periph_WDOG1, .name="kRDC_Periph_WDOG1"},
    {.id=kRDC_Periph_WDOG2, .name="kRDC_Periph_WDOG2"},
    {.id=kRDC_Periph_WDOG3, .name="kRDC_Periph_WDOG3"},
    {.id=kRDC_Periph_OCRAM_MECC, .name="kRDC_Periph_OCRAM_MECC"},
    {.id=kRDC_Periph_OCRAM_S_MECC, .name="kRDC_Periph_OCRAM_S_MECC"},
    {.id=kRDC_Periph_GPT1, .name="kRDC_Periph_GPT1"},
    {.id=kRDC_Periph_GPT2, .name="kRDC_Periph_GPT2"},
    {.id=kRDC_Periph_GPT3, .name="kRDC_Periph_GPT3"},
    {.id=kRDC_Periph_MU2_B, .name="kRDC_Periph_MU2_B"},
    {.id=kRDC_Periph_ROMCP, .name="kRDC_Periph_ROMCP"},
    {.id=kRDC_Periph_MU3_A, .name="kRDC_Periph_MU3_A"},
    {.id=kRDC_Periph_IOMUXC, .name="kRDC_Periph_IOMUXC"},
    {.id=kRDC_Periph_IOMUXC_GPR, .name="kRDC_Periph_IOMUXC_GPR"},
    {.id=kRDC_Periph_OCOTP_CTRL, .name="kRDC_Periph_OCOTP_CTRL"},
    {.id=kRDC_Periph_ANA_PLL, .name="kRDC_Periph_ANA_PLL"},
    {.id=kRDC_Periph_SNVS_HP, .name="kRDC_Periph_SNVS_HP"},
    {.id=kRDC_Periph_CCM, .name="kRDC_Periph_CCM"},
    {.id=kRDC_Periph_SRC, .name="kRDC_Periph_SRC"},
    {.id=kRDC_Periph_GPC, .name="kRDC_Periph_GPC"},
    {.id=kRDC_Periph_SEMAPHORE1, .name="kRDC_Periph_SEMAPHORE1"},
    {.id=kRDC_Periph_SEMAPHORE2, .name="kRDC_Periph_SEMAPHORE2"},
    {.id=kRDC_Periph_RDC, .name="kRDC_Periph_RDC"},
    {.id=kRDC_Periph_CSU, .name="kRDC_Periph_CSU"},
    {.id=kRDC_Periph_MU3_B, .name="kRDC_Periph_MU3_B"},
    {.id=kRDC_Periph_ISI, .name="kRDC_Periph_ISI"},
    {.id=kRDC_Periph_ISP0, .name="kRDC_Periph_ISP0"},
    {.id=kRDC_Periph_ISP1, .name="kRDC_Periph_ISP1"},
    {.id=kRDC_Periph_IPS_DEWARP, .name="kRDC_Periph_IPS_DEWARP"},
    {.id=kRDC_Periph_MIPI_CSI0, .name="kRDC_Periph_MIPI_CSI0"},
    {.id=kRDC_Periph_HSIOMIX_BLK_CTL, .name="kRDC_Periph_HSIOMIX_BLK_CTL"},
    {.id=kRDC_Periph_PWM1, .name="kRDC_Periph_PWM1"},
    {.id=kRDC_Periph_PWM2, .name="kRDC_Periph_PWM2"},
    {.id=kRDC_Periph_PWM3, .name="kRDC_Periph_PWM3"},
    {.id=kRDC_Periph_PWM4, .name="kRDC_Periph_PWM4"},
    {.id=kRDC_Periph_SYS_COUNTER_RD, .name="kRDC_Periph_SYS_COUNTER_RD"},
    {.id=kRDC_Periph_SYS_COUNTER_CMP, .name="kRDC_Periph_SYS_COUNTER_CMP"},
    {.id=kRDC_Periph_SYS_COUNTER_CTRL, .name="kRDC_Periph_SYS_COUNTER_CTRL"},
    {.id=kRDC_Periph_I2C5, .name="kRDC_Periph_I2C5"},
    {.id=kRDC_Periph_GPT6, .name="kRDC_Periph_GPT6"},
    {.id=kRDC_Periph_GPT5, .name="kRDC_Periph_GPT5"},
    {.id=kRDC_Periph_GPT4, .name="kRDC_Periph_GPT4"},
    {.id=kRDC_Periph_MIPI_CSI1, .name="kRDC_Periph_MIPI_CSI1"},
    {.id=kRDC_Periph_MIPI_DSI0, .name="kRDC_Periph_MIPI_DSI0"},
    {.id=kRDC_Periph_MEDIAMIX_BLK_CTL, .name="kRDC_Periph_MEDIAMIX_BLK_CTL"},
    {.id=kRDC_Periph_LCDIF1, .name="kRDC_Periph_LCDIF1"},
    {.id=kRDC_Periph_EDMA, .name="kRDC_Periph_EDMA"},
    {.id=kRDC_Periph_EDMA_0_15, .name="kRDC_Periph_EDMA_0_15"},
    {.id=kRDC_Periph_EDMA_16_31, .name="kRDC_Periph_EDMA_16_31"},
    {.id=kRDC_Periph_TZASC, .name="kRDC_Periph_TZASC"},
    {.id=kRDC_Periph_I2C6, .name="kRDC_Periph_I2C6"},
    {.id=kRDC_Periph_CAAM, .name="kRDC_Periph_CAAM"},
    {.id=kRDC_Periph_LCDIF2, .name="kRDC_Periph_LCDIF2"},
    {.id=kRDC_Periph_PERFMON1, .name="kRDC_Periph_PERFMON1"},
    {.id=kRDC_Periph_PERFMON2, .name="kRDC_Periph_PERFMON2"},
    {.id=kRDC_Periph_PLATFORM_CTRL, .name="kRDC_Periph_PLATFORM_CTRL"},
    {.id=kRDC_Periph_QOSC, .name="kRDC_Periph_QOSC"},
    {.id=kRDC_Periph_LVDS0, .name="kRDC_Periph_LVDS0"},
    {.id=kRDC_Periph_LVDS1, .name="kRDC_Periph_LVDS1"},
    {.id=kRDC_Periph_I2C1, .name="kRDC_Periph_I2C1"},
    {.id=kRDC_Periph_I2C2, .name="kRDC_Periph_I2C2"},
    {.id=kRDC_Periph_I2C3, .name="kRDC_Periph_I2C3"},
    {.id=kRDC_Periph_I2C4, .name="kRDC_Periph_I2C4"},
    {.id=kRDC_Periph_UART4, .name="kRDC_Periph_UART4"},
    {.id=kRDC_Periph_HDMI_TX, .name="kRDC_Periph_HDMI_TX"},
    {.id=kRDC_Periph_IRQ_STEER, .name="kRDC_Periph_IRQ_STEER"},
    {.id=kRDC_Periph_SDMA2, .name="kRDC_Periph_SDMA2"},
    {.id=kRDC_Periph_MU1_A, .name="kRDC_Periph_MU1_A"},
    {.id=kRDC_Periph_MU1_B, .name="kRDC_Periph_MU1_B"},
    {.id=kRDC_Periph_SEMAPHORE_HS, .name="kRDC_Periph_SEMAPHORE_HS"},
    {.id=kRDC_Periph_SAI1, .name="kRDC_Periph_SAI1"},
    {.id=kRDC_Periph_SAI2, .name="kRDC_Periph_SAI2"},
    {.id=kRDC_Periph_SAI3, .name="kRDC_Periph_SAI3"},
    {.id=kRDC_Periph_CAN_FD1, .name="kRDC_Periph_CAN_FD1"},
    {.id=kRDC_Periph_SAI5, .name="kRDC_Periph_SAI5"},
    {.id=kRDC_Periph_SAI6, .name="kRDC_Periph_SAI6"},
    {.id=kRDC_Periph_USDHC1, .name="kRDC_Periph_USDHC1"},
    {.id=kRDC_Periph_USDHC2, .name="kRDC_Periph_USDHC2"},
    {.id=kRDC_Periph_USDHC3, .name="kRDC_Periph_USDHC3"},
    {.id=kRDC_Periph_PCIE_PHY1, .name="kRDC_Periph_PCIE_PHY1"},
    {.id=kRDC_Periph_HDMI_TX_AUDLNK_MSTR, .name="kRDC_Periph_HDMI_TX_AUDLNK_MSTR"},
    {.id=kRDC_Periph_CAN_FD2, .name="kRDC_Periph_CAN_FD2"},
    {.id=kRDC_Periph_SPBA2, .name="kRDC_Periph_SPBA2"},
    {.id=kRDC_Periph_QSPI, .name="kRDC_Periph_QSPI"},
    {.id=kRDC_Periph_AUDIO_BLK_CTRL, .name="kRDC_Periph_AUDIO_BLK_CTRL"},
    {.id=kRDC_Periph_SDMA1, .name="kRDC_Periph_SDMA1"},
    {.id=kRDC_Periph_ENET1, .name="kRDC_Periph_ENET1"},
    {.id=kRDC_Periph_ENET2_TSN, .name="kRDC_Periph_ENET2_TSN"},
    {.id=kRDC_Periph_SPDIF1, .name="kRDC_Periph_SPDIF1"},
    {.id=kRDC_Periph_ECSPI1, .name="kRDC_Periph_ECSPI1"},
    {.id=kRDC_Periph_ECSPI2, .name="kRDC_Periph_ECSPI2"},
    {.id=kRDC_Periph_ECSPI3, .name="kRDC_Periph_ECSPI3"},
    {.id=kRDC_Periph_SAI7, .name="kRDC_Periph_SAI7"},
    {.id=kRDC_Periph_UART1, .name="kRDC_Periph_UART1"},
    {.id=kRDC_Periph_UART3, .name="kRDC_Periph_UART3"},
    {.id=kRDC_Periph_UART2, .name="kRDC_Periph_UART2"},
    {.id=kRDC_Periph_PDM, .name="kRDC_Periph_PDM"},
    {.id=kRDC_Periph_AUDIO_XCVR_RX, .name="kRDC_Periph_AUDIO_XCVR_RX"},
    {.id=kRDC_Periph_SDMA3, .name="kRDC_Periph_SDMA3"},
    {.id=kRDC_Periph_SPBA1, .name="kRDC_Periph_SPBA1"},
};



#define RDC_DBG_ARRAY_SIZE(a) sizeof(a)/sizeof(rdc_profile_t)

const char *RDC_AccessPolicyStr[4] = {
    "RDC_NA",
    "RDC_WO",
    "RDC_RO",
    "RDC_RW"
};

void RDC_DBG_PrintAllMasterAccessPolicy(void) {
    int i;
    uint8_t domainId                   = 0U;
    uint8_t RDC_AccessStatus = 0;
    domainId = RDC_GetCurrentMasterDomainId(RDC);
    PRINTF("Current Domain is [%d]\r\n", domainId);
    for(i=0; i<RDC_DBG_ARRAY_SIZE(RDC_Master_profile); i++) {
        RDC_AccessStatus = RDC_GetPeriphAccessPolicy (RDC, RDC_Master_profile[i].id, domainId);
        PRINTF("[%d] %s: %s\r\n", i, RDC_Master_profile[i].name, RDC_AccessPolicyStr[RDC_AccessStatus]);
    }
}

void RDC_DBG_PrintAllMemAccessPolicy(void) {
    int i;
    uint8_t domainId                   = 0U;
    uint8_t RDC_AccessStatus = 0;
    domainId = RDC_GetCurrentMasterDomainId(RDC);
    PRINTF("Current Domain is [%d]\r\n", domainId);
    for(i=0; i<RDC_DBG_ARRAY_SIZE(RDC_Mem_profile); i++) {
        RDC_AccessStatus = RDC_GetMemAccessPolicy (RDC, RDC_Mem_profile[i].id, domainId);
        PRINTF("[%d] %s: %s\r\n", i, RDC_Mem_profile[i].name, RDC_AccessPolicyStr[RDC_AccessStatus]);
    }
}

void RDC_DBG_PrintAllPeriphAccessPolicy(void) {
    int i;
    uint8_t domainId                   = 0U;
    uint8_t RDC_AccessStatus = 0;
    domainId = RDC_GetCurrentMasterDomainId(RDC);
    PRINTF("Current Domain is [%d]\r\n", domainId);
    for(i=0; i<RDC_DBG_ARRAY_SIZE(RDC_Periph_profile); i++) {
        RDC_AccessStatus = RDC_GetPeriphAccessPolicy (RDC, RDC_Periph_profile[i].id, domainId);
        PRINTF("[%d] %s: %s\r\n", i, RDC_Periph_profile[i].name, RDC_AccessPolicyStr[RDC_AccessStatus]);
    }
}








