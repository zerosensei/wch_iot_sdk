/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "peripheral_hr.h"
#include <buf/buf.h>
#include <bluetooth/services/hrs.h>
#include <bluetooth/services/dis.h>
#include <string.h>

BUF_SIMPLE_DEFINE_STATIC(hrs_buf, 10);

BLE_CONN_PARAM_DEFINE(hr_conn);

static char attDeviceName[] = CONFIG_BT_DEVICE_NAME;

const struct ble_data ad[] = {
    BLE_DATA_BYTES(GAP_ADTYPE_FLAGS,
                   (GAP_ADTYPE_FLAGS_GENERAL |
                    GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED)),

    BLE_DATA_BYTES(GAP_ADTYPE_16BIT_MORE,
                   LO_UINT16(HEARTRATE_SERV_UUID),
                   HI_UINT16(HEARTRATE_SERV_UUID)),
};

const struct ble_data sd[] = {
    BLE_DATA_STRING(GAP_ADTYPE_LOCAL_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME),
};

tmosTaskID hr_taskid = TASK_NO_TASK;

static uint16_t heartRateBpm = 70;
static uint16_t heartRateEnergy = 0;
static uint16_t heartRateRrInterval1 = 600;
static uint16_t heartRateRrInterval2 = 600;

static void peripheralStateNotificationCB(gapRole_States_t newState, gapRoleEvent_t *pEvent);

static void Peripheral_ProcessTMOSMsg(tmos_event_hdr_t *pMsg);

static void perip_bond_passcode(uint8_t *deviceAddr, uint16_t connectionHandle,
                                uint8_t uiInputs, uint8_t uiOutputs);
static void perip_pair_state(uint16_t conn_handle, uint8_t state, uint8_t status);

static gapRolesCBs_t Peripheral_PeripheralCBs = {
    .pfnStateChange = peripheralStateNotificationCB,
};

static gapBondCBs_t Peripheral_BondMgrCBs = {
    .pairStateCB = perip_pair_state,
    .passcodeCB = perip_bond_passcode
};

static hrs_cb_t hrs = {
    .hrs_ccc_changed = ,
    .hrs_ctrl_energy_exp = ,
};

static void perip_bond_passcode(uint8_t *deviceAddr, uint16_t connectionHandle,
                                uint8_t uiInputs, uint8_t uiOutputs)
{
    uint32_t passkey = 0;

    if (uiOutputs) {
        passkey = tmos_rand() % 1000000;

        printk("Pass key: %06d\n", passkey);

        GAPBondMgr_PasscodeRsp(connectionHandle, SUCCESS, passkey);
    }
}

static char *pair_state_cause_str(uint8_t state)
{
    switch (state) {
    case GAPBOND_PAIRING_STATE_STARTED:
        return "Pairing started";
    case GAPBOND_PAIRING_STATE_COMPLETE:
        return "Pairing complete";
    case GAPBOND_PAIRING_STATE_BONDED:
        return "Bonded";
    case GAPBOND_PAIRING_STATE_BOND_SAVED:
        return "Bond saved";
    default:
        return "Unknown state";
    }
}

static void perip_pair_state(uint16_t conn_handle, uint8_t state, uint8_t status)
{
    printk("Conn %#x ", conn_handle);
    printk("%s ", pair_state_cause_str(state));
    printk("%s\n", status ? ": ERROR" : ": SUCCESS");
}

static void perip_hr_ccc_changed(uint16_t config)
{
    printk("hrs CCC %s\n", config ? "enabled" : "disabled");

    if (config) {
        tmos_start_reload_task(hr_taskid, HR_POLL_NOTIFY_EVENT, 
                MS1_TO_SYSTEM_TIME(1000));
    } else {
        tmos_stop_task(hr_taskid, HR_POLL_NOTIFY_EVENT);
    }   
}

static void perip_hr_ctrl_energy_exp(void)
{

}


tmosEvents hr_process_event(tmosTaskID taskID, tmosEvents event)
{
    if (event & SYS_EVENT_MSG) {
        uint8_t *pMsg;

        if ((pMsg = tmos_msg_receive(hr_taskid)) != NULL) {
            Peripheral_ProcessTMOSMsg((tmos_event_hdr_t *)pMsg);
            // Release the TMOS message
            tmos_msg_deallocate(pMsg);
        }
        return event ^ SYS_EVENT_MSG;
    }

    if (event & HR_START_DEVICE) {
        int ret = GAPRole_PeripheralStartDevice(hr_taskid, &Peripheral_BondMgrCBs, &Peripheral_PeripheralCBs);

        __ASSERT(ret == 0, "ret = %d\n", ret);
        return event ^ HR_START_DEVICE;
    }

    if (event & HR_POLL_NOTIFY_EVENT) {

        static uint8_t flags = HRS_FLAGS_VAL_FORMAT_UINT16 |
                            HRS_FLAGS_CONTACT_NOT_DET |
                            HRS_FLAGS_ENERGY_EXP |
                            HRS_FLAGS_RR;

        buf_simple_reset(&hrs_buf);
        buf_simple_add_u8(&hrs_buf, flags);

        if (flags & HRS_FLAGS_VAL_FORMAT_UINT16) {
            buf_simple_add_le16(&hrs_buf, heartRateBpm);
        } else {
            buf_simple_add_u8(&hrs_buf, (uint8_t)heartRateBpm);
        }

        if (flags & HRS_FLAGS_ENERGY_EXP) {
            buf_simple_add_le16(&hrs_buf, heartRateEnergy);
        }

        if (flags & HRS_FLAGS_RR) {
            buf_simple_add_le16(&hrs_buf, heartRateRrInterval1);
            buf_simple_add_le16(&hrs_buf, heartRateRrInterval2);
        }

        printk("notify conn %#x: ", hr_conn.connHandle);
        int ret = hrs_meas_notify(hr_conn.connHandle, hrs_buf.data, hrs_buf.len);
        printk("%d\n", ret);

        return event ^ HR_POLL_NOTIFY_EVENT;
    }

    return 0;
}

