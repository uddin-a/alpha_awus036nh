/* 
   This is part of rtl8187 OpenSource driver.
   Copyright (C) Andrea Merello 2004-2005  <andreamrl@tiscali.it> 
   Released under the terms of GPL (General Public Licence)
   
   Parts of this driver are based on the GPL part of the 
   official realtek driver
   
   Parts of this driver are based on the rtl8192 driver skeleton 
   from Patric Schenke & Andres Salomon
   
   Parts of this driver are based on the Intel Pro Wireless 2100 GPL driver
   
   We want to tanks the Authors of those projects and the Ndiswrapper 
   project Authors.
*/

#ifndef R819xU_H
#define R819xU_H

#include <linux/module.h>
#include <linux/kernel.h>
//#include <linux/config.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
//#include <linux/pci.h>
#include <linux/usb.h>
#include <linux/etherdevice.h>
#include <linux/delay.h>
#include <linux/rtnetlink.h>	//for rtnl_lock()
#include <linux/wireless.h>
#include <linux/timer.h>
#include <linux/proc_fs.h>	// Necessary because we use the proc fs
#include <linux/if_arp.h>
#include <linux/random.h>
#include <linux/version.h>
#include <asm/io.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27))
#include <asm/semaphore.h>
#endif
#include "ieee80211.h"

#ifdef RTL8192SU
#include "r8192S_firmware.h"
#else
#include "r819xU_firmware.h"
#endif

//#define RTL8192U
#define RTL819xU_MODULE_NAME "rtl819xU"
//added for HW security, john.0629
#define FALSE 0
#define TRUE 1
#define MAX_KEY_LEN     61
#define KEY_BUF_SIZE    5

#define BIT0            0x00000001 
#define BIT1            0x00000002 
#define BIT2            0x00000004 
#define BIT3            0x00000008 
#define BIT4            0x00000010 
#define BIT5            0x00000020 
#define BIT6            0x00000040 
#define BIT7            0x00000080 
#define BIT8            0x00000100 
#define BIT9            0x00000200 
#define BIT10           0x00000400 
#define BIT11           0x00000800 
#define BIT12           0x00001000 
#define BIT13           0x00002000 
#define BIT14           0x00004000 
#define BIT15           0x00008000 
#define BIT16           0x00010000 
#define BIT17           0x00020000 
#define BIT18           0x00040000 
#define BIT19           0x00080000 
#define BIT20           0x00100000 
#define BIT21           0x00200000 
#define BIT22           0x00400000 
#define BIT23           0x00800000 
#define BIT24           0x01000000 
#define BIT25           0x02000000 
#define BIT26           0x04000000 
#define BIT27           0x08000000 
#define BIT28           0x10000000 
#define BIT29           0x20000000 
#define BIT30           0x40000000 
#define BIT31           0x80000000 

// Rx smooth factor
#define	Rx_Smooth_Factor		20
#if 0 //we need to use RT_TRACE instead DMESG as RT_TRACE will clearly show debug level wb.
#define DMESG(x,a...) printk(KERN_INFO RTL819xU_MODULE_NAME ": " x "\n", ## a)
#define DMESGW(x,a...) printk(KERN_WARNING RTL819xU_MODULE_NAME ": WW:" x "\n", ## a)
#define DMESGE(x,a...) printk(KERN_WARNING RTL819xU_MODULE_NAME ": EE:" x "\n", ## a)
#else
#define DMESG(x,a...)
#define DMESGW(x,a...)
#define DMESGE(x,a...)
extern u32 rt_global_debug_component;
#define RT_TRACE(component, x, args...) \
do { if(rt_global_debug_component & component) \
	printk(KERN_DEBUG RTL819xU_MODULE_NAME ":" x "\n" , \
	       ##args);\
}while(0);
//----------------------------------------------------------------------
//// Get 8192SU  Rx descriptor. Added by Roger, 2008.04.15.
////----------------------------------------------------------------------
#define RX_DESC_SIZE 24
#define RX_DRV_INFO_SIZE_UNIT   8

#define IS_UNDER_11N_AES_MODE(_ieee)  ((_ieee->pHTInfo->bCurrentHTSupport==TRUE) &&\
									(_ieee->pairwise_key_type==KEY_TYPE_CCMP))

#define COMP_TRACE				BIT0		// For function call tracing.
#define COMP_DBG				BIT1		// Only for temporary debug message.
#define COMP_INIT				BIT2		// during driver initialization / halt / reset.


#define COMP_RECV				BIT3		// Reveive part data path.
#define COMP_SEND				BIT4		// Send part path.
#define COMP_IO					BIT5		// I/O Related. Added by Annie, 2006-03-02.
#define COMP_POWER				BIT6		// 802.11 Power Save mode or System/Device Power state related.
#define COMP_EPROM				BIT7		// 802.11 link related: join/start BSS, leave BSS.
#define COMP_SWBW				BIT8	// For bandwidth switch.
#define COMP_POWER_TRACKING			BIT9	//FOR 8190 TX POWER TRACKING
#define COMP_TURBO				BIT10	// For Turbo Mode related. By Annie, 2005-10-21.
#define COMP_QOS				BIT11	// For QoS.
#define COMP_RATE				BIT12	// For Rate Adaptive mechanism, 2006.07.02, by rcnjko.
#define COMP_LPS					BIT13	// For Radio Measurement. 
#define COMP_DIG				BIT14	// For DIG, 2006.09.25, by rcnjko.
#define COMP_PHY	 			BIT15	
#define COMP_CH					BIT16	//channel setting debug
#define COMP_TXAGC				BIT17	// For Tx power, 060928, by rcnjko. 
#define COMP_HIPWR				BIT18	// For High Power Mechanism, 060928, by rcnjko. 
#define COMP_HALDM				BIT19	// For HW Dynamic Mechanism, 061010, by rcnjko. 
#define COMP_SEC			        BIT20	// Event handling
#define COMP_LED				BIT21	// For LED.
#define COMP_RF					BIT22	// For RF.
//1!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define COMP_RXDESC				BIT23	// Show Rx desc information for SD3 debug. Added by Annie, 2006-07-15.
//1//1Attention Please!!!<11n or 8190 specific code should be put below this line>
//1!!!!!!!!!!!!!!!!!!!!!!!!!!!

#define COMP_FIRMWARE				BIT24	//for firmware downloading
#define COMP_HT					BIT25	// For 802.11n HT related information. by Emily 2006-8-11
#define COMP_AMSDU				BIT26	// For A-MSDU Debugging

#define COMP_SCAN				BIT27	
#define COMP_CMD				BIT28
#define COMP_DOWN				BIT29  //for rm driver module
#define COMP_RESET				BIT30  //for silent reset
#define COMP_ERR				BIT31 //for error out, always on
#endif

