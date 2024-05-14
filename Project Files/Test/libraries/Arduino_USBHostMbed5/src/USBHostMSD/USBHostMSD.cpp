/* mbed USBHost Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "USBHostMSD.h"

#if USBHOST_MSD
#include "USBHost/dbg.h"

#define CBW_SIGNATURE   0x43425355
#define CSW_SIGNATURE   0x53425355

#define DEVICE_TO_HOST  0x80
#define HOST_TO_DEVICE  0x00

#define GET_MAX_LUN             (0xFE)
#define BO_MASS_STORAGE_RESET   (0xFF)

void (*mount_fnc)(void) = nullptr;
void (*unmount_fnc)(void) = nullptr;

USBHostMSD::USBHostMSD()
{
    /*  register an object in FAT */

    init_usb();
}

void USBHostMSD::init_usb()
{
    dev_connected = false;
    dev = NULL;
    bulk_in = NULL;
    bulk_out = NULL;
    dev_connected = false;
    blockSize = 0;
    blockCount = 0;
    msd_intf = -1;
    msd_device_found = false;
    disk_init = false;
    dev_connected = false;
    nb_ep = 0;
}


bool USBHostMSD::connected()
{
    return dev_connected;
}

bool USBHostMSD::connect()
{
    if (dev_connected) {
        return true;
    }

    host = USBHost::getHostInst();

    for (uint8_t i = 0; i < MAX_DEVICE_CONNECTED; i++) {
        if ((dev = host->getDevice(i)) != NULL) {

            USB_DBG("Trying to connect MSD device\r\n");

            if(host->enumerate(dev, this))
                break;

            if (msd_device_found) {
                /* As this is done in a specific thread
                 * this lock is taken to avoid to process a disconnection in
                 * usb process during the device registering */
                USBHost::Lock  Lock(host);

                bulk_in = dev->getEndpoint(msd_intf, BULK_ENDPOINT, IN);
                bulk_out = dev->getEndpoint(msd_intf, BULK_ENDPOINT, OUT);

                if (!bulk_in || !bulk_out)
                    continue;

                USB_INFO("New MSD device: VID:%04x PID:%04x [dev: %p - intf: %d]", dev->getVid(), dev->getPid(), dev, msd_intf);
                dev->setName("MSD", msd_intf);
                host->registerDriver(dev, msd_intf, this, &USBHostMSD::init_usb);

                dev_connected = true;

                return true;
            }
        } //if()
    } //for()
    init_usb();
    return false;
}

/*virtual*/ void USBHostMSD::setVidPid(uint16_t vid, uint16_t pid)
{
    // we don't check VID/PID for MSD driver
}

/*virtual*/ bool USBHostMSD::parseInterface(uint8_t intf_nb, uint8_t intf_class, uint8_t intf_subclass, uint8_t intf_protocol) //Must return true if the interface should be parsed
{
    if ((msd_intf == -1) &&
            (intf_class == MSD_CLASS) &&
            (intf_subclass == 0x06) &&
            (intf_protocol == 0x50)) {
        msd_intf = intf_nb;
        return true;
    }
    return false;
}

/*virtual*/ bool USBHostMSD::useEndpoint(uint8_t intf_nb, ENDPOINT_TYPE type, ENDPOINT_DIRECTION dir) //Must return true if the endpoint will be used
{
    if (intf_nb == msd_intf) {
        if (type == BULK_ENDPOINT) {
            nb_ep++;
            if (nb_ep == 2)
                msd_device_found = true;
            return true;
        }
    }
    return false;
}


int USBHostMSD::testUnitReady()
{
    USB_DBG("Test unit ready");
    return SCSITransfer(NULL, 6, DEVICE_TO_HOST, 0, 0);
}


int USBHostMSD::readCapacity()
{
    USB_DBG("Read capacity");
    uint8_t cmd[10] = {0x25,0,0,0,0,0,0,0,0,0};
    uint8_t result[8];
    int status = SCSITransfer(cmd, 10, DEVICE_TO_HOST, result, 8);
    if (status == 0) {
        blockCount = (result[0] << 24) | (result[1] << 16) | (result[2] << 8) | result[3];
        blockSize = (result[4] << 24) | (result[5] << 16) | (result[6] << 8) | result[7];
        USB_INFO("MSD [dev: %p] - blockCount: %u, blockSize: %d, Capacity: %d\r\n", dev, blockCount, blockSize, blockCount*blockSize);
    }
    return status;
}


int USBHostMSD::SCSIRequestSense()
{
    USB_DBG("Request sense");
    uint8_t cmd[6] = {0x03,0,0,0,18,0};
    uint8_t result[18];
    int status = SCSITransfer(cmd, 6, DEVICE_TO_HOST, result, 18);
    return status;
}


int USBHostMSD::inquiry(uint8_t lun, uint8_t page_code)
{
    USB_DBG("Inquiry");
    uint8_t evpd = (page_code == 0) ? 0 : 1;
    uint8_t cmd[6] = {0x12, uint8_t((lun << 5) | evpd), page_code, 0, 36, 0};
    uint8_t result[36];
    int status = SCSITransfer(cmd, 6, DEVICE_TO_HOST, result, 36);
    if (status == 0) {
        char vid_pid[17];
        memcpy(vid_pid, &result[8], 8);
        vid_pid[8] = 0;
        USB_INFO("MSD [dev: %p] - Vendor ID: %s", dev, vid_pid);

        memcpy(vid_pid, &result[16], 16);
        vid_pid[16] = 0;
        USB_INFO("MSD [dev: %p] - Product ID: %s", dev, vid_pid);

        memcpy(vid_pid, &result[32], 4);
        vid_pid[4] = 0;
        USB_INFO("MSD [dev: %p] - Product rev: %s", dev, vid_pid);
    }
    return status;
}

int USBHostMSD::checkResult(uint8_t res, USBEndpoint * ep)
{
    // Guard against host not being initialized
    if (nullptr == host)
    {
        return -1;
    }

    // Notice that USB_TYPE_STALL_ERROR is never actually set anywhere in the library for the ST target, because
    // URB_STALL is never handled directly. Otherwise, this would be the code to unstall the EP.
    if (res == USB_TYPE_STALL_ERROR) {
        res = host->controlWrite(   dev,
                                    USB_RECIPIENT_ENDPOINT | USB_HOST_TO_DEVICE | USB_REQUEST_TYPE_STANDARD,
                                    CLEAR_FEATURE,
                                    0, ep->getAddress(), NULL, 0);
        // set state to IDLE if clear feature successful
        if (res == USB_TYPE_OK) {
            ep->setState(USB_TYPE_IDLE);
        }
    }

    if (res != USB_TYPE_OK)
        return -1;

    return 0;
}


int USBHostMSD::SCSITransfer(uint8_t * cmd, uint8_t cmd_len, int flags, uint8_t * data, uint32_t transfer_len)
{

    int res = 0;

    // Guard against host not being initialized
    if (nullptr == host)
    {
        return -1;
    }

    cbw.Signature = CBW_SIGNATURE;
    cbw.Tag = 0;
    cbw.DataLength = transfer_len;
    cbw.Flags = flags;
    cbw.LUN = 0;
    cbw.CBLength = cmd_len;
    memset(cbw.CB,0,sizeof(cbw.CB));
    if (cmd) {
        memcpy(cbw.CB,cmd,cmd_len);
    }

    // send the cbw
    USB_DBG("Send CBW");
    res = host->bulkWrite(dev, bulk_out,(uint8_t *)&cbw, 31);
    if (checkResult(res, bulk_out))
        return -1;

    // data stage if needed
    if (data) {
        USB_DBG("data stage");
        if (flags == HOST_TO_DEVICE) {

            res = host->bulkWrite(dev, bulk_out, data, transfer_len);
            if (checkResult(res, bulk_out))
                return -1;

        } else if (flags == DEVICE_TO_HOST) {

            res = host->bulkRead(dev, bulk_in, data, transfer_len);
            if (checkResult(res, bulk_in))
                return -1;
        }
    }

    // status stage
    csw.Signature = 0;
    USB_DBG("Read CSW");
    res = host->bulkRead(dev, bulk_in,(uint8_t *)&csw, 13);
    if (checkResult(res, bulk_in))
        return -1;

    if (csw.Signature != CSW_SIGNATURE) {
        return -1;
    }

    USB_DBG("recv csw: status: %d", csw.Status);

    // ModeSense?
    if ((csw.Status == 1) && (cmd[0] != 0x03)) {
        USB_DBG("request mode sense");
        return SCSIRequestSense();
    }

    // perform reset recovery
    if ((csw.Status == 2) && (cmd[0] != 0x03)) {

        // send Bulk-Only Mass Storage Reset request
        res = host->controlWrite(   dev,
                                    USB_RECIPIENT_INTERFACE | USB_HOST_TO_DEVICE | USB_REQUEST_TYPE_CLASS,
                                    BO_MASS_STORAGE_RESET,
                                    0, msd_intf, NULL, 0);

        // Unstall [sic!] both endpoints.
        // Notice that this is the unstall that is required by the specification as part of clearing a Phase Error,
        // _not_ the unstall for an ordinarily stalled EP!
        res = host->controlWrite(   dev,
                                    USB_RECIPIENT_ENDPOINT | USB_HOST_TO_DEVICE | USB_REQUEST_TYPE_STANDARD,
                                    CLEAR_FEATURE,
                                    0, bulk_in->getAddress(), NULL, 0);

        res = host->controlWrite(   dev,
                                    USB_RECIPIENT_ENDPOINT | USB_HOST_TO_DEVICE | USB_REQUEST_TYPE_STANDARD,
                                    CLEAR_FEATURE,
                                    0, bulk_out->getAddress(), NULL, 0);

    }

    return csw.Status;
}