void hr_process_init(void)
{
    hr_taskid = TMOS_ProcessEventRegister(hr_process_event);

    ble_adv_enable(BLE_ADV_CONN, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
    GGS_SetParameter(GGS_DEVICE_NAME_ATT, strlen(attDeviceName), attDeviceName);

    // Setup the GAP Bond Manager
    {
        uint8_t pairMode = GAPBOND_PAIRING_MODE_WAIT_FOR_REQ;
        uint8_t mitm = true;
        uint8_t bonding = true;
        uint8_t ioCap = GAPBOND_IO_CAP_DISPLAY_ONLY;
        GAPBondMgr_SetParameter(GAPBOND_PERI_PAIRING_MODE, sizeof(uint8_t), &pairMode);
        GAPBondMgr_SetParameter(GAPBOND_PERI_MITM_PROTECTION, sizeof(uint8_t), &mitm);
        GAPBondMgr_SetParameter(GAPBOND_PERI_IO_CAPABILITIES, sizeof(uint8_t), &ioCap);
        GAPBondMgr_SetParameter(GAPBOND_PERI_BONDING_ENABLED, sizeof(uint8_t), &bonding);
    }

    GGS_AddService(GATT_ALL_SERVICES);           // GAP
    GATTServApp_AddService(GATT_ALL_SERVICES);   // GATT attributes
    device_info_service_register();
    heartrate_service_register();

    tmos_set_event(hr_taskid, HR_START_DEVICE);
}

/*********************************************************************
 * @fn      Peripheral_ProcessTMOSMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void Peripheral_ProcessTMOSMsg(tmos_event_hdr_t *pMsg)
{
    switch (pMsg->event)
    {
    case GAP_MSG_EVENT:
    {
        break;
    }

    case GATT_MSG_EVENT:
    {
        gattMsgEvent_t *pMsgEvent;

        pMsgEvent = (gattMsgEvent_t *)pMsg;
        if (pMsgEvent->method == ATT_MTU_UPDATED_EVENT)
        {
            // peripheralMTU = pMsgEvent->msg.exchangeMTUReq.clientRxMTU;
            printk("mtu exchange: %d\n", pMsgEvent->msg.exchangeMTUReq.clientRxMTU);
        }
        break;
    }

    default:
        break;
    }
}

static void Peripheral_LinkEstablished(gapRoleEvent_t *pEvent)
{
    gapEstLinkReqEvent_t *event = (gapEstLinkReqEvent_t *)pEvent;

    // See if already connected
    if (hr_conn.connHandle != GAP_CONNHANDLE_INIT)
    {
        GAPRole_TerminateLink(pEvent->linkCmpl.connectionHandle);
        printk("Connection max...\n");
    }
    else
    {
        hr_conn.connHandle = event->connectionHandle;
        hr_conn.connInterval = event->connInterval;
        hr_conn.connSlaveLatency = event->connLatency;
        hr_conn.connTimeout = event->connTimeout;

        printk("Conn %x - Int %x \n", event->connectionHandle, event->connInterval);
    }
}

static void Peripheral_LinkTerminated(gapRoleEvent_t *pEvent)
{
    gapTerminateLinkEvent_t *event = (gapTerminateLinkEvent_t *)pEvent;

    if (event->connectionHandle == hr_conn.connHandle)
    {
        BLE_CONN_PARAM_RESET(hr_conn);

        tmos_stop_task(hr_taskid, HR_POLL_NOTIFY_EVENT);

        // Restart advertising
        {
            uint8_t advertising_enable = TRUE;
            GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &advertising_enable);
        }
    }
    else
    {
        printk("ERR..\n");
    }
}

static void peripheralStateNotificationCB(gapRole_States_t newState, gapRoleEvent_t *pEvent)
{
    switch (newState)
    {
    case GAPROLE_STARTED:
        printk("Initialized..\n");
        break;

    case GAPROLE_ADVERTISING:
        if (pEvent->gap.opcode == GAP_LINK_TERMINATED_EVENT)
        {
            Peripheral_LinkTerminated(pEvent);
            printk("Disconnected.. Reason:%x\n", pEvent->linkTerminate.reason);
            printk("Advertising..\n");
        }
        else if (pEvent->gap.opcode == GAP_MAKE_DISCOVERABLE_DONE_EVENT)
        {
            printk("Advertising..\n");
        }
        break;

    case GAPROLE_CONNECTED:
        if (pEvent->gap.opcode == GAP_LINK_ESTABLISHED_EVENT)
        {
            Peripheral_LinkEstablished(pEvent);
            printk("Connected..\n");
        }
        break;

    case GAPROLE_CONNECTED_ADV:
        if (pEvent->gap.opcode == GAP_MAKE_DISCOVERABLE_DONE_EVENT)
        {
            printk("Connected Advertising..\n");
        }
        break;

    case GAPROLE_WAITING:
        if (pEvent->gap.opcode == GAP_END_DISCOVERABLE_DONE_EVENT)
        {
            printk("Waiting for advertising..\n");
        }
        else if (pEvent->gap.opcode == GAP_LINK_TERMINATED_EVENT)
        {
            Peripheral_LinkTerminated(pEvent);
            printk("Disconnected.. Reason:%x\n", pEvent->linkTerminate.reason);
        }
        else if (pEvent->gap.opcode == GAP_LINK_ESTABLISHED_EVENT)
        {
            if (pEvent->gap.hdr.status != SUCCESS)
            {
                printk("Waiting for advertising..\n");
            }
            else
            {
                printk("Error..\n");
            }
        }
        else
        {
            printk("Error..%x\n", pEvent->gap.opcode);
        }
        break;

    case GAPROLE_ERROR:
        printk("Error..\n");
        break;

    default:
        break;
    }
}