#define RTL819x_DEBUG
#ifdef RTL819x_DEBUG
#define assert(expr) \
        if (!(expr)) {                                  \
                printk( "Assertion failed! %s,%s,%s,line=%d\n", \
                #expr,__FILE__,__FUNCTION__,__LINE__);          \
        }
//wb added to debug out data buf
//if you want print DATA buffer related BA, please set ieee80211_debug_level to DATA|BA
#define RT_DEBUG_DATA(level, data, datalen)      \
        do{ if ((rt_global_debug_component & (level)) == (level))   \
                {       \
                        int i;                                  \
                        u8* pdata = (u8*) data;                 \
                        printk(KERN_DEBUG RTL819xU_MODULE_NAME ": %s()\n", __FUNCTION__);   \
                        for(i=0; i<(int)(datalen); i++)                 \
                        {                                               \
                                printk("%2x ", pdata[i]);               \
                                if ((i+1)%16 == 0) printk("\n");        \
                        }                               \
                        printk("\n");                   \
                }                                       \
        } while (0)  
#else
#define assert(expr) do {} while (0)
#define RT_DEBUG_DATA(level, data, datalen) do {} while(0)
#endif /* RTL8169_DEBUG */

//#ifdef RTL8192SU		
	//2TODO: We should define 8192S firmware related macro settings here!!
	#define RTL819X_DEFAULT_RF_TYPE				RF_1T2R
	#define RTL819X_TOTAL_RF_PATH				2

	//#define Rtl819XFwBootArray					Rtl8192UsbFwBootArray
	//#define Rtl819XFwMainArray					Rtl8192UsbFwMainArray
	//#define Rtl819XFwDataArray					Rtl8192UsbFwDataArray

	#define Rtl819XMACPHY_Array_PG				Rtl8192UsbMACPHY_Array_PG
	#define Rtl819XMACPHY_Array					Rtl8192UsbMACPHY_Array
	#define Rtl819XPHY_REGArray					Rtl8192UsbPHY_REGArray
	#define Rtl819XPHY_REG_1T2RArray				Rtl8192UsbPHY_REG_1T2RArray				
	//#define Rtl819XRadioA_Array					Rtl8192UsbRadioA_Array
	//#define Rtl819XRadioB_Array					Rtl8192UsbRadioB_Array
	#define Rtl819XRadioC_Array					Rtl8192UsbRadioC_Array
	#define Rtl819XRadioD_Array					Rtl8192UsbRadioD_Array	

	//2008.11.06 Add.
	#define Rtl819XFwImageArray					Rtl8192SUFwImgArray
	#define Rtl819XMAC_Array						Rtl8192SUMAC_2T_Array
	#define Rtl819XAGCTAB_Array					Rtl8192SUAGCTAB_Array
	#define Rtl819XPHY_REG_Array					Rtl8192SUPHY_REG_2T2RArray			
	#define Rtl819XPHY_REG_to1T1R_Array			Rtl8192SUPHY_ChangeTo_1T1RArray
	#define Rtl819XPHY_REG_to1T2R_Array			Rtl8192SUPHY_ChangeTo_1T2RArray
	#define Rtl819XPHY_REG_to2T2R_Array			Rtl8192SUPHY_ChangeTo_2T2RArray
	#define Rtl819XPHY_REG_Array_PG				Rtl8192SUPHY_REG_Array_PG
	#define Rtl819XRadioA_Array					Rtl8192SURadioA_1T_Array
	#define Rtl819XRadioB_Array					Rtl8192SURadioB_Array
	#define Rtl819XRadioB_GM_Array				Rtl8192SURadioB_GM_Array
	#define Rtl819XRadioA_to1T_Array				Rtl8192SURadioA_to1T_Array
	#define Rtl819XRadioA_to2T_Array				Rtl8192SURadioA_to2T_Array
//#endif	

//
// Queue Select Value in TxDesc
//
#define QSLT_BK                                 0x1
#define QSLT_BE                                 0x0
#define QSLT_VI                                 0x4
#define QSLT_VO                                 0x6
#define QSLT_BEACON                             0x10
#define QSLT_HIGH                               0x11
#define QSLT_MGNT                               0x12
#define QSLT_CMD                                0x13

#define DESC90_RATE1M                           0x00
#define DESC90_RATE2M                           0x01
#define DESC90_RATE5_5M                         0x02
#define DESC90_RATE11M                          0x03
#define DESC90_RATE6M                           0x04
#define DESC90_RATE9M                           0x05
#define DESC90_RATE12M                          0x06
#define DESC90_RATE18M                          0x07
#define DESC90_RATE24M                          0x08
#define DESC90_RATE36M                          0x09
#define DESC90_RATE48M                          0x0a
#define DESC90_RATE54M                          0x0b
#define DESC90_RATEMCS0                         0x00
#define DESC90_RATEMCS1                         0x01
#define DESC90_RATEMCS2                         0x02
#define DESC90_RATEMCS3                         0x03
#define DESC90_RATEMCS4                         0x04
#define DESC90_RATEMCS5                         0x05
#define DESC90_RATEMCS6                         0x06
#define DESC90_RATEMCS7                         0x07
#define DESC90_RATEMCS8                         0x08
#define DESC90_RATEMCS9                         0x09
#define DESC90_RATEMCS10                        0x0a
#define DESC90_RATEMCS11                        0x0b
#define DESC90_RATEMCS12                        0x0c
#define DESC90_RATEMCS13                        0x0d
#define DESC90_RATEMCS14                        0x0e
#define DESC90_RATEMCS15                        0x0f
#define DESC90_RATEMCS32                        0x20

//#ifdef RTL8192SU
// CCK Rates, TxHT = 0
#define DESC92S_RATE1M					0x00
#define DESC92S_RATE2M					0x01
#define DESC92S_RATE5_5M				0x02
#define DESC92S_RATE11M					0x03

// OFDM Rates, TxHT = 0
#define DESC92S_RATE6M					0x04
#define DESC92S_RATE9M					0x05
#define DESC92S_RATE12M					0x06
#define DESC92S_RATE18M					0x07
#define DESC92S_RATE24M					0x08
#define DESC92S_RATE36M					0x09
#define DESC92S_RATE48M					0x0a
#define DESC92S_RATE54M					0x0b

// MCS Rates, TxHT = 1
#define DESC92S_RATEMCS0				0x0c
#define DESC92S_RATEMCS1				0x0d
#define DESC92S_RATEMCS2				0x0e
#define DESC92S_RATEMCS3				0x0f
#define DESC92S_RATEMCS4				0x10
#define DESC92S_RATEMCS5				0x11
#define DESC92S_RATEMCS6				0x12
#define DESC92S_RATEMCS7				0x13
#define DESC92S_RATEMCS8				0x14
#define DESC92S_RATEMCS9				0x15
#define DESC92S_RATEMCS10				0x16
#define DESC92S_RATEMCS11				0x17
#define DESC92S_RATEMCS12				0x18
#define DESC92S_RATEMCS13				0x19
#define DESC92S_RATEMCS14				0x1a
#define DESC92S_RATEMCS15				0x1b
#define DESC92S_RATEMCS15_SG			0x1c
#define DESC92S_RATEMCS32				0x20
//#endif

#define RTL819X_DEFAULT_RF_TYPE RF_1T2R

#define IEEE80211_WATCH_DOG_TIME    2000
#define		PHY_Beacon_RSSI_SLID_WIN_MAX		10
//for txpowertracking by amy
#define 	OFDM_Table_Length	19
#define	CCK_Table_length	12

#ifdef RTL8192SU
//
//Tx Descriptor for RLT8192SU(Normal mode)
//
typedef struct _tx_desc_819x_usb {
	// DWORD 0
	u16		PktSize;//:16;
	u8		Offset;//:8;
	u8		Type:2;	// Reserved for MAC header Frame Type subfield.
	u8		LastSeg:1;
	u8		FirstSeg:1;
	u8		LINIP:1;
	u8		AMSDU:1;
	u8		GF:1;
	u8		OWN:1;	
	
	// DWORD 1
	u8		MacID:5;			
	u8		MoreData:1;
	u8		MOREFRAG:1;
	u8		PIFS:1;
	u8		QueueSelect:5;
	u8		AckPolicy:2;
	u8		NoACM:1;
	u8		NonQos:1;
	u8		KeyID:2;
	u8		OUI:1;
	u8		PktType:1;
	u8		EnDescID:1;
	u8		SecType:2;
	u8		HTC:1;	//padding0
	u8		WDS:1;	//padding1
	u8		PktOffset:5;	//padding_len (hw)	
	u8		HWPC:1;		
	
	// DWORD 2
	u32		DataRetryLmt:6;
	u32		RetryLmtEn:1;
	u32		TSFL:5;	
	u32		RTSRC:6;	// Reserved for HW RTS Retry Count.
	u32		DATARC:6;	// Reserved for HW DATA Retry Count.
	u32		Rsvd1:5;
	u32		AllowAggregation:1;
	u32		BK:1;	//Aggregation break.
	u32		OwnMAC:1;
	
	// DWORD 3
	u8		NextHeadPage;//:8;
	u8		TailPage;//:8;
	u16		Seq:12;
	u16		Frag:4;	

	// DWORD 4
	u32		RTSRate:6;
	u32		DisRTSFB:1;
	u32		RTSRateFBLmt:4;
	u32		CTS2Self:1;
	u32		RTSEn:1;
	u32		RaBRSRID:3;	//Rate adaptive BRSR ID.
	u32		TxHT:1;
	u32		TxShort:1;//for data
	u32		TxBandwidth:1;
	u32		TxSubCarrier:2;
	u32		STBC:2;
	u32		RD:1;
	u32		RTSHT:1;
	u32		RTSShort:1;
	u32		RTSBW:1;
	u32		RTSSubcarrier:2;
	u32		RTSSTBC:2;
	u32		USERATE:1;	
	// DWORD 5
	u32		PktID:9;
	u32		TxRate:6;
	u32		DISFB:1;
	u32		DataRateFBLmt:5;
	u32		TxAGC:11;	

	// DWORD 6
	u16		IPChkSum;//:16;
	u16		TCPChkSum;//:16;

	// DWORD 7
	//u16		TxBuffSize;//:16;//pcie
        u16     	TxBufferSize;
	u16		IPHdrOffset:8;
	u16		Rsvd2:7;
	u16		TCPEn:1;	
}tx_desc_819x_usb, *ptx_desc_819x_usb;
typedef struct _tx_status_desc_8192s_usb{
	
	//DWORD 0
	u8		TxRate:6;
	u8		Rsvd1:1;
	u8		BandWidth:1;
	u8		RTSRate:6;
	u8		AGGLS:1;
	u8		AGG:1;
	u8		RTSRC:6;
	u8		DataRC:6;
	u8		FailCause:2;
	u8		TxOK:1;
	u8		Own:1;

	//DWORD 1
	u16		Seq:12;
	u8		QueueSel:5;
	u8		MACID:5;
	u8		PwrMgt:1;
	u8		MoreData:1;
	u8		Rsvd2;

	//DWORD 2
	u8		RxAGC1;
	u8		RxAGC2;
	u8		RxAGC3;
	u8		RxAGC4;
}tx_status_desc_8192s_usb, *ptx_status_desc_8192s_usb;
#else
/* for rtl819x */
typedef struct _tx_desc_819x_usb {
        //DWORD 0
        u16	PktSize;
        u8	Offset;
        u8	Reserved0:3;
        u8	CmdInit:1;
        u8	LastSeg:1;
        u8	FirstSeg:1;
        u8	LINIP:1;
        u8	OWN:1;

        //DWORD 1
        u8	TxFWInfoSize;
        u8	RATid:3;
        u8	DISFB:1;
        u8	USERATE:1;
        u8	MOREFRAG:1;
        u8	NoEnc:1;
        u8	PIFS:1;
        u8	QueueSelect:5;
        u8	NoACM:1;
        u8	Reserved1:2;
        u8	SecCAMID:5;
        u8	SecDescAssign:1;
        u8	SecType:2;

        //DWORD 2
        u16	TxBufferSize;
        //u16 Reserved2;
        u8	ResvForPaddingLen:7;
        u8	Reserved3:1;
        u8	Reserved4;

        //DWORD 3, 4, 5
        u32	Reserved5;
        u32	Reserved6;
        u32	Reserved7;
}tx_desc_819x_usb, *ptx_desc_819x_usb;
#endif

#ifdef USB_TX_DRIVER_AGGREGATION_ENABLE
typedef struct _tx_desc_819x_usb_aggr_subframe {
	//DWORD 0
	u16	PktSize;
	u8	Offset;
	u8	TxFWInfoSize;

	//DWORD 1
	u8	RATid:3;
	u8	DISFB:1;
	u8	USERATE:1;
	u8	MOREFRAG:1;
	u8	NoEnc:1;
	u8	PIFS:1;
	u8	QueueSelect:5;
	u8	NoACM:1;
	u8	Reserved1:2;
	u8	SecCAMID:5;
	u8	SecDescAssign:1;
	u8	SecType:2;
	u8	PacketID:7;
	u8	OWN:1;
}tx_desc_819x_usb_aggr_subframe, *ptx_desc_819x_usb_aggr_subframe;
#endif


#ifdef RTL8192SU
//
//Tx Descriptor for RLT8192SU(Load FW mode)
//
typedef struct _tx_desc_cmd_819x_usb{  
       // DWORD 0
	u16		PktSize;
	u8		Offset;
	u8		Rsvd0:4;
	u8		LINIP:1;
	u8		Rsvd1:2;
	u8		OWN:1;	

	// DWORD 1, 2, 3, 4, 5, 6 are all reserved.		
	u32		Rsvd2;
	u32		Rsvd3;
	u32		Rsvd4;
	u32		Rsvd5;
	u32		Rsvd6;
	u32		Rsvd7;	

	// DWORD 7
	u16		TxBuffSize;//pcie
	u16		Rsvd8;		
}tx_desc_cmd_819x_usb, *ptx_desc_cmd_819x_usb;
//
//H2C Command for RLT8192SU(Host TxCmd)
//
typedef struct _tx_h2c_desc_cmd_8192s_usb{  
       // DWORD 0
	u32		PktSize:16;
	u32		Offset:8;
	u32		Rsvd0:7;
	u32		OWN:1;	
	
	// DWORD 1	
	u32		Rsvd1:8;
	u32		QSEL:5;
	u32		Rsvd2:19;

	// DWORD 2
	u32		Rsvd3;	

	// DWORD 3
	u32		NextHeadPage:8;
	u32		TailPage:8;
	u32		Rsvd4:16;	

	// DWORD 4, 5, 6, 7
	u32		Rsvd5;
	u32		Rsvd6;
	u32		Rsvd7;
	u32		Rsvd8;
}tx_h2c_desc_cmd_8192s_usb, *ptx_h2c_desc_cmd_8192s_usb;


typedef struct _tx_h2c_cmd_hdr_8192s_usb{  
       // DWORD 0
	u32		CmdLen:16;
	u32		ElementID:8;
	u32		CmdSeq:8;
		
	// DWORD 1	
	u32		Rsvd0;
}tx_h2c_cmd_hdr_8192s_usb, *ptx_h2c_cmd_hdr_8192s_usb;
#else
typedef struct _tx_desc_cmd_819x_usb {
        //DWORD 0
	u16	Reserved0;
	u8	Reserved1;
	u8	Reserved2:3;
	u8	CmdInit:1;
	u8	LastSeg:1;
	u8	FirstSeg:1;
	u8	LINIP:1;
	u8	OWN:1;

        //DOWRD 1
	//u32	Reserved3;
	u8	TxFWInfoSize;
	u8	Reserved3;
	u8	QueueSelect;
	u8	Reserved4;

        //DOWRD 2
	u16 	TxBufferSize;
	u16	Reserved5;
	
       //DWORD 3,4,5
	//u32	TxBufferAddr;
	//u32	NextDescAddress;
	u32	Reserved6;
	u32	Reserved7;
	u32	Reserved8;
}tx_desc_cmd_819x_usb, *ptx_desc_cmd_819x_usb;
#endif

#ifdef RTL8192SU
typedef struct _tx_fwinfo_819x_usb{
	//DWORD 0
	u8			TxRate:7;
	u8			CtsEnable:1;
	u8			RtsRate:7;
	u8			RtsEnable:1;
	u8			TxHT:1;
	u8			Short:1;						//Short PLCP for CCK, or short GI for 11n MCS
	u8			TxBandwidth:1;				// This is used for HT MCS rate only.
	u8			TxSubCarrier:2;				// This is used for legacy OFDM rate only.
	u8			STBC:2;
	u8			AllowAggregation:1;
	u8			RtsHT:1;						//Interpre RtsRate field as high throughput data rate
	u8			RtsShort:1;					//Short PLCP for CCK, or short GI for 11n MCS
	u8			RtsBandwidth:1;				// This is used for HT MCS rate only.
	u8			RtsSubcarrier:2;				// This is used for legacy OFDM rate only.
	u8			RtsSTBC:2;
	u8			EnableCPUDur:1;				//Enable firmware to recalculate and assign packet duration

	//DWORD 1
	u32			RxMF:2;
	u32			RxAMD:3;
        u32			Reserved1:3;
	u32			TxAGCOffSet:4;//TxAGCOffset:4;
	u32			TxAGCSign:1;
	u32			Tx_INFO_RSVD:6;
	u32			PacketID:13;
}tx_fwinfo_819x_usb, *ptx_fwinfo_819x_usb;
#else
typedef struct _tx_fwinfo_819x_usb {
        //DOWRD 0
        u8		TxRate:7;
        u8		CtsEnable:1;
        u8		RtsRate:7;
        u8		RtsEnable:1;
        u8		TxHT:1;
        u8		Short:1;                //Short PLCP for CCK, or short GI for 11n MCS
        u8		TxBandwidth:1;          // This is used for HT MCS rate only.
        u8		TxSubCarrier:2;         // This is used for legacy OFDM rate only.
        u8		STBC:2;
        u8		AllowAggregation:1;
        u8		RtsHT:1;                //Interpre RtsRate field as high throughput data rate
        u8		RtsShort:1;             //Short PLCP for CCK, or short GI for 11n MCS
        u8		RtsBandwidth:1;         // This is used for HT MCS rate only.
        u8		RtsSubcarrier:2;        // This is used for legacy OFDM rate only.
        u8		RtsSTBC:2;
        u8		EnableCPUDur:1;         //Enable firmware to recalculate and assign packet duration

        //DWORD 1
        u32		RxMF:2;
        u32		RxAMD:3;
        u32		TxPerPktInfoFeedback:1;//1 indicate Tx info gathtered by firmware and returned by Rx Cmd
        u32		Reserved1:2;
        u32		TxAGCOffSet:4;
        u32		TxAGCSign:1;
        u32		Tx_INFO_RSVD:6;
	u32		PacketID:13;
        //u32                Reserved;
}tx_fwinfo_819x_usb, *ptx_fwinfo_819x_usb;
#endif

typedef struct rtl8192_rx_info {
	struct urb *urb;
	struct net_device *dev;
	u8 out_pipe;
}rtl8192_rx_info ;

#ifdef RTL8192SU
//typedef struct _RX_DESC_STATUS_8192SU{
typedef struct rx_desc_819x_usb{
	//DWORD 0
	u16		Length:14;
	u16		CRC32:1;
	u16		ICV:1;
	u8		RxDrvInfoSize:4;
	u8		Security:3;
	u8		Qos:1;
	u8		Shift:2;
	u8		PHYStatus:1;
	u8		SWDec:1;
	u8		LastSeg:1;
	u8		FirstSeg:1;
	u8		EOR:1;
	u8		Own:1;	

	//DWORD 1
	u16		MACID:5;
	u16		TID:4;
	u16		HwRsvd:5;
	u16		PAGGR:1;
	u16		FAGGR:1;
	u8		A1_FIT:4;
	u8		A2_FIT:4;
	u8		PAM:1;
	u8		PWR:1;
	u8		MoreData:1;
	u8		MoreFrag:1;
	u8		Type:2;
	u8		MC:1;
	u8		BC:1;

	//DWORD 2
	u16		Seq:12;
	u16		Frag:4;
#ifdef USB_RX_AGGREGATION_SUPPORT	
	u8		UsbAggPktNum;//:8;
#else
	u8		NextPktLen;//:8;
#endif
	u8		Rsvd0:6;
	u8		NextIND:1;
	u8		Rsvd1:1;

	//DWORD 3
	u8		RxMCS:6;
	u8		RxHT:1;
	u8		AMSDU:1;
	u8		SPLCP:1;
	u8		BW:1;
	u8		HTC:1;
	u8		TCPChkRpt:1;
	u8		IPChkRpt:1;
	u8		TCPChkValID:1;
	u8		HwPCErr:1;
	u8		HwPCInd:1;
	u16		IV0;//:16;

	//DWORD 4
	u32		IV1;

	//DWORD 5
	u32		TSFL;
//}RX_DESC_STATUS_8192SU, *PRX_DESC_STATUS_8192SU;
}rx_desc_819x_usb, *prx_desc_819x_usb;
#else
typedef struct rx_desc_819x_usb{
	//DOWRD 0
	u16                 Length:14;
	u16                 CRC32:1;
	u16                 ICV:1;
	u8                  RxDrvInfoSize;
	u8                  Shift:2;
	u8                  PHYStatus:1;
	u8                  SWDec:1;
	//u8                LastSeg:1;
	//u8                FirstSeg:1;
	//u8                EOR:1;
	//u8                OWN:1;
	u8                  Reserved1:4;

	//DWORD 1
	u32                 Reserved2;

	//DWORD 2
	//u32               Reserved3;

	//DWORD 3
	//u32                BufferAddress;

}rx_desc_819x_usb, *prx_desc_819x_usb;
#endif

#ifdef USB_RX_AGGREGATION_SUPPORT
typedef struct _rx_desc_819x_usb_aggr_subframe{
	//DOWRD 0
	u16			Length:14;
	u16			CRC32:1;
	u16			ICV:1;
	u8			Offset;
	u8			RxDrvInfoSize;
	//DOWRD 1
	u8			Shift:2;
	u8			PHYStatus:1;
	u8			SWDec:1;
	u8			Reserved1:4;
	u8			Reserved2;
	u16			Reserved3;
	//DWORD 2
	//u4Byte		Reserved3;
	//DWORD 3
	//u4Byte           	BufferAddress;
}rx_desc_819x_usb_aggr_subframe, *prx_desc_819x_usb_aggr_subframe;
#endif

#ifdef RTL8192SU
//
// Driver info are written to the begining of the RxBuffer
//
//typedef struct _RX_DRIVER_INFO_8192S{
typedef struct rx_drvinfo_819x_usb{
	//
	// Driver info contain PHY status and other variabel size info	
	// PHY Status content as below
	//
	
	//DWORD 0
	/*u4Byte			gain_0:7;
	u4Byte			trsw_0:1;
	u4Byte			gain_1:7;
	u4Byte			trsw_1:1;
	u4Byte			gain_2:7;
	u4Byte			trsw_2:1;
	u4Byte			gain_3:7;
	u4Byte			trsw_3:1;	*/
	u8			gain_trsw[4];

	//DWORD 1
	/*u4Byte			pwdb_all:8;
	u4Byte			cfosho_0:8;
	u4Byte			cfosho_1:8;
	u4Byte			cfosho_2:8;*/
	u8			pwdb_all;
	u8			cfosho[4];

	//DWORD 2
	/*u4Byte			cfosho_3:8;
	u4Byte			cfotail_0:8;
	u4Byte			cfotail_1:8;
	u4Byte			cfotail_2:8;*/
	u8			cfotail[4];

	//DWORD 3
	/*u4Byte			cfotail_3:8;
	u4Byte			rxevm_0:8;
	u4Byte			rxevm_1:8;
	u4Byte			rxsnr_0:8;*/
	char			        rxevm[2];
	char			        rxsnr[4];

	//DWORD 4
	/*u4Byte			rxsnr_1:8;
	u4Byte			rxsnr_2:8;
	u4Byte			rxsnr_3:8;
	u4Byte			pdsnr_0:8;*/
	u8			pdsnr[2];

	//DWORD 5
	/*u4Byte			pdsnr_1:8;
	u4Byte			csi_current_0:8;
	u4Byte			csi_current_1:8;
	u4Byte			csi_target_0:8;*/
	u8			csi_current[2];
	u8			csi_target[2];

	//DWORD 6
	/*u4Byte			csi_target_1:8;
	u4Byte			sigevm:8;
	u4Byte			max_ex_pwr:8;
	u4Byte			ex_intf_flag:1;
	u4Byte			sgi_en:1;
	u4Byte			rxsc:2;
	u4Byte			reserve:4;*/
	u8			sigevm;
	u8			max_ex_pwr;
	u8			ex_intf_flag:1;
	u8			sgi_en:1;
	u8			rxsc:2;
	u8			reserve:4;
	
}rx_drvinfo_819x_usb, *prx_drvinfo_819x_usb;
#else
typedef struct rx_drvinfo_819x_usb{
	//DWORD 0
	u16                 Reserved1:12;
	u16                 PartAggr:1;
	u16                 FirstAGGR:1;
	u16                 Reserved2:2;

	u8                  RxRate:7;
	u8                  RxHT:1;

	u8                  BW:1;
	u8                  SPLCP:1;
	u8                  Reserved3:2;
	u8                  PAM:1;
	u8                  Mcast:1;
	u8                  Bcast:1;
	u8                  Reserved4:1;

	//DWORD 1
	u32                  TSFL;

}rx_drvinfo_819x_usb, *prx_drvinfo_819x_usb;
#endif

	#define HWSET_MAX_SIZE_92S	128
#ifdef RTL8192SU
	#define MAX_802_11_HEADER_LENGTH 40
	#define MAX_PKT_AGG_NUM		256
	#define TX_PACKET_SHIFT_BYTES USB_HWDESC_HEADER_LEN
#else
	#define MAX_802_11_HEADER_LENGTH        (40 + MAX_FIRMWARE_INFORMATION_SIZE)
	#define	MAX_PKT_AGG_NUM		64
	#define TX_PACKET_SHIFT_BYTES (USB_HWDESC_HEADER_LEN + sizeof(tx_fwinfo_819x_usb)) 
#endif

#define MAX_DEV_ADDR_SIZE		8  /* support till 64 bit bus width OS */
#define MAX_FIRMWARE_INFORMATION_SIZE   32 /*2006/04/30 by Emily forRTL8190*/
//#define MAX_802_11_HEADER_LENGTH        (40 + MAX_FIRMWARE_INFORMATION_SIZE)
#define ENCRYPTION_MAX_OVERHEAD		128
#define	USB_HWDESC_HEADER_LEN		sizeof(tx_desc_819x_usb)
//#define TX_PACKET_SHIFT_BYTES 	  	(USB_HWDESC_HEADER_LEN + sizeof(tx_fwinfo_819x_usb)) 
#define MAX_FRAGMENT_COUNT		8
#ifdef RTL8192U
#ifdef USB_TX_DRIVER_AGGREGATION_ENABLE
#define MAX_TRANSMIT_BUFFER_SIZE			32000
#else
#define MAX_TRANSMIT_BUFFER_SIZE			8000
#endif	
#else
#define MAX_TRANSMIT_BUFFER_SIZE  	(1600+(MAX_802_11_HEADER_LENGTH+ENCRYPTION_MAX_OVERHEAD)*MAX_FRAGMENT_COUNT) 
#endif
#ifdef USB_TX_DRIVER_AGGREGATION_ENABLE
#define TX_PACKET_DRVAGGR_SUBFRAME_SHIFT_BYTES (sizeof(tx_desc_819x_usb_aggr_subframe) + sizeof(tx_fwinfo_819x_usb))
#endif
#define scrclng					4		// octets for crc32 (FCS, ICV)

typedef enum rf_optype
{
	RF_OP_By_SW_3wire = 0,
	RF_OP_By_FW,
	RF_OP_MAX
}rf_op_type;
/* 8190 Loopback Mode definition */
typedef enum _rtl819xUsb_loopback{
	RTL819xU_NO_LOOPBACK = 0,
	RTL819xU_MAC_LOOPBACK = 1,
	RTL819xU_DMA_LOOPBACK = 2,
	RTL819xU_CCK_LOOPBACK = 3,
}rtl819xUsb_loopback_e;

/* for rtl819x */
typedef enum _RT_STATUS{
	RT_STATUS_SUCCESS = 0,
	RT_STATUS_FAILURE = 1,
	RT_STATUS_PENDING = 2,
	RT_STATUS_RESOURCE = 3
}RT_STATUS,*PRT_STATUS;

//#ifdef RTL8192SU
typedef enum _RTL8192SUSB_LOOPBACK{
	RTL8192SU_NO_LOOPBACK = 0,
	RTL8192SU_MAC_LOOPBACK = 1,
	RTL8192SU_DMA_LOOPBACK = 2,
	RTL8192SU_CCK_LOOPBACK = 3,
}RTL8192SUSB_LOOPBACK_E;
//#endif


#if 0 
/* due to rtl8192 firmware */
typedef enum _desc_packet_type_e{
	DESC_PACKET_TYPE_INIT = 0,
	DESC_PACKET_TYPE_NORMAL = 1,	
}desc_packet_type_e;

typedef enum _firmware_source{
	FW_SOURCE_IMG_FILE = 0,
	FW_SOURCE_HEADER_FILE = 1,		//from header file
}firmware_source_e, *pfirmware_source_e;

typedef enum _firmware_status{
	FW_STATUS_0_INIT = 0,
	FW_STATUS_1_MOVE_BOOT_CODE = 1,
	FW_STATUS_2_MOVE_MAIN_CODE = 2,
	FW_STATUS_3_TURNON_CPU = 3,
	FW_STATUS_4_MOVE_DATA_CODE = 4,
	FW_STATUS_5_READY = 5,
}firmware_status_e;

typedef struct _rt_firmare_seg_container {
	u16	seg_size;
	u8	*seg_ptr;
}fw_seg_container, *pfw_seg_container;

#ifdef RTL8192SU
//--------------------------------------------------------------------------------
// 8192S Firmware related 
//--------------------------------------------------------------------------------
typedef  struct _RT_8192S_FIRMWARE_PRIV { //8-bytes alignment required

	//--- LONG WORD 0 ----
	u32		RegulatoryClass;
	u32		Rfintfs;
	
	//--- LONG WORD 1 ----
	u32		ChipVer;
	u32		HCISel;
	
	//--- LONG WORD 2 ----
	u32		IBKMode;
	u32		Rsvd00;
	
	//--- LONG WORD 3 ----
	u32		Rsvd01;
	u8		Qos_En;			// QoS enable
	u8		En40MHz;		// 40MHz BW enable
	u8		AMSDU2AMPDU_En;	//14181 convert AMSDU to AMPDU, 0: disable
	u8		AMPDU_En;		//111n AMPDU/AMSDU enable

	//--- LONG WORD 4 ----
	u8		rate_control_offload;//FW offloads, 0: driver handles
	u8		aggregation_offload;	// FW offloads, 0: driver handles
	u8		beacon_offload;	//FW offloads, 0: driver handles
	u8		MLME_offload;	// FW offloads, 0: driver handles
	u8		hwpc_offload;	// FW offloads, 0: driver handles
	u8		tcp_checksum_offload;	//FW offloads, 0: driver handles
	u8		tcp_offload;			//FW offloads, 0: driver handles
	u8		ps_control_offload;	//FW offloads, 0: driver handles

	//--- LONG WORD 5 ----
	u8		WWLAN_Offload;	// FW offloads, 0: driver handles
	u8		MPMode;	// normal mode, 0: MP mode;
	u16		Version;		//0x8000 ~ 0x8FFF for FPGA version, 0x0000 ~ 0x7FFF for ASIC version, 
	u16		Signature;	//0x12: 8712, 0x92: 8192S
	u16		Rsvd11;
	
//	u32		rsvd1;
//	u32		wireless_band;	//no A-band exists in 8712 
}RT_8192S_FIRMWARE_PRIV, *PRT_8192S_FIRMWARE_PRIV;

typedef struct _RT_8192S_FIRMWARE_HDR {//8-byte alinment required

	//--- LONG WORD 0 ----
	u16		Signature;
	u16		Version;		  //0x8000 ~ 0x8FFF for FPGA version, 0x0000 ~ 0x7FFF for ASIC version,
	u32		DMEMSize;    //define the size of boot loader


	//--- LONG WORD 1 ----
	u32		IMG_IMEM_SIZE;    //define the size of FW in IMEM
	u32		IMG_SRAM_SIZE;    //define the size of FW in SRAM

	//--- LONG WORD 2 ----
	u32		FW_PRIV_SIZE;       //define the size of DMEM variable 
	u32		Rsvd0;  

	//--- LONG WORD 3 ----
	u32		Rsvd1;
	u32		Rsvd2;

	RT_8192S_FIRMWARE_PRIV	FWPriv;
	
}RT_8192S_FIRMWARE_HDR, *PRT_8192S_FIRMWARE_HDR;

#define	RT_8192S_FIRMWARE_HDR_SIZE	80

typedef enum _FIRMWARE_8192S_STATUS{
	FW_STATUS_INIT = 0,
	FW_STATUS_LOAD_IMEM = 1,
	FW_STATUS_LOAD_EMEM = 2,
	FW_STATUS_LOAD_DMEM = 3,
	FW_STATUS_READY = 4,
}FIRMWARE_8192S_STATUS;

#define RTL8190_MAX_FIRMWARE_CODE_SIZE  64000   //64k

typedef struct _rt_firmware{	
	firmware_source_e	eFWSource;	
	PRT_8192S_FIRMWARE_HDR		pFwHeader;
	FIRMWARE_8192S_STATUS	FWStatus;
	u8		FwIMEM[64000];
	u8		FwEMEM[64000];
	u32		FwIMEMLen;
	u32		FwEMEMLen;	
	u8		szFwTmpBuffer[164000];	
	u16		CmdPacketFragThresold;		
	//firmware_status_e       firmware_status;//in 92u temp FIXLZM
	//u16               cmdpacket_frag_thresold;//in 92u temp FIXLZM
	//u8                firmware_buf[RTL8190_MAX_FIRMWARE_CODE_SIZE];//in 92u temp FIXLZM
	//u16               firmware_buf_size;//in 92u temp FIXLZM

}rt_firmware, *prt_firmware;
#else
typedef struct _rt_firmware{
	firmware_status_e firmware_status;
	u16               cmdpacket_frag_thresold;
#define RTL8190_MAX_FIRMWARE_CODE_SIZE  64000   //64k
#define MAX_FW_INIT_STEP                3
	u8                firmware_buf[MAX_FW_INIT_STEP][RTL8190_MAX_FIRMWARE_CODE_SIZE];
	u16               firmware_buf_size[MAX_FW_INIT_STEP];
}rt_firmware, *prt_firmware;
#endif
typedef struct _rt_firmware_info_819xUsb{
	u8		sz_info[16];
}rt_firmware_info_819xUsb, *prt_firmware_info_819xUsb;
#endif

//+by amy 080507
#define MAX_RECEIVE_BUFFER_SIZE	9100	// Add this to 9100 bytes to receive A-MSDU from RT-AP


/* Firmware Queue Layout */
#define NUM_OF_FIRMWARE_QUEUE		10
#define NUM_OF_PAGES_IN_FW		0x100

#ifdef USE_ONE_PIPE
#define NUM_OF_PAGE_IN_FW_QUEUE_BE	0x000
#define NUM_OF_PAGE_IN_FW_QUEUE_BK	0x000
#define NUM_OF_PAGE_IN_FW_QUEUE_VI	0x0ff
#define NUM_OF_PAGE_IN_FW_QUEUE_VO	0x000
#define NUM_OF_PAGE_IN_FW_QUEUE_HCCA	0
#define NUM_OF_PAGE_IN_FW_QUEUE_CMD	0x0
#define NUM_OF_PAGE_IN_FW_QUEUE_MGNT	0x00
#define NUM_OF_PAGE_IN_FW_QUEUE_HIGH	0
#define NUM_OF_PAGE_IN_FW_QUEUE_BCN	0x0
#define NUM_OF_PAGE_IN_FW_QUEUE_PUB	0x00
#else

#define NUM_OF_PAGE_IN_FW_QUEUE_BE	0x020
#define NUM_OF_PAGE_IN_FW_QUEUE_BK	0x020
#define NUM_OF_PAGE_IN_FW_QUEUE_VI	0x040
#define NUM_OF_PAGE_IN_FW_QUEUE_VO	0x040
#define NUM_OF_PAGE_IN_FW_QUEUE_HCCA	0
#define NUM_OF_PAGE_IN_FW_QUEUE_CMD	0x4
#define NUM_OF_PAGE_IN_FW_QUEUE_MGNT	0x20
#define NUM_OF_PAGE_IN_FW_QUEUE_HIGH	0
#define NUM_OF_PAGE_IN_FW_QUEUE_BCN	0x4
#define NUM_OF_PAGE_IN_FW_QUEUE_PUB	0x18

#endif

#define APPLIED_RESERVED_QUEUE_IN_FW	0x80000000
#define RSVD_FW_QUEUE_PAGE_BK_SHIFT	0x00
#define RSVD_FW_QUEUE_PAGE_BE_SHIFT	0x08
#define RSVD_FW_QUEUE_PAGE_VI_SHIFT	0x10
#define RSVD_FW_QUEUE_PAGE_VO_SHIFT	0x18
#define RSVD_FW_QUEUE_PAGE_MGNT_SHIFT	0x10
#define RSVD_FW_QUEUE_PAGE_CMD_SHIFT	0x08
#define RSVD_FW_QUEUE_PAGE_BCN_SHIFT	0x00
#define RSVD_FW_QUEUE_PAGE_PUB_SHIFT	0x08
//=================================================================
//=================================================================

#define EPROM_93c46 0
#define EPROM_93c56 1

#define DEFAULT_FRAG_THRESHOLD 2342U
#define MIN_FRAG_THRESHOLD     256U
#define DEFAULT_BEACONINTERVAL 0x64U
#define DEFAULT_BEACON_ESSID "Rtl819xU"

#define DEFAULT_SSID ""
#define DEFAULT_RETRY_RTS 7
#define DEFAULT_RETRY_DATA 7
#define PRISM_HDR_SIZE 64

#define		PHY_RSSI_SLID_WIN_MAX				100


typedef enum _WIRELESS_MODE {
	WIRELESS_MODE_UNKNOWN = 0x00,
	WIRELESS_MODE_A = 0x01,
	WIRELESS_MODE_B = 0x02,
	WIRELESS_MODE_G = 0x04,
	WIRELESS_MODE_AUTO = 0x08,
	WIRELESS_MODE_N_24G = 0x10,
	WIRELESS_MODE_N_5G = 0x20
} WIRELESS_MODE;


#define RTL_IOCTL_WPA_SUPPLICANT		SIOCIWFIRSTPRIV+30

typedef struct buffer
{
	struct buffer *next;
	u32 *buf;
	
} buffer;

typedef struct rtl_reg_debug{
        unsigned int  cmd;
        struct {
                unsigned char type;
                unsigned char addr;
                unsigned char page;
                unsigned char length;
        } head;
        unsigned char buf[0xff];
}rtl_reg_debug;





#if 0

typedef struct tx_pendingbuf
{
	struct ieee80211_txb *txb;
	short ispending;
	short descfrag;
} tx_pendigbuf;

#endif

typedef struct _rt_9x_tx_rate_history {
	u32             cck[4];
	u32             ofdm[8];
	// HT_MCS[0][]: BW=0 SG=0
	// HT_MCS[1][]: BW=1 SG=0
	// HT_MCS[2][]: BW=0 SG=1
	// HT_MCS[3][]: BW=1 SG=1
	u32             ht_mcs[4][16];
}rt_tx_rahis_t, *prt_tx_rahis_t;
typedef struct _RT_SMOOTH_DATA_4RF {
	char    elements[4][100];//array to store values
	u32     index;                  //index to current array to store
	u32     TotalNum;               //num of valid elements
	u32     TotalVal[4];            //sum of valid elements
}RT_SMOOTH_DATA_4RF, *PRT_SMOOTH_DATA_4RF;

#define MAX_8192U_RX_SIZE			8192    // This maybe changed for D-cut larger aggregation size
//stats seems messed up, clean it ASAP
typedef struct Stats
{
	unsigned long txrdu;
//	unsigned long rxrdu;
	//unsigned long rxnolast;
	//unsigned long rxnodata;
//	unsigned long rxreset;
//	unsigned long rxnopointer;
	unsigned long rxok;
	unsigned long rxframgment;
	unsigned long rxcmdpkt[4];		//08/05/08 amy rx cmd element txfeedback/bcn report/cfg set/query
	unsigned long rxurberr;
	unsigned long rxstaterr;
	unsigned long received_rate_histogram[4][32];	//0: Total, 1:OK, 2:CRC, 3:ICV, 2007 07 03 cosa
	unsigned long received_preamble_GI[2][32];		//0: Long preamble/GI, 1:Short preamble/GI
	unsigned long rx_AMPDUsize_histogram[5]; // level: (<4K), (4K~8K), (8K~16K), (16K~32K), (32K~64K)
	unsigned long rx_AMPDUnum_histogram[5]; // level: (<5), (5~10), (10~20), (20~40), (>40)
	unsigned long numpacket_matchbssid;	// debug use only.
	unsigned long numpacket_toself;		// debug use only.
	unsigned long num_process_phyinfo;		// debug use only.
	unsigned long numqry_phystatus;
	unsigned long numqry_phystatusCCK;
	unsigned long numqry_phystatusHT;
	unsigned long received_bwtype[5];              //0: 20M, 1: funn40M, 2: upper20M, 3: lower20M, 4: duplicate
	unsigned long txnperr;
	unsigned long txnpdrop;
	unsigned long txresumed;
//	unsigned long rxerr;
//	unsigned long rxoverflow;
//	unsigned long rxint;
	unsigned long txnpokint;
//	unsigned long txhpokint;
//	unsigned long txhperr;
//	unsigned long ints;
//	unsigned long shints;
	unsigned long txoverflow;
//	unsigned long rxdmafail;
//	unsigned long txbeacon;
//	unsigned long txbeaconerr;
	unsigned long txlpokint;
	unsigned long txlpdrop;
	unsigned long txlperr;
	unsigned long txbeokint;
	unsigned long txbedrop;
	unsigned long txbeerr;
	unsigned long txbkokint;
	unsigned long txbkdrop;
	unsigned long txbkerr;
	unsigned long txviokint;
	unsigned long txvidrop;
	unsigned long txvierr;
	unsigned long txvookint;
	unsigned long txvodrop;
	unsigned long txvoerr;
	unsigned long txbeaconokint;
	unsigned long txbeacondrop;
	unsigned long txbeaconerr;
	unsigned long txmanageokint;
	unsigned long txmanagedrop;
	unsigned long txmanageerr;
	unsigned long txdatapkt;
	unsigned long txfeedback;
	unsigned long txfeedbackok;

	unsigned long txoktotal;
	unsigned long txokbytestotal;
	unsigned long txokinperiod;
	unsigned long txmulticast;
	unsigned long txbytesmulticast;
	unsigned long txbroadcast;
	unsigned long txbytesbroadcast;
	unsigned long txunicast;
	unsigned long txbytesunicast;

	unsigned long rxoktotal;
	unsigned long rxbytesunicast;
	unsigned long txfeedbackfail;
	unsigned long txerrtotal;
	unsigned long txerrbytestotal;
	unsigned long txerrmulticast;
	unsigned long txerrbroadcast;
	unsigned long txerrunicast;
	unsigned long txretrycount;
	unsigned long txfeedbackretry;
	u8	      last_packet_rate;
	unsigned long slide_signal_strength[100];
	unsigned long slide_evm[100];
	unsigned long slide_rssi_total;	// For recording sliding window's RSSI value
	unsigned long slide_evm_total;	// For recording sliding window's EVM value
	long signal_strength; // Transformed, in dbm. Beautified signal strength for UI, not correct.
	long signal_quality;
	long last_signal_strength_inpercent;
	long recv_signal_power;	// Correct smoothed ss in Dbm, only used in driver to report real power now.
	u8 rx_rssi_percentage[4];
	u8 rx_evm_percentage[2];
	long rxSNRdB[4];
	rt_tx_rahis_t txrate;
	u32 Slide_Beacon_pwdb[100];     //cosa add for beacon rssi
	u32 Slide_Beacon_Total;         //cosa add for beacon rssi
	RT_SMOOTH_DATA_4RF              cck_adc_pwdb;

	u32	CurrentShowTxate;
} Stats;


// Bandwidth Offset                     	
#define HAL_PRIME_CHNL_OFFSET_DONT_CARE		0
#define HAL_PRIME_CHNL_OFFSET_LOWER			1
#define HAL_PRIME_CHNL_OFFSET_UPPER			2

//+by amy 080507

typedef struct 	ChnlAccessSetting {
	u16 SIFS_Timer;
	u16 DIFS_Timer; 
	u16 SlotTimeTimer;
	u16 EIFS_Timer;
	u16 CWminIndex;
	u16 CWmaxIndex;
}*PCHANNEL_ACCESS_SETTING,CHANNEL_ACCESS_SETTING;

typedef struct _BB_REGISTER_DEFINITION{
	u32 rfintfs; 			// set software control: //		0x870~0x877[8 bytes]
	u32 rfintfi; 			// readback data: //		0x8e0~0x8e7[8 bytes]
	u32 rfintfo; 			// output data: //		0x860~0x86f [16 bytes]
	u32 rfintfe; 			// output enable: //		0x860~0x86f [16 bytes]
	u32 rf3wireOffset; 		// LSSI data: //		0x840~0x84f [16 bytes]
	u32 rfLSSI_Select; 		// BB Band Select: //		0x878~0x87f [8 bytes]
	u32 rfTxGainStage;		// Tx gain stage: //		0x80c~0x80f [4 bytes]
	u32 rfHSSIPara1; 		// wire parameter control1 : //		0x820~0x823,0x828~0x82b, 0x830~0x833, 0x838~0x83b [16 bytes]
	u32 rfHSSIPara2; 		// wire parameter control2 : //		0x824~0x827,0x82c~0x82f, 0x834~0x837, 0x83c~0x83f [16 bytes]
	u32 rfSwitchControl; 	//Tx Rx antenna control : //		0x858~0x85f [16 bytes]
	u32 rfAGCControl1; 	//AGC parameter control1 : //		0xc50~0xc53,0xc58~0xc5b, 0xc60~0xc63, 0xc68~0xc6b [16 bytes] 
	u32 rfAGCControl2; 	//AGC parameter control2 : //		0xc54~0xc57,0xc5c~0xc5f, 0xc64~0xc67, 0xc6c~0xc6f [16 bytes] 
	u32 rfRxIQImbalance; 	//OFDM Rx IQ imbalance matrix : //		0xc14~0xc17,0xc1c~0xc1f, 0xc24~0xc27, 0xc2c~0xc2f [16 bytes]
	u32 rfRxAFE;  			//Rx IQ DC ofset and Rx digital filter, Rx DC notch filter : //		0xc10~0xc13,0xc18~0xc1b, 0xc20~0xc23, 0xc28~0xc2b [16 bytes]
	u32 rfTxIQImbalance; 	//OFDM Tx IQ imbalance matrix //		0xc80~0xc83,0xc88~0xc8b, 0xc90~0xc93, 0xc98~0xc9b [16 bytes]
	u32 rfTxAFE; 			//Tx IQ DC Offset and Tx DFIR type //		0xc84~0xc87,0xc8c~0xc8f, 0xc94~0xc97, 0xc9c~0xc9f [16 bytes]
	u32 rfLSSIReadBack; 	//LSSI RF readback data //		0x8a0~0x8af [16 bytes]
	u32 rfLSSIReadBackPi; 	//LSSI RF readback data PI mode 0x8b8-8bc for Path A and B
}BB_REGISTER_DEFINITION_T, *PBB_REGISTER_DEFINITION_T;

typedef enum _RT_RF_TYPE_819xU{
        RF_TYPE_MIN = 0,
        RF_8225,
        RF_8256,
        RF_8258,
	RF_6052=4,		// 4 11b/g/n RF
        RF_PSEUDO_11N = 5,
}RT_RF_TYPE_819xU, *PRT_RF_TYPE_819xU;

//#ifdef RTL8192SU
typedef enum _RF_POWER_STATE{
	RF_ON, 
	RF_SLEEP, 
	RF_OFF, 
	RF_SHUT_DOWN,
}RF_POWER_STATE, *PRF_POWER_STATE;
//#endif

typedef struct _rate_adaptive
{
	u8				rate_adaptive_disabled;
	u8				ratr_state;
	u16				reserve;	
	
	u32				high_rssi_thresh_for_ra;
	u32				high2low_rssi_thresh_for_ra;
	u8				low2high_rssi_thresh_for_ra40M;
	u32				low_rssi_thresh_for_ra40M;
	u8				low2high_rssi_thresh_for_ra20M;
	u32				low_rssi_thresh_for_ra20M;
	u32				upper_rssi_threshold_ratr;
	u32				middle_rssi_threshold_ratr;
	u32				low_rssi_threshold_ratr;
	u32				low_rssi_threshold_ratr_40M;
	u32				low_rssi_threshold_ratr_20M;
	u8				ping_rssi_enable;	//cosa add for test
	u32				ping_rssi_ratr;	//cosa add for test
	u32				ping_rssi_thresh_for_ra;//cosa add for test
	u32				last_ratr;
	
} rate_adaptive, *prate_adaptive;

#define TxBBGainTableLength 37
#define	CCKTxBBGainTableLength 23

typedef struct _txbbgain_struct
{
	long	txbb_iq_amplifygain;
	u32	txbbgain_value;
} txbbgain_struct, *ptxbbgain_struct;

typedef struct _ccktxbbgain_struct
{
	//The Value is from a22 to a29 one Byte one time is much Safer 
	u8	ccktxbb_valuearray[8];
} ccktxbbgain_struct,*pccktxbbgain_struct;


typedef struct _init_gain
{
	u8				xaagccore1;
	u8				xbagccore1;
	u8				xcagccore1;
	u8				xdagccore1;
	u8				cca;

} init_gain, *pinit_gain;
//by amy 0606

typedef struct _phy_ofdm_rx_status_report_819xusb
{	
	u8	trsw_gain_X[4];	
	u8	pwdb_all;
	u8	cfosho_X[4];
	u8	cfotail_X[4];
	u8	rxevm_X[2];
	u8	rxsnr_X[4];
	u8	pdsnr_X[2];
	u8	csi_current_X[2];
	u8	csi_target_X[2];
	u8	sigevm;
	u8	max_ex_pwr;
	u8	sgi_en;
	u8  rxsc_sgien_exflg;
}phy_sts_ofdm_819xusb_t;

typedef struct _phy_cck_rx_status_report_819xusb
{
	/* For CCK rate descriptor. This is a unsigned 8:1 variable. LSB bit presend
	   0.5. And MSB 7 bts presend a signed value. Range from -64~+63.5. */
	u8	adc_pwdb_X[4];
	u8	sq_rpt;	
	u8	cck_agc_rpt;
}phy_sts_cck_819xusb_t;


typedef struct _phy_ofdm_rx_status_rxsc_sgien_exintfflag{
	u8			reserved:4;
	u8			rxsc:2;	
	u8			sgi_en:1;	
	u8			ex_intf_flag:1;	
}phy_ofdm_rx_status_rxsc_sgien_exintfflag;

typedef enum _RT_CUSTOMER_ID
{
	RT_CID_DEFAULT = 0,
	RT_CID_8187_ALPHA0 = 1,
	RT_CID_8187_SERCOMM_PS = 2,
	RT_CID_8187_HW_LED = 3,
	RT_CID_8187_NETGEAR = 4,
	RT_CID_WHQL = 5,
	RT_CID_819x_CAMEO  = 6, 
	RT_CID_819x_RUNTOP = 7,
	RT_CID_819x_Senao = 8,
	RT_CID_TOSHIBA = 9,	// Merge by Jacken, 2008/01/31.
	RT_CID_819x_Netcore = 10,
	RT_CID_Nettronix = 11,
	RT_CID_DLINK = 12,
	RT_CID_PRONET = 13,
}RT_CUSTOMER_ID, *PRT_CUSTOMER_ID;

//================================================================================
// LED customization.
//================================================================================

typedef	enum _LED_STRATEGY_8190{
	SW_LED_MODE0, // SW control 1 LED via GPIO0. It is default option.
	SW_LED_MODE1, // SW control for PCI Express
	SW_LED_MODE2, // SW control for Cameo.
	SW_LED_MODE3, // SW contorl for RunTop.
	SW_LED_MODE4, // SW control for Netcore
	HW_LED, // HW control 2 LEDs, LED0 and LED1 (there are 4 different control modes)
}LED_STRATEGY_8190, *PLED_STRATEGY_8190;

typedef enum _RESET_TYPE {
	RESET_TYPE_NORESET = 0x00,
	RESET_TYPE_NORMAL = 0x01,
	RESET_TYPE_SILENT = 0x02
} RESET_TYPE;

/* The simple tx command OP code. */
typedef enum _tag_TxCmd_Config_Index{
	TXCMD_TXRA_HISTORY_CTRL				= 0xFF900000,
	TXCMD_RESET_TX_PKT_BUFF				= 0xFF900001,
	TXCMD_RESET_RX_PKT_BUFF				= 0xFF900002,
	TXCMD_SET_TX_DURATION				= 0xFF900003,
	TXCMD_SET_RX_RSSI						= 0xFF900004,
	TXCMD_SET_TX_PWR_TRACKING			= 0xFF900005,
	TXCMD_XXXX_CTRL,
}DCMD_TXCMD_OP;

typedef enum{
	NIC_8192U = 1,
	NIC_8190P = 2,
	NIC_8192E = 3,
	NIC_8192SE = 4,
	NIC_8192SU = 5,
	} nic_t;

//definded by WB. Ready to fill handlers for different NIC types.
//add handle here when necessary.
struct rtl819x_ops{
	nic_t nic_type;
	void (* rtl819x_read_eeprom_info)(struct net_device *dev);
	short (* rtl819x_tx)(struct net_device *dev, struct sk_buff* skb);
	short (* rtl819x_tx_cmd)(struct net_device *dev, struct sk_buff *skb);
	void (* rtl819x_rx_nomal)(struct sk_buff* skb);
	void (* rtl819x_rx_cmd)(struct sk_buff *skb);
	bool (*	rtl819x_adapter_start)(struct net_device *dev);
	void (* rtl819x_link_change)(struct net_device *dev);
	void (*	rtl819x_initial_gain)(struct net_device *dev,u8 Operation);
	void (*	rtl819x_query_rxdesc_status)(struct sk_buff *skb, struct ieee80211_rx_stats *stats, bool bIsRxAggrSubframe);
};

typedef struct r8192_priv
{
	struct rtl819x_ops* ops;
	struct usb_device *udev;
	//added for maintain info from eeprom
	short epromtype;
	u16 eeprom_vid;
	u16 eeprom_pid;
	u8  eeprom_CustomerID;
	u8  eeprom_SubCustomerID;
	u8  eeprom_ChannelPlan;
	RT_CUSTOMER_ID CustomerID;
	LED_STRATEGY_8190	LedStrategy;  
	u8  txqueue_to_outpipemap[9];	
	u8  RtOutPipes[16];
	u8  RtInPipes[16];
	u8  ep_in_num;
	u8  ep_out_num;
	u8  ep_num;
	int irq;
	struct ieee80211_device *ieee80211;
	
	short card_8192; /* O: rtl8192, 1:rtl8185 V B/C, 2:rtl8185 V D */
	u8 card_8192_version; /* if TCR reports card V B/C this discriminates */
//	short phy_ver; /* meaningful for rtl8225 1:A 2:B 3:C */
	short enable_gpio0;
	enum card_type {PCI,MINIPCI,CARDBUS,USB}card_type;
	short hw_plcp_len;
	short plcp_preamble_mode;
		
	spinlock_t irq_lock;
//	spinlock_t irq_th_lock;
	spinlock_t tx_lock;
	spinlock_t ps_lock;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,16))
	struct semaphore mutex;
#else
        struct mutex mutex;
#endif
	spinlock_t rf_lock; //used to lock rf write operation added by wb

	u16 irq_mask;
//	short irq_enabled;
//	struct net_device *dev; //comment this out.
	short chan;
	short sens;
	short max_sens;

	
	//	u8 chtxpwr[15]; //channels from 1 to 14, 0 not used
//	u8 chtxpwr_ofdm[15]; //channels from 1 to 14, 0 not used
//	u8 cck_txpwr_base;
//	u8 ofdm_txpwr_base;
//	u8 challow[15]; //channels from 1 to 14, 0 not used
	short up;
	short crcmon; //if 1 allow bad crc frame reception in monitor mode
//	short prism_hdr;
	
//	struct timer_list scan_timer;
	/*short scanpending;
	short stopscan;*/
//	spinlock_t scan_lock;
//	u8 active_probe;
	//u8 active_scan_num;
	struct semaphore wx_sem;
	struct semaphore rf_sem; //used to lock rf write operation added by wb, modified by david
//	short hw_wep;
		
//	short digphy;
//	short antb;
//	short diversity;
//	u8 cs_treshold;
//	short rcr_csense;
	u8 rf_type; //0 means 1T2R, 1 means 2T4R
	RT_RF_TYPE_819xU rf_chip;
	
//	u32 key0[4];
	short (*rf_set_sens)(struct net_device *dev,short sens);
	u8 (*rf_set_chan)(struct net_device *dev,u8 ch);
	void (*rf_close)(struct net_device *dev);
	void (*rf_init)(struct net_device *dev);
	//short rate;
	short promisc;	
	/*stats*/
	struct Stats stats;
	struct iw_statistics wstats;
	struct proc_dir_entry *dir_dev;
	
	/*RX stuff*/
//	u32 *rxring;
//	u32 *rxringtail;
//	dma_addr_t rxringdma;
	struct urb **rx_urb;
	struct urb **rx_cmd_urb;

/* modified by davad for Rx process */
       struct sk_buff_head rx_queue;
       struct sk_buff_head skb_queue;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0))
	struct tq_struct qos_activate;
#else
       struct work_struct qos_activate;
#endif
	short  tx_urb_index;
	atomic_t tx_pending[0x10];//UART_PRIORITY+1


	struct tasklet_struct irq_rx_tasklet;
	struct urb *rxurb_task;

	//2 Tx Related variables
	u16	ShortRetryLimit;
	u16	LongRetryLimit;
	u32	TransmitConfig;
	u8	RegCWinMin;		// For turbo mode CW adaptive. Added by Annie, 2005-10-27.

	u32     LastRxDescTSFHigh;
	u32     LastRxDescTSFLow;


	//2 Rx Related variables
	u16	EarlyRxThreshold;
	u32	ReceiveConfig;
	u8	AcmControl;

	u8	RFProgType;
	
	u8 retry_data;
	u8 retry_rts;
	u16 rts;

	struct 	ChnlAccessSetting  ChannelAccessSetting;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,5,0)	
	struct work_struct reset_wq;
