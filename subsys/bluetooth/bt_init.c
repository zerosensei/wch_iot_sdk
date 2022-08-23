/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <bluetooth/bluetooth.h>
#include <logging/log.h>
#include <pm/pm.h>
#include <string.h>

__attribute__((aligned(4))) uint8_t _bt_heap[CONFIG_BT_STACK_SIZE];

uint32_t bt_flash_read_cb(uint32_t addr, uint32_t num, uint32_t *pBuf)
{
    //TODO: FLASH read API

    return 0;
}

uint32_t bt_flash_write_cb(uint32_t addr, uint32_t num, uint32_t *pBuf)
{
    //TODO: FLASH write API

    return 0;
}

void bt_init(void)
{
    bleConfig_t cfg;

    if(tmos_memcmp(VER_LIB, VER_FILE, strlen(VER_FILE)) == FALSE) {
        LOG_ERR("head file error...\n");
        while(1);
    }

    tmos_memset(&cfg, 0, sizeof(bleConfig_t));
    cfg.MEMAddr = (uint32_t)_bt_heap;
    cfg.MEMLen = CONFIG_BT_STACK_SIZE;
    cfg.BufMaxLen = (uint32_t)CONFIG_BT_L2CAP_TX_SIZE;
    cfg.BufNumber = (uint32_t)CONFIG_BT_L2CAP_BUF_NUM;
    cfg.TxNumEvent = (uint32_t)CONFIG_BT_L2CAP_TX_EVENT_NUM;
    cfg.TxPower = (uint32_t)CONFIG_BT_CTLR_TX_PWR_0;
#if(defined(CONFIG_BT_SNV)) && (CONFIG_BT_SNV_ADDR)
    cfg.SNVAddr = (uint32_t)CONFIG_BT_SNV_ADDR;
    cfg.readFlashCB = bt_flash_read_cb;
    cfg.writeFlashCB = bt_flash_write_cb;
#endif
    cfg.SelRTCClock = 1;
    cfg.ConnectNumber = (CONFIG_BT_PERIPHERAL_MAX_CONNECTION & 3) | 
                    (CONFIG_BT_CENTRAL_MAX_CONNECTION << 2);
    cfg.srandCB = k_cycle_get_32; //TODO: random cb
#if(defined CONFIG_BT_TEM_SAMPLE)
    cfg.tsCB = HAL_GetInterTempValue; //TODO: temp
#if(defined CONFIG_SOC_LSI_32000) || (defined CONFIG_SOC_LSI_32768)
    cfg.rcCB = hal_clk_lsi_calibrate; // 内部32K时钟校准
  #endif
#endif
#if defined(CONFIG_PM)
    // cfg.WakeUpTime = WAKE_UP_RTC_MAX_TIME;
    cfg.sleepCB = pm_system_suspend; // 启用睡眠
#endif
#if(defined(CONFIG_BT_CUSTOM_ADDRESS))
    for(int i = 0; i < 6; i++) {
        cfg.MacAddr[i] = MacAddr[5 - i];
    }
#else
    GetMACAddress(cfg.MacAddr);
#endif

    __ASSERT(cfg.MEMAddr, "MEMAddr invalid");
    __ASSERT_NO_MSG(cfg.MEMLen >= 4 * 1024);

    if(!cfg.MEMAddr || cfg.MEMLen < 4 * 1024) {
        while(1);
    }

    bStatus_t ret = BLE_LibInit(&cfg);

    __ASSERT(ret == 0, "BLE lib init error: %#x", ret);

    if(ret) {
        while(1);
    }
}