int USBHostMSD::dataTransfer(uint8_t * buf, uint32_t block, uint8_t nbBlock, int direction)
{
    uint8_t cmd[10];
    memset(cmd,0,10);
    cmd[0] = (direction == DEVICE_TO_HOST) ? 0x28 : 0x2A;

    cmd[2] = (block >> 24) & 0xff;
    cmd[3] = (block >> 16) & 0xff;
    cmd[4] = (block >> 8) & 0xff;
    cmd[5] =  block & 0xff;

    cmd[7] = (nbBlock >> 8) & 0xff;
    cmd[8] = nbBlock & 0xff;

    return SCSITransfer(cmd, 10, direction, buf, blockSize*nbBlock);
}

int USBHostMSD::getMaxLun()
{
    uint8_t buf[1], res;

    // Guard against host not being initialized
    if (nullptr == host)
    {
        return -1;
    }

    res = host->controlRead(    dev, USB_RECIPIENT_INTERFACE | USB_DEVICE_TO_HOST | USB_REQUEST_TYPE_CLASS,
                                0xfe, 0, msd_intf, buf, 1);
    USB_DBG("max lun: %d", buf[0]);
    return res;
}

int USBHostMSD::init()
{
    USB_DBG("FILESYSTEM: init");
    uint16_t i, timeout = 10, ret;
    getMaxLun();
    for (i = 0; i < timeout; i++) {
        rtos::ThisThread::sleep_for(100);
        if (!testUnitReady())
            break;
    }

    if (i == timeout) {
        disk_init = false;
        return -1;
    }

    inquiry(0, 0);
    disk_init = 1;
    return readCapacity();
}

int USBHostMSD::program(const void *buffer, mbed::bd_addr_t addr, mbed::bd_size_t size)
{
    uint32_t block_number, count;
    uint8_t *buf = (uint8_t *)buffer;
    if (!disk_init) {
        init();
    }
    if (!disk_init) {
        return -1;
    }
    block_number =  addr / blockSize;
    count = size /blockSize;

    for (uint32_t b = block_number; b < block_number + count; b++) {
        if (dataTransfer(buf, b, 1, HOST_TO_DEVICE))
            return -1;
        buf += blockSize;
    }
    return 0;
}

int USBHostMSD::read(void *buffer, mbed::bd_addr_t addr, mbed::bd_size_t size)
{
    uint32_t block_number, count;
    uint8_t *buf = (uint8_t *)buffer;
    if (!disk_init) {
        init();
    }
    if (!disk_init) {
        return -1;
    }
    block_number =  addr / blockSize;
    count = size / blockSize;

    for (uint32_t b = block_number; b < block_number + count; b++) {
        if (dataTransfer(buf, b, 1, DEVICE_TO_HOST))
            return -1;
        buf += blockSize;
    }
    return 0;
}

int USBHostMSD::erase(mbed::bd_addr_t addr, mbed::bd_size_t size)
{
    return 0;
}

mbed::bd_size_t USBHostMSD::get_read_size() const
{
    return (disk_init ? (mbed::bd_size_t)blockSize : -1);
}

mbed::bd_size_t USBHostMSD::get_program_size() const
{
    return (disk_init ? (mbed::bd_size_t)blockSize : -1);
}
mbed::bd_size_t USBHostMSD::get_erase_size() const
{
    return (disk_init ? (mbed::bd_size_t)blockSize : -1);
}

mbed::bd_size_t USBHostMSD::size() const
{
    USB_DBG("FILESYSTEM: size ");
    return (disk_init ? (mbed::bd_size_t)blockCount*blockSize : 0);
}

const char *USBHostMSD::get_type() const
{
    return "USBMSD";
}

// The callback will only be called if connect() has been used at least once before,
// because of an older bugfix (commit 72d0aa6), but this is still useful - see the
// implementation in Arduino_POSIXStorage for an example
bool USBHostMSD::attach_detected_callback(void (*cbk)()) {
    mount_fnc = cbk;
    return true;
}

bool USBHostMSD::attach_removed_callback(void (*cbk)()) {
    unmount_fnc = cbk;
    return true;
}

#endif