#else
	struct tq_struct reset_wq;
#endif

/**********************************************************/
	//for rtl819xUsb
	u16     basic_rate;
	u8      short_preamble;
	u8      slot_time;
	bool 	bDcut;
	bool bCurrentRxAggrEnable;
	u8 Rf_Mode; //add for Firmware RF -R/W switch
	prt_firmware		pFirmware;
	rtl819xUsb_loopback_e	LoopbackMode;
	firmware_source_e	firmware_source;
	bool usb_error;

	u16 EEPROMTxPowerDiff;
	u8 EEPROMThermalMeter;
	u8 EEPROMPwDiff;
	u8 EEPROMCrystalCap;
	u8 EEPROM_Def_Ver;
	u8 EEPROMTxPowerLevelCCK;// CCK channel 1~14
	u8 EEPROMTxPowerLevelCCK_V1[3];
	u8 EEPROMTxPowerLevelOFDM24G[3]; // OFDM 2.4G channel 1~14
	u8 EEPROMTxPowerLevelOFDM5G[24];	// OFDM 5G
	
//RTL8192SU
	bool	bDmDisableProtect;
	bool	bIgnoreDiffRateTxPowerOffset;

#ifdef EEPROM_OLD_FORMAT_SUPPORT
	u8  EEPROMTxPowerLevelCCK24G[14];		// CCK 2.4G channel 1~14
	//u8  EEPROMTxPowerLevelOFDM24G[14];	// OFDM 2.4G channel 1~14
	//u8  EEPROMTxPowerLevelOFDM5G[24];	// OFDM 5G
