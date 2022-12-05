/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <bluetooth/bluetooth.h>
#include <drivers/timer/system_timer.h>
#include <logging/log.h>
#include <pm/pm.h>
#include <random/rand32.h>
#include <string.h>
LOG_MODULE_REGISTER(ble_init, CONFIG_BT_LOG_LEVEL);

__attribute__((aligned(4))) uint8_t _bt_heap[CONFIG_BT_STACK_SIZE];

void ble_init(void)
{
    bleConfig_t cfg;

    if(tmos_memcmp(VER_LIB, VER_FILE, strlen(VER_FILE)) == FALSE) {
        LOG_ERR("head file error...\n");
        while(1);
    }

    tmos_memset(&cfg, 0, sizeof(bleConfig_t));
    cfg.MEMAddr = (uint32_t)_bt_heap;
    cfg.MEMLen = (uint32_t)CONFIG_BT_STACK_SIZE;
    cfg.BufMaxLen = (uint32_t)CONFIG_BT_L2CAP_TX_SIZE;
    cfg.BufNumber = (uint32_t)CONFIG_BT_L2CAP_BUF_NUM;
    cfg.TxNumEvent = (uint32_t)CONFIG_BT_L2CAP_TX_EVENT_NUM;
    cfg.TxPower = (uint32_t)BLE_TX_POWER_DEFAULT;
#if(defined(CONFIG_BT_SNV)) && (CONFIG_BT_SNV_ADDR)
    cfg.SNVAddr = (uint32_t)CONFIG_BT_SNV_ADDR;
    cfg.readFlashCB = ble_flash_read;
    cfg.writeFlashCB = ble_flash_write;
#endif
#if (defined CONFIG_SOC_LSE_32768)
    cfg.SelRTCClock = 0;
#elif (defined CONFIG_SOC_LSI_32000)
    cfg.SelRTCClock = BIT(0);
#else 
    cfg.SelRTCClock = BIT(1);
#endif
    cfg.ConnectNumber = (CONFIG_BT_PERIPHERAL_MAX_CONNECTION & 3) | 
                    (CONFIG_BT_CENTRAL_MAX_CONNECTION << 2);
    cfg.srandCB = sys_rand32_get;
#if(defined CONFIG_BT_TEMP_SAMPLE)
    cfg.tsCB = ble_hal_get_inter_temp;
#if(defined CONFIG_SOC_LSI_32000) || (defined CONFIG_SOC_LSI_32768)
    cfg.rcCB = ble_hal_lsi_calibrate; 
#endif
#endif
#if defined(CONFIG_PM)
    cfg.WakeUpTime = 1;
    // cfg.sleepCB = ble_idle;
    //   exit_latency_tick = soc_pm_state.exit_latency_ticks;
    // min_residency_tick = soc_pm_state.min_residency_ticks;
    cfg.sleepCB = pm_system_suspend;
#endif
    __attribute__((aligned(4))) uint8_t mac_addr[6];

#if(defined(CONFIG_BT_CUSTOM_ADDRESS))
    //TODO: default MAC
    tmos_memcpy(cfg.MacAddr, mac_addr, 6);
#else
    GetMACAddress(mac_addr);
    tmos_memcpy(cfg.MacAddr, mac_addr, 6);
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