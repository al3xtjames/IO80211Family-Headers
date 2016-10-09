/*
 * Copyright (c) 2005 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 *
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 */

/* Partially modified to match Maverick's IO80211Controller vtable. */

#ifndef _IO80211CONTROLLER_H
#define _IO80211CONTROLLER_H

#include <IOKit/network/IOEthernetController.h>

#include <sys/param.h>
#include <net/bpf.h>

#include "apple80211_ioctl.h"

#define AUTH_TIMEOUT			15	// seconds

/*! @enum LinkSpeed.
 @abstract ???.
 @discussion ???.
 @constant LINK_SPEED_80211A 54 Mbps
 @constant LINK_SPEED_80211B 11 Mbps.
 @constant LINK_SPEED_80211G 54 Mbps.
 */
enum {
	LINK_SPEED_80211A	= 54000000ul,		// 54 Mbps
	LINK_SPEED_80211B	= 11000000ul,		// 11 Mbps
	LINK_SPEED_80211G	= 54000000ul,		// 54 Mbps
	LINK_SPEED_80211N	= 300000000ul,		// 300 Mbps (MCS index 15, 400ns GI, 40 MHz channel)
};

enum IO80211CountryCodeOp
{
	kIO80211CountryCodeReset,				// Reset country code to world wide default, and start
    // searching for 802.11d beacon
};
typedef enum IO80211CountryCodeOp IO80211CountryCodeOp;

enum IO80211SystemPowerState
{
	kIO80211SystemPowerStateUnknown,
	kIO80211SystemPowerStateAwake,
	kIO80211SystemPowerStateSleeping,
};
typedef enum IO80211SystemPowerState IO80211SystemPowerState;

enum IO80211FeatureCode
{
	kIO80211Feature80211n = 1,
};
typedef enum IO80211FeatureCode IO80211FeatureCode;

#define IO80211_LOG( _interface, _level, _msg, ... )	do{ if( _interface && ( _interface->debugFlags() & _level ) ) IOLog( _msg, ##__VA_ARGS__ ); }while(0)

class IO80211Interface;
class IO80211VirtualInterface;
class IO80211Scanner;
class IO80211WorkLoop;

/*! @class IO80211Controller
 @abstract Abstract superclass for 80211 controllers.
 @discussion 80211 controller drivers should subclass IO80211Controller, and implement or override the hardware specific methods to create an 80211 driver. An interface object (an IO80211Interface instance) must be instantiated by the driver, through attachInterface(), to connect the controller driver to the data link layer.
 */
class IO80211Controller : public IOEthernetController
{
    OSDeclareAbstractStructors( IO80211Controller )

protected:
    
    virtual IOService *getProvider() const;
    virtual IOWorkLoop *getWorkLoop() const;
    virtual IOOutputQueue *getOutputQueue() const override;
    
    virtual bool createWorkLoop() override;
    
    virtual bool attachInterface(IONetworkInterface ** interface, bool doRegister = true) override;
    
    virtual bool requestPacketTx(void *, unsigned int);
    
    virtual IOReturn getHardwareAddressForInterface(IO80211Interface *interface, IOEthernetAddress *addr);
    
	virtual void inputMonitorPacket(mbuf_t m, uint32_t dlt, void * header, size_t header_len);
    
    virtual void bpfOutputPacket(OSObject*, unsigned int, mbuf_t m);
    
    virtual SInt32 monitorModeSetEnabled(IO80211Interface * interface, bool enabled, UInt32 dlt) {
        return EOPNOTSUPP;
    }
    
    virtual IO80211Interface *getNetworkInterface();
    
    virtual	SInt32 apple80211_ioctl(IO80211Interface *interface, IO80211VirtualInterface *vinterface,
									ifnet_t ifn, unsigned long cmd, void *data);
    
    virtual SInt32 apple80211Request(UInt32 req, int type, IO80211Interface * intf, void * data) = 0;
    
    virtual SInt32 apple80211VirtualRequest(UInt32 req, int type, IO80211VirtualInterface * vintf, void * data) {
        return EOPNOTSUPP;
    }
    
    virtual SInt32 stopDMA() {
        return EOPNOTSUPP;
    }
    
    virtual UInt32 hardwareOutputQueueDepth(IO80211Interface *interface) {
        return 0;
    }
    
    virtual SInt32 performCountryCodeOperation(IO80211Interface * interface, IO80211CountryCodeOp op) {
        return EOPNOTSUPP;
    }
    
    virtual bool useAppleRSNSupplicant(IO80211Interface * interface);
    virtual bool useAppleRSNSupplicant(IO80211VirtualInterface * vinterface);
    
    virtual void dataLinkLayerAttachComplete(IO80211Interface *interface);
    
    virtual SInt32 enableFeature(IO80211FeatureCode feature, void * refcon) {
        return EOPNOTSUPP;
    }
    
    virtual void setVirtualHardwareAddress(IO80211VirtualInterface *vintf, ether_addr *addr);
    
    virtual IOReturn enableVirtualInterface(IO80211VirtualInterface *vintf);
    virtual IOReturn disableVirtualInterface(IO80211VirtualInterface *vintf);
    
    virtual void inputPacket(mbuf_t m);
    
    virtual SInt32 apple80211_ioctl_get(IO80211Interface *interface, IO80211VirtualInterface *vintf,
                                        ifnet_t ifn, void *data);
    virtual SInt32 apple80211_ioctl_set(IO80211Interface *interface, IO80211VirtualInterface *vintf,
                                        ifnet_t ifn, void *data);
    
    virtual IO80211VirtualInterface *createVirtualInterface(ether_addr* addr, unsigned int arg);
    
    virtual bool attachVirtualInterface(IO80211VirtualInterface **interface, ether_addr *addr, unsigned int arg1, bool arg2);
    virtual void detachVirtualInterface(IO80211VirtualInterface *interface, bool arg);

protected:
    IO80211WorkLoop *_myWorkLoop;
    IO80211Interface *_netif;
    
private:
    OSMetaClassDeclareReservedUnused( IO80211Controller,  0);
	OSMetaClassDeclareReservedUnused( IO80211Controller,  1);
    OSMetaClassDeclareReservedUnused( IO80211Controller,  2);
    OSMetaClassDeclareReservedUnused( IO80211Controller,  3);
    OSMetaClassDeclareReservedUnused( IO80211Controller,  4);
    OSMetaClassDeclareReservedUnused( IO80211Controller,  5);
    OSMetaClassDeclareReservedUnused( IO80211Controller,  6);
    OSMetaClassDeclareReservedUnused( IO80211Controller,  7);
    OSMetaClassDeclareReservedUnused( IO80211Controller,  8);
    OSMetaClassDeclareReservedUnused( IO80211Controller,  9);
    OSMetaClassDeclareReservedUnused( IO80211Controller, 10);
    OSMetaClassDeclareReservedUnused( IO80211Controller, 11);
    OSMetaClassDeclareReservedUnused( IO80211Controller, 12);
    OSMetaClassDeclareReservedUnused( IO80211Controller, 13);
    OSMetaClassDeclareReservedUnused( IO80211Controller, 14);
    OSMetaClassDeclareReservedUnused( IO80211Controller, 15);
};

#endif /* !_IO80211CONTROLLER_H */