#else
	// For EEPROM TX Power Index like 8190 series
	u8  EEPROMRfACCKChnl1TxPwLevel[3];	//RF-A CCK Tx Power Level at channel 7
	u8  EEPROMRfAOfdmChnlTxPwLevel[3];//RF-A CCK Tx Power Level at [0],[1],[2] = channel 1,7,13
	u8  EEPROMRfCCCKChnl1TxPwLevel[3];	//RF-C CCK Tx Power Level at channel 7
	u8  EEPROMRfCOfdmChnlTxPwLevel[3];//RF-C CCK Tx Power Level at [0],[1],[2] = channel 1,7,13

	// F92S new definition
	//RF-A&B CCK/OFDM Tx Power Level at three channel are [1-3] [4-9] [10-14]
	u8  RfCckChnlAreaTxPwr[2][3];	
	u8  RfOfdmChnlAreaTxPwr1T[2][3];	
	u8  RfOfdmChnlAreaTxPwr2T[2][3];	
#endif

	// Add For EEPROM Efuse switch and  Efuse Shadow map Setting
	bool		EepromOrEfuse;
	bool		bBootFromEfuse;	// system boot form EFUSE
	u8  		EfuseMap[2][HWSET_MAX_SIZE_92S];
	
	u8  		EEPROMUsbOption;
	u8  		EEPROMUsbPhyParam[5];
	u8  		EEPROMTxPwrBase;
	u8  		EEPROMBoardType;
	bool		bBootFromEEPROM;   // system boot from EEPROM
	u8  		EEPROMTSSI_A;
	u8  		EEPROMTSSI_B;
	u8  		EEPROMHT2T_TxPwr[6];			// For channel 1, 7 and 13 on path A/B.
	u8  		EEPROMTxPwrTkMode;

	u8  		bTXPowerDataReadFromEEPORM;

	u8		EEPROMVersion;
	u8		EEPROMUsbEndPointNumber;
	
	bool		AutoloadFailFlag;
	u8	RfTxPwrLevelCck[2][14];
	u8	RfTxPwrLevelOfdm1T[2][14];
	u8	RfTxPwrLevelOfdm2T[2][14];
	// 2009/01/20 MH Add for new EEPROM format.
	u8					TxPwrHt20Diff[2][14];				// HT 20<->40 Pwr diff
	u8					TxPwrLegacyHtDiff[2][14];		// For HT<->legacy pwr diff
	u8					TxPwrbandEdgeHt40[2][2];		// Band edge for HY 40MHZlow/up channel
	u8					TxPwrbandEdgeHt20[2][2];		// Band edge for HY 40MHZ low/up channel
	u8					TxPwrbandEdgeLegacyOfdm[2][2];	// Band edge for legacy ofdm low/up channel
	u8					TxPwrbandEdgeFlag;				// Band edge enable flag

	// L1 and L2 high power threshold.
	u8 				MidHighPwrTHR_L1;
	u8 				MidHighPwrTHR_L2;
	u8				TxPwrSafetyFlag;				// for Tx power safety spec
//RTL8192SU

/*PHY related*/
	BB_REGISTER_DEFINITION_T	PHYRegDef[4];	//Radio A/B/C/D
	// Read/write are allow for following hardware information variables
#ifdef RTL8192SU
	u32	MCSTxPowerLevelOriginalOffset[7];//FIXLZM
#else
	u32	MCSTxPowerLevelOriginalOffset[6];
#endif
	u32	CCKTxPowerLevelOriginalOffset;
	u8	TxPowerLevelCCK[14];			// CCK channel 1~14
	u8	TxPowerLevelOFDM24G[14];		// OFDM 2.4G channel 1~14
	u8	TxPowerLevelOFDM5G[14];			// OFDM 5G
	u32	Pwr_Track;
	u8	TxPowerDiff;
	u8	AntennaTxPwDiff[2];				// Antenna gain offset, index 0 for B, 1 for C, and 2 for D
	u8	CrystalCap;						// CrystalCap.
	u8	ThermalMeter[2];				// ThermalMeter, index 0 for RFIC0, and 1 for RFIC1

	u8	CckPwEnl;
	// Use to calculate PWBD.
	u8	bCckHighPower;
	long	undecorated_smoothed_pwdb;

	//for set channel 
	u8	SwChnlInProgress;
	u8 	SwChnlStage;
	u8	SwChnlStep;
	u8	SetBWModeInProgress;
	HT_CHANNEL_WIDTH		CurrentChannelBW;
	u8      ChannelPlan;
	u8      pwrGroupCnt;
	// 8190 40MHz mode
	//
	u8	nCur40MhzPrimeSC;	// Control channel sub-carrier
	// Joseph test for shorten RF configuration time.
	// We save RF reg0 in this variable to reduce RF reading.
	//
	u32					RfReg0Value[4];
	u8 					NumTotalRFPath;	
	bool 				brfpath_rxenable[4];
	//RF set related
	bool				SetRFPowerStateInProgress;
//+by amy 080507
	struct timer_list watch_dog_timer;	

//+by amy 080515 for dynamic mechenism
	//Add by amy Tx Power Control for Near/Far Range 2008/05/15
	bool	bdynamic_txpower;  //bDynamicTxPower
	bool	bDynamicTxHighPower;  // Tx high power state
	bool	bDynamicTxLowPower;  // Tx low power state
	bool	bLastDTPFlag_High;
	bool	bLastDTPFlag_Low;
	
	bool	bstore_last_dtpflag;
	bool	bstart_txctrl_bydtp;   //Define to discriminate on High power State or on sitesuvey to change Tx gain index	
	//Add by amy for Rate Adaptive
	rate_adaptive rate_adaptive;
	//Add by amy for TX power tracking
	//2008/05/15  Mars OPEN/CLOSE TX POWER TRACKING
       txbbgain_struct txbbgain_table[TxBBGainTableLength];
       u8	EEPROMTxPowerTrackEnable;
	u8			   txpower_count;//For 6 sec do tracking again
	bool			   btxpower_trackingInit;
	u8			   OFDM_index;
	u8			   CCK_index;
	//2007/09/10 Mars Add CCK TX Power Tracking
	ccktxbbgain_struct	cck_txbbgain_table[CCKTxBBGainTableLength];
	ccktxbbgain_struct	cck_txbbgain_ch14_table[CCKTxBBGainTableLength];
	u8 rfa_txpowertrackingindex;
	u8 rfa_txpowertrackingindex_real;
	u8 rfa_txpowertracking_default;
	u8 rfc_txpowertrackingindex;
	u8 rfc_txpowertrackingindex_real;

	s8 cck_present_attentuation;
	u8 cck_present_attentuation_20Mdefault;
	u8 cck_present_attentuation_40Mdefault;
	char cck_present_attentuation_difference;
	bool btxpower_tracking;
	bool bcck_in_ch14;
	bool btxpowerdata_readfromEEPORM;
	u16 	TSSI_13dBm;
	//For Backup Initial Gain
	init_gain initgain_backup;
	u8 DefaultInitialGain[4];
	// For EDCA Turbo mode, Added by amy 080515.
	bool		bis_any_nonbepkts;
	bool		bcurrent_turbo_EDCA;
	bool		bis_cur_rdlstate;
	struct timer_list fsync_timer;
	bool bfsync_processing;	// 500ms Fsync timer is active or not
	u32 	rate_record;
	u32 	rateCountDiffRecord;
	u32	ContiuneDiffCount;
	bool bswitch_fsync;

	u8	framesync;
	u32 	framesyncC34;
	u8   	framesyncMonitor;
        	//Added by amy 080516  for RX related
	u16 	nrxAMPDU_size;
	u8 	nrxAMPDU_aggr_num;

	//by amy for gpio
	 bool bHwRadioOff;

	//by amy for reset_count
	u32 reset_count;
	bool bpbc_pressed;
	//by amy for debug
	u32 txpower_checkcnt;
	u32 txpower_tracking_callback_cnt;
	u8 thermal_read_val[40];
	u8 thermal_readback_index;
	u32 ccktxpower_adjustcnt_not_ch14;
	u32 ccktxpower_adjustcnt_ch14;
	u8 tx_fwinfo_force_subcarriermode;
	u8 tx_fwinfo_force_subcarrierval;
	//by amy for silent reset
	RESET_TYPE	ResetProgress;
	bool		bForcedSilentReset;
	bool		bDisableNormalResetCheck;
	u16		TxCounter;
	u16		RxCounter;
	int		IrpPendingCount;
	bool		bResetInProgress;
	bool		force_reset;
	u8		InitialGainOperateType;
	
	u16		SifsTime;
	
	//define work item by amy 080526
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,5,0)  
        
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)    
	struct delayed_work update_beacon_wq;
	struct delayed_work watch_dog_wq;    
	struct delayed_work txpower_tracking_wq;
	struct delayed_work rfpath_check_wq;
	struct delayed_work gpio_change_rf_wq;
	struct delayed_work initialgain_operate_wq;
#else
	struct work_struct update_beacon_wq;
	struct work_struct watch_dog_wq;
	struct work_struct txpower_tracking_wq;
	struct work_struct rfpath_check_wq;
	struct work_struct gpio_change_rf_wq;
	struct work_struct initialgain_operate_wq;
#endif
	struct workqueue_struct *priv_wq;
#else
	/* used for periodly scan */
	struct tq_struct update_beacon_wq;
	struct tq_struct txpower_tracking_wq;
	struct tq_struct rfpath_check_wq;
	struct tq_struct watch_dog_wq;
	struct tq_struct gpio_change_rf_wq;
	struct tq_struct initialgain_operate_wq;
#endif
//#ifdef RTL8192SU
	//lzm add for 8192S
	 u32 			IntrMask;
	// RF and BB access related synchronization flags.
	bool				bChangeBBInProgress; // BaseBand RW is still in progress.
	bool				bChangeRFInProgress; // RF RW is still in progress.

	u32				CCKTxPowerAdjustCntCh14;		//debug only
	u32				CCKTxPowerAdjustCntNotCh14;	//debug only
	u32				TXPowerTrackingCallbackCnt;		//debug only
	u32				TxPowerCheckCnt;				//debug only
	u32				RFWritePageCnt[3];				//debug only
	u32				RFReadPageCnt[3];				//debug only
	u8				ThermalReadBackIndex;			//debug only
	u8				ThermalReadVal[40];				//debug only

	// For HCT test, 2005.07.15, by rcnjko.
	// not realize true, just define it, set it 0 default, because some func use it 
	bool				bInHctTest;	

	// The current Tx Power Level
	u8				CurrentCckTxPwrIdx;
	u8				CurrentOfdm24GTxPwrIdx;

	// For pass 92S common phycfg.c compiler
	u8					TxPowerLevelCCK_A[14];			// RF-A, CCK channel 1~14
	u8					TxPowerLevelOFDM24G_A[14];	// RF-A, OFDM 2.4G channel 1~14 
	u8					TxPowerLevelCCK_C[14];			// RF-C, CCK channel 1~14
	u8					TxPowerLevelOFDM24G_C[14];	// RF-C, OFDM 2.4G channel 1~14
	u8					LegacyHTTxPowerDiff;			// Legacy to HT rate power diff
	char					RF_C_TxPwDiff;					// Antenna gain offset, rf-c to rf-a

	bool	bRFSiOrPi;//0=si, 1=pi.
	//lzm add for 8192S

	bool SetFwCmdInProgress; //is set FW CMD in Progress? 92S only
	u8 CurrentFwCmdIO;

	u8 MinSpaceCfg;

	u16 rf_pathmap;
//#endif


#ifdef USB_RX_AGGREGATION_SUPPORT
	bool		bCurrentRxAggrEnable;
	bool		bForcedUsbRxAggr;
	u32		ForcedUsbRxAggrInfo;
	u32		LastUsbRxAggrInfoSetting;
	u32		RegUsbRxAggrInfo;
#endif



}r8192_priv;

// for rtl8187
// now mirging to rtl8187B
/*
typedef enum{ 
	LOW_PRIORITY = 0x02,
	NORM_PRIORITY 
	} priority_t;
*/
//for rtl8187B
typedef enum{
	BULK_PRIORITY = 0x01,
	//RSVD0,
	//RSVD1,
	LOW_PRIORITY,
	NORM_PRIORITY, 
	VO_PRIORITY,
	VI_PRIORITY, //0x05
	BE_PRIORITY,
	BK_PRIORITY,
	RSVD2,
	RSVD3,
	BEACON_PRIORITY, //0x0A
	HIGH_PRIORITY,
	MANAGE_PRIORITY,
	RSVD4,
	RSVD5,
	UART_PRIORITY //0x0F
} priority_t;

#if 0
typedef enum{
	NIC_8192U = 1,
	NIC_8190P = 2,
	NIC_8192E = 3,
	NIC_8192SE = 4,
	NIC_8192SU = 5,
	} nic_t;
#endif

#if 0 //defined in Qos.h
//typedef u32 AC_CODING;
#define AC0_BE	0		// ACI: 0x00	// Best Effort
#define AC1_BK	1		// ACI: 0x01	// Background
#define AC2_VI	2		// ACI: 0x10	// Video
#define AC3_VO	3		// ACI: 0x11	// Voice
#define AC_MAX	4		// Max: define total number; Should not to be used as a real enum.

//
// ECWmin/ECWmax field.
// Ref: WMM spec 2.2.2: WME Parameter Element, p.13.
//
typedef	union _ECW{
	u8	charData;
	struct
	{
		u8	ECWmin:4;
		u8	ECWmax:4;
	}f;	// Field
}ECW, *PECW;

//
// ACI/AIFSN Field.
// Ref: WMM spec 2.2.2: WME Parameter Element, p.12.
//
typedef	union _ACI_AIFSN{
	u8	charData;
	
	struct
	{
		u8	AIFSN:4;
		u8	ACM:1;
		u8	ACI:2;
		u8	Reserved:1;
	}f;	// Field
}ACI_AIFSN, *PACI_AIFSN;

//
// AC Parameters Record Format.
// Ref: WMM spec 2.2.2: WME Parameter Element, p.12.
//
typedef	union _AC_PARAM{
	u32	longData;
	u8	charData[4];

	struct
	{
		ACI_AIFSN	AciAifsn;
		ECW		Ecw;
		u16		TXOPLimit;
	}f;	// Field
}AC_PARAM, *PAC_PARAM;

#endif
#ifdef JOHN_HWSEC
struct ssid_thread {
	struct net_device *dev;
       	u8 name[IW_ESSID_MAX_SIZE + 1];
};
#endif

#ifdef RTL8192SU
short rtl8192SU_tx_cmd(struct net_device *dev, struct sk_buff *skb);
short rtl8192SU_tx(struct net_device *dev, struct sk_buff* skb);
bool FirmwareDownload92S(struct net_device *dev);
#else
short rtl8192_tx(struct net_device *dev, struct sk_buff* skb);
bool init_firmware(struct net_device *dev);
#endif

short rtl819xU_tx_cmd(struct net_device *dev, struct sk_buff *skb);
short rtl8192_tx(struct net_device *dev, struct sk_buff* skb);

u32 read_cam(struct net_device *dev, u8 addr);
void write_cam(struct net_device *dev, u8 addr, u32 data);

u8 read_nic_byte(struct net_device *dev, int x);
u8 read_nic_byte_E(struct net_device *dev, int x);
u32 read_nic_dword(struct net_device *dev, int x);
u16 read_nic_word(struct net_device *dev, int x) ;
void write_nic_byte(struct net_device *dev, int x,u8 y);
void write_nic_byte_E(struct net_device *dev, int x,u8 y);
void write_nic_word(struct net_device *dev, int x,u16 y);
void write_nic_dword(struct net_device *dev, int x,u32 y);
void force_pci_posting(struct net_device *dev);

void rtl8192_rtx_disable(struct net_device *);
void rtl8192_rx_enable(struct net_device *);
void rtl8192_tx_enable(struct net_device *);

void rtl8192_disassociate(struct net_device *dev);
//void fix_rx_fifo(struct net_device *dev);
void rtl8185_set_rf_pins_enable(struct net_device *dev,u32 a);

void rtl8192_set_anaparam(struct net_device *dev,u32 a);
void rtl8185_set_anaparam2(struct net_device *dev,u32 a);
void rtl8192_update_msr(struct net_device *dev);
int rtl8192_down(struct net_device *dev);
int rtl8192_up(struct net_device *dev);
void rtl8192_commit(struct net_device *dev);
void rtl8192_set_chan(struct net_device *dev,short ch);
void write_phy(struct net_device *dev, u8 adr, u8 data);
void write_phy_cck(struct net_device *dev, u8 adr, u32 data);
void write_phy_ofdm(struct net_device *dev, u8 adr, u32 data);
void rtl8185_tx_antenna(struct net_device *dev, u8 ant);
void rtl8192_set_rxconf(struct net_device *dev);
//short check_nic_enough_desc(struct net_device *dev, priority_t priority);
extern void rtl819xusb_beacon_tx(struct net_device *dev,u16  tx_rate);
void CamResetAllEntry(struct net_device* dev);
void EnableHWSecurityConfig8192(struct net_device *dev);
void setKey(struct net_device *dev, u8 EntryNo, u8 KeyIndex, u16 KeyType, u8 *MacAddr, u8 DefaultKey, u32 *KeyContent );
short rtl8192_is_tx_queue_empty(struct net_device *dev);

#endif
