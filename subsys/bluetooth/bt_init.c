/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <bluetooth/bluetooth.h>
#include <drivers/eeprom.h>
#include <drivers/timer/system_timer.h>
#include <logging/log.h>
#include <pm/pm.h>
#include <random/rand32.h>
#include <string.h>

LOG_MODULE_REGISTER(ble_init, CONFIG_BT_LOG_LEVEL);

__attribute__((aligned(4))) uint8_t _bt_heap[CONFIG_BT_STACK_SIZE];

extern const struct pm_state_info soc_pm_state;
uint32_t exit_latency_tick;
uint32_t min_residency_tick;

uint32_t ble_flash_read_cb(uint32_t addr, uint32_t num, uint32_t *pBuf)
{
    const struct device *eeprom = DEVICE_GET(eeprom);

    if (!device_is_ready(eeprom)) {
        LOG_ERR("eeprom is not ready\n");
    }

    return eeprom_read(eeprom, addr, pBuf, num);
}

uint32_t ble_flash_write_cb(uint32_t addr, uint32_t num, uint32_t *pBuf)
{
    const struct device *eeprom = DEVICE_GET(eeprom);

    if (!device_is_ready(eeprom)) {
        LOG_ERR("eeprom is not ready\n");
    }

    return eeprom_write(eeprom, addr, pBuf, num);

    return 0;
}

uint32_t ble_idle(uint32_t tick)
{
    uint32_t sleep_tick;
    int key = irq_lock();
    uint32_t curr_tick = k_cycle_get_32();

    irq_unlock(key);

    if (tick >= curr_tick) {
        sleep_tick = tick - curr_tick;
    } else {
        sleep_tick = tick + 
                (CONFIG_SOC_RTC_MAX_TICK - curr_tick);
    }

    if (sleep_tick <= exit_latency_tick + 
                  min_residency_tick) {
        if(sleep_tick < min_residency_tick) {
            return 2;
        }

        sys_clock_set_ticks(tick, true);
        pm_state_set(PM_STATE_RUNTIME_IDLE, 0);

        return 1;
    }

    sys_clock_set_ticks(tick - exit_latency_tick, true);

    pm_state_set(PM_STATE_SUSPEND_TO_RAM, 0);

    sys_clock_set_ticks(tick, true);
    pm_state_set(PM_STATE_SUSPEND_TO_IDLE, 0);

    pm_state_exit_post_ops(PM_STATE_SUSPEND_TO_RAM, 0);

    return 0;
}

void LIBStatusCallback( uint8_t code, uint32_t status )
{
  static uint8_t flag=0;
  int16_t cfo;

  if( code == 9 ){
    if( flag == 1 ){
      printk("tx\trx\t");
    }
    else if( flag == 2 ){
      printk("%d\t%d\t", status>>16, 100-(status&0xFFFF) );
    }
  }
  else if( code == 0x89 ){
//      printk("md timeout:%d ", status );
  }
  else if( code == 10 ){
    if( flag == 1 ){
      printk("crc\t");
    }
    else if( flag == 2 ){
      printk("%d\t",status);
    }
  }
  else if( code == 11 ){
    if( flag == 1 ){
      printk("cfo\t" );
    }
    if( flag == 2 ){
      printk("%d\t",(s16)(status&0xffff) );
    }
    cfo = (s16)(status&0xffff);
    if( cfo > 36 ){
    }
    else if( cfo < -36 ){
    }
  }
  else if( code == 12 ){
    if( flag == 1 ){
      printk("rssi\n" );
      flag = 2;
    }
    else if( flag == 2 ){
      printk("%d\n",(s8)(status&0xff));
    }    else{
      flag = 1;
    }
  }
  else if( code == 13 ){
    printk("aa:%X\n", status );
  }
  else if( code == 14 ){
    printk("crc:%X\n", status );
  }
  else if( code == 16 ){
//    printk("time:%d  ", status );
  }
  else if( code == 17 ){
//    printk("%d ", status );
  }
  else if( code == 18 ){
//    printk("%d\n", status );
  }
  else if( code == 19 ){
//    if(status)printk("length=%d\n", status );
  }
  else printk("lib-code:%x s::%x\n",code,status );
}

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
    cfg.readFlashCB = ble_flash_read_cb;
    cfg.writeFlashCB = ble_flash_write_cb;
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
#if(defined CONFIG_BT_TEM_SAMPLE)
    cfg.tsCB = HAL_GetInterTempValue; //TODO: temp
#if(defined CONFIG_SOC_LSI_32000) || (defined CONFIG_SOC_LSI_32768)
    cfg.rcCB = hal_clk_lsi_calibrate; 
#endif
#endif
    // cfg.staCB = LIBStatusCallback;
#if defined(CONFIG_PM)
    cfg.WakeUpTime = 1;
    // cfg.sleepCB = ble_idle;
    //   exit_latency_tick = soc_pm_state.exit_latency_ticks;
    // min_residency_tick = soc_pm_state.min_residency_ticks;
    cfg.sleepCB = pm_system_suspend;
#endif
#if(defined(CONFIG_BT_CUSTOM_ADDRESS))
    for(int i = 0; i < 6; i++) {
        cfg.MacAddr[i] = MacAddr[5 - i];
    }
#else
    __attribute__((aligned(4))) uint8_t MacAddr[6];

    GetMACAddress(MacAddr);
    tmos_memcpy(cfg.MacAddr, MacAddr, 6);
#endif
    printk("MAC: ");
    for(int i = 0; i < 6; i++) {
        printk("%#x ", MacAddr[i]);
    }
    printk("\n");

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