/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "peripheral.h"
#include <bluetooth/services/sps.h>
#include <bluetooth/services/dis.h>
#include <string.h>
#include <pm/policy.h>

BLE_CONN_PARAM_DEFINE(perip_conn);

static char attDeviceName[] = CONFIG_BT_DEVICE_NAME;

const struct ble_data ad[] = {
    BLE_DATA_BYTES(GAP_ADTYPE_FLAGS,
                   (GAP_ADTYPE_FLAGS_GENERAL |
                    GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED)),

    BLE_DATA_BYTES(GAP_ADTYPE_16BIT_MORE,
                   LO_UINT16(SIMPLEPROFILE_SERV_UUID),
                   HI_UINT16(SIMPLEPROFILE_SERV_UUID)),
};

const struct ble_data sd[] = {
    BLE_DATA_STRING(GAP_ADTYPE_LOCAL_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME),
};

tmosTaskID perip_taskid = TASK_NO_TASK;

static void perip_state_notify(gapRole_States_t newState, gapRoleEvent_t *pEvent);
static void perip_process_msg(tmos_event_hdr_t *pMsg);
static void sps_ccc_changed(uint16_t config);
static void sps_value_changed(uint8_t charc, uint8_t *value, uint16_t len);
static void perip_bond_passcode(uint8_t *deviceAddr, uint16_t connectionHandle,
                                uint8_t uiInputs, uint8_t uiOutputs);
static void perip_pair_state(uint16_t conn_handle, uint8_t state, uint8_t status);

static gapRolesCBs_t perip_role_cb = {
    .pfnStateChange = perip_state_notify,
};

static gapBondCBs_t perip_bond_cb = {
    .pairStateCB = perip_pair_state,
    .passcodeCB = perip_bond_passcode
};

static sps_cb_t sps = {
    .sps_ccc_changed = sps_ccc_changed,
    .sps_value_changed = sps_value_changed,
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

static void sps_ccc_changed(uint16_t config)
{
    printk("sps CCC %s\n", config ? "enabled" : "disabled");

    if (config) {
        tmos_start_reload_task(perip_taskid, PERIP_POLL_NOTIFY_EVENT, 
                MS1_TO_SYSTEM_TIME(1000));
    } else {
        tmos_stop_task(perip_taskid, PERIP_POLL_NOTIFY_EVENT);
    }
}

static void sps_value_changed(uint8_t charc, uint8_t *value, uint16_t len)
{
    printk("char %d: ", charc);

    for (int i = 0; i < len; i++) {
        printk("%#x ", value[i]);
    }
    printk("\n");
}

#define TEST_POLL_EVENT     BIT(4)

tmosEvents perip_process_event(tmosTaskID taskID, tmosEvents event)
{
    if (event & SYS_EVENT_MSG) {
        uint8_t *pMsg;

        if ((pMsg = tmos_msg_receive(perip_taskid)) != NULL) {
            perip_process_msg((tmos_event_hdr_t *)pMsg);
            // Release the TMOS message
            tmos_msg_deallocate(pMsg);
        }
        return event ^ SYS_EVENT_MSG;
    }

    if (event & PRRIP_START_DEVICE) {
        int ret = GAPRole_PeripheralStartDevice(perip_taskid, 
                            &perip_bond_cb, &perip_role_cb);

        __ASSERT(ret == 0, "ret = %d\n", ret);

        return event ^ PRRIP_START_DEVICE;
    }

    if (event & PERIP_POLL_NOTIFY_EVENT) {
        static uint8_t temp = 0;
        
        printk("sps notify: ");

        if(sps_notify(perip_conn.connHandle, &temp, sizeof(temp)) == SUCCESS) {
            printk("%d\n", temp);
            
            temp++;
        } else {
            printk("ERROR\n");
        }

        return event ^ PERIP_POLL_NOTIFY_EVENT;
    }

    if (event & TEST_POLL_EVENT) {
        printk("*\n");
        static bool flag = 0;
        if (!flag) {
            flag = 1;
            pm_policy_state_lock_get(PM_STATE_SUSPEND_TO_RAM);
        } else {
            flag = 0;
            pm_policy_state_lock_put(PM_STATE_SUSPEND_TO_RAM);
        }

        return event ^ TEST_POLL_EVENT;
    }

    return 0;
}

void perip_process_init(void)
{
    perip_taskid = TMOS_ProcessEventRegister(perip_process_event);

    ble_adv_enable(BLE_ADV_CONN, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
    GGS_SetParameter(GGS_DEVICE_NAME_ATT, strlen(attDeviceName), attDeviceName);

    // Setup the GAP Bond Manager
    {
        uint8_t pairMode = GAPBOND_PAIRING_MODE_WAIT_FOR_REQ;
        uint8_t mitm = true;
        uint8_t bonding = false;
        uint8_t ioCap = GAPBOND_IO_CAP_DISPLAY_ONLY;
        GAPBondMgr_SetParameter(GAPBOND_PERI_PAIRING_MODE, sizeof(uint8_t), &pairMode);
        GAPBondMgr_SetParameter(GAPBOND_PERI_MITM_PROTECTION, sizeof(uint8_t), &mitm);
        GAPBondMgr_SetParameter(GAPBOND_PERI_IO_CAPABILITIES, sizeof(uint8_t), &ioCap);
        GAPBondMgr_SetParameter(GAPBOND_PERI_BONDING_ENABLED, sizeof(uint8_t), &bonding);
    }

    GGS_AddService(GATT_ALL_SERVICES);           // GAP
    GATTServApp_AddService(GATT_ALL_SERVICES);   // GATT attributes
    device_info_service_register();
    simple_service_register(&sps);

    uint8_t charc1[SIMPLEPROFILE_CHAR1_LEN] = {1};
    uint8_t charc2[SIMPLEPROFILE_CHAR2_LEN] = {2};
    uint8_t charc3[SIMPLEPROFILE_CHAR3_LEN] = {3};
    uint8_t charc4[SIMPLEPROFILE_CHAR4_LEN] = {4};
    uint8_t charc5[SIMPLEPROFILE_CHAR5_LEN] = {1, 2, 3, 4, 5};
    sps_set_param(SIMPLEPROFILE_CHAR1, charc1, sizeof(charc1));
    sps_set_param(SIMPLEPROFILE_CHAR2, charc2, sizeof(charc2));
    sps_set_param(SIMPLEPROFILE_CHAR3, charc3, sizeof(charc3));
    sps_set_param(SIMPLEPROFILE_CHAR4, charc4, sizeof(charc4));
    sps_set_param(SIMPLEPROFILE_CHAR5, charc5, sizeof(charc5));

    tmos_set_event(perip_taskid, PRRIP_START_DEVICE);

    tmos_start_reload_task(perip_taskid, TEST_POLL_EVENT, MS1_TO_SYSTEM_TIME(1000));
}

static void perip_process_msg(tmos_event_hdr_t *pMsg)
{
    switch (pMsg->event) {
    case GAP_MSG_EVENT:
        break;

    case GATT_MSG_EVENT: {
        gattMsgEvent_t *pMsgEvent;

        pMsgEvent = (gattMsgEvent_t *)pMsg;
        if (pMsgEvent->method == ATT_MTU_UPDATED_EVENT) {
            printk("mtu exchange: %d\n", pMsgEvent->msg.exchangeMTUReq.clientRxMTU);
        }
        break;
    }

    default:
        break;
    }
}

static void perip_link_established(gapRoleEvent_t *pEvent)
{
    gapEstLinkReqEvent_t *event = (gapEstLinkReqEvent_t *)pEvent;

    // See if already connected
    if (perip_conn.connHandle != GAP_CONNHANDLE_INIT) {
        GAPRole_TerminateLink(pEvent->linkCmpl.connectionHandle);
        printk("Connection max...\n");
    } else {
        perip_conn.connHandle = event->connectionHandle;
        perip_conn.connInterval = event->connInterval;
        perip_conn.connSlaveLatency = event->connLatency;
        perip_conn.connTimeout = event->connTimeout;

        printk("Conn %#x - (%#x, %#x, %#x)\n", event->connectionHandle, 
                        event->connInterval,
                        event->connLatency, 
                        event->connTimeout);
    }
}

static void perip_link_terminated(gapRoleEvent_t *pEvent)
{
    gapTerminateLinkEvent_t *event = (gapTerminateLinkEvent_t *)pEvent;

    if (event->connectionHandle == perip_conn.connHandle) {
        BLE_CONN_PARAM_RESET(perip_conn);
        tmos_stop_task(perip_taskid, PERIP_POLL_NOTIFY_EVENT);

        ble_adv_restart();
    } else {
        printk("ERR..\n");
    }
}

static void perip_state_notify(gapRole_States_t newState, gapRoleEvent_t *pEvent)
{
    switch (newState) {
    case GAPROLE_STARTED:
        printk("Initialized..\n");
        break;

    case GAPROLE_ADVERTISING:
        if (pEvent->gap.opcode == GAP_LINK_TERMINATED_EVENT) {
            perip_link_terminated(pEvent);
            printk("Disconnected.. Reason: %#x\n", pEvent->linkTerminate.reason);
            printk("Advertising..\n");
        } else if (pEvent->gap.opcode == GAP_MAKE_DISCOVERABLE_DONE_EVENT) {
            printk("Advertising..\n");
        }
        break;

    case GAPROLE_CONNECTED:
        if (pEvent->gap.opcode == GAP_LINK_ESTABLISHED_EVENT) {
            perip_link_established(pEvent);
            printk("Connected..\n");
        }
        break;

    case GAPROLE_CONNECTED_ADV:
        if (pEvent->gap.opcode == GAP_MAKE_DISCOVERABLE_DONE_EVENT) {
            printk("Connected Advertising..\n");
        }
        break;

    case GAPROLE_WAITING:
        if (pEvent->gap.opcode == GAP_END_DISCOVERABLE_DONE_EVENT) {
            printk("Waiting for advertising..\n");
        } else if (pEvent->gap.opcode == GAP_LINK_TERMINATED_EVENT) {
            perip_link_terminated(pEvent);
            printk("Disconnected.. Reason: %#x\n", pEvent->linkTerminate.reason);
        } else if (pEvent->gap.opcode == GAP_LINK_ESTABLISHED_EVENT) {
            if (pEvent->gap.hdr.status != SUCCESS) {
                printk("Waiting for advertising..\n");
            } else {
                printk("Error..\n");
            }
        } else {
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
