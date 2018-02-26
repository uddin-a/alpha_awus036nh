/**************************************************************************************************
 * Procedure:    Init boot code/firmware code/data session
 *
 * Description: This routine will intialize firmware. If any error occurs during the initialization 
 * 		process, the routine shall terminate immediately and return fail.
 *		NIC driver should call NdisOpenFile only from MiniportInitialize.	
 *
 * Arguments:   The pointer of the adapter

 * Returns:
 *        NDIS_STATUS_FAILURE - the following initialization process should be terminated
 *        NDIS_STATUS_SUCCESS - if firmware initialization process success
**************************************************************************************************/
//#include "ieee80211.h"
#if defined(RTL8192SE)||defined(RTL8192SU)
#include "r8192U.h"
#include "r8192S_firmware.h"
#include <linux/unistd.h>

#ifdef RTL8192SU
#include "r8192S_hw.h"
#include "r8192SU_HWImg.h"
//#include "r8192S_FwImgDTM.h"
#else
//#include "r8192U_hw.h"
#include "r8192xU_firmware_img.h"
#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,5,0)
#include <linux/firmware.h>
#endif

#define   byte(x,n)  ( (x >> (8 * n)) & 0xff  )

//
// Description:   This routine will intialize firmware. If any error occurs during the initialization 
//				process, the routine shall terminate immediately and return fail.
//
// Arguments:   The pointer of the adapter
//			   Code address (Virtual address, should fill descriptor with physical address)
//			   Code size
// Created by Roger, 2008.04.10.
// 
bool FirmwareDownloadCode(struct net_device *dev, u8 *	code_virtual_address,u32 buffer_len)
{
	struct r8192_priv   *priv = ieee80211_priv(dev);
	bool 		    rt_status = true;
	u16		    frag_threshold = MAX_FIRMWARE_CODE_SIZE; //Fragmentation might be required in 90/92 but not in 92S
	u16		    frag_length, frag_offset = 0;
	struct sk_buff	    *skb;
	unsigned char	    *seg_ptr;
	cb_desc		    *tcb_desc;	
	u8                  	    bLastIniPkt = 0;
	u16 			    ExtraDescOffset = 0;
	
#ifdef RTL8192SE
	fw_SetRQPN(dev);	// For 92SE only
#endif

	RT_TRACE(COMP_FIRMWARE, "--->FirmwareDownloadCode()\n" );	

	//MAX_TRANSMIT_BUFFER_SIZE
	if(buffer_len >= MAX_FIRMWARE_CODE_SIZE-USB_HWDESC_HEADER_LEN)
	{
		RT_TRACE(COMP_ERR, "Size over MAX_FIRMWARE_CODE_SIZE! \n");
		goto cmdsend_downloadcode_fail;
	}
	
	ExtraDescOffset = USB_HWDESC_HEADER_LEN;
	
	do {
		if((buffer_len-frag_offset) > frag_threshold)
		{
			frag_length = frag_threshold + ExtraDescOffset;
		}
		else
		{
			frag_length = (u16)(buffer_len - frag_offset + ExtraDescOffset);
		bLastIniPkt = 1;	
		}

		/* Allocate skb buffer to contain firmware info and tx descriptor info. */
		skb  = dev_alloc_skb(frag_length); 
		memcpy((unsigned char *)(skb->cb),&dev,sizeof(dev));
		
		tcb_desc = (cb_desc*)(skb->cb + MAX_DEV_ADDR_SIZE);
		tcb_desc->queue_index = TXCMD_QUEUE;
		tcb_desc->bCmdOrInit = DESC_PACKET_TYPE_INIT;
		tcb_desc->bLastIniPkt = bLastIniPkt;
		
		skb_reserve(skb, ExtraDescOffset);
		seg_ptr = (u8 *)skb_put(skb, (u32)(frag_length-ExtraDescOffset));
		memcpy(seg_ptr, code_virtual_address+frag_offset, (u32)(frag_length-ExtraDescOffset));
		
		tcb_desc->txbuf_size= frag_length;

		if(!priv->ieee80211->check_nic_enough_desc(dev,tcb_desc->queue_index)||
			(!skb_queue_empty(&priv->ieee80211->skb_waitQ[tcb_desc->queue_index]))||\
			(priv->ieee80211->queue_stop) ) 
		{
			RT_TRACE(COMP_FIRMWARE,"=====================================================> tx full!\n");
			skb_queue_tail(&priv->ieee80211->skb_waitQ[tcb_desc->queue_index], skb);
		} 
		else 
		{
			priv->ieee80211->softmac_hard_start_xmit(skb,dev);
		}
		
		frag_offset += (frag_length - ExtraDescOffset);

	}while(frag_offset < buffer_len);
	
	return rt_status ;


cmdsend_downloadcode_fail:	
	rt_status = false;
	RT_TRACE(COMP_ERR, "CmdSendDownloadCode fail !!\n");
	return rt_status;

}

#ifdef RTL8192SE
static void fw_SetRQPN(struct net_device *dev)
{	
	// Only for 92SE HW bug, we have to set RAPN before every FW download
	// We can remove the code later.
	write_nic_dword(dev,  RQPN, 0xffffffff);
	write_nic_dword(dev,  RQPN+4, 0xffffffff);
	write_nic_byte(dev,  RQPN+8, 0xff);
	write_nic_byte(dev,  RQPN+0xB, 0x80);
	//#if ((HAL_CODE_BASE ==  RTL8192_S) && (PLATFORM != PLATFORM_WINDOWS_USB))

}	/* fw_SetRQPN */
#endif	

RT_STATUS
FirmwareEnableCPU(struct net_device *dev)
{

	RT_STATUS	rtStatus = RT_STATUS_SUCCESS;
	u8		tmpU1b, CPUStatus = 0;
	u16		tmpU2b;	
	u32		iCheckTime = 200;	

	RT_TRACE(COMP_FIRMWARE, "-->FirmwareEnableCPU()\n" );
#ifdef RTL8192SE
	fw_SetRQPN(dev);	// For 92SE only
#endif	
	// Enable CPU.
	tmpU1b = read_nic_byte(dev, SYS_CLKR);
	write_nic_byte(dev,  SYS_CLKR, (tmpU1b|SYS_CPU_CLKSEL)); //AFE source

	tmpU2b = read_nic_word(dev, SYS_FUNC_EN);	
	write_nic_word(dev, SYS_FUNC_EN, (tmpU2b|FEN_CPUEN));

	//Polling IMEM Ready after CPU has refilled.
	do
	{
		CPUStatus = read_nic_byte(dev, TCR);
		if(CPUStatus& IMEM_RDY)
		{
			RT_TRACE(COMP_FIRMWARE, "IMEM Ready after CPU has refilled.\n");	
			break;		
		}

		//usleep(100);
		udelay(100);
	}while(iCheckTime--);

	if(!(CPUStatus & IMEM_RDY))
		return RT_STATUS_FAILURE;

	RT_TRACE(COMP_FIRMWARE, "<--FirmwareEnableCPU(): rtStatus(%#x)\n", rtStatus);
	return rtStatus;			
}

FIRMWARE_8192S_STATUS
FirmwareGetNextStatus(FIRMWARE_8192S_STATUS FWCurrentStatus)
{
	FIRMWARE_8192S_STATUS	NextFWStatus = 0;
	
	switch(FWCurrentStatus)
	{
		case FW_STATUS_INIT:
			NextFWStatus = FW_STATUS_LOAD_IMEM;
			break;

		case FW_STATUS_LOAD_IMEM:
			NextFWStatus = FW_STATUS_LOAD_EMEM;
			break;
		
		case FW_STATUS_LOAD_EMEM:
			NextFWStatus = FW_STATUS_LOAD_DMEM;
			break;

		case FW_STATUS_LOAD_DMEM:
			NextFWStatus = FW_STATUS_READY;
			break;

		default:
			RT_TRACE(COMP_ERR,"Invalid FW Status(%#x)!!\n", FWCurrentStatus);			
			break;
	}	
	return	NextFWStatus;
}

bool
FirmwareCheckReady(struct net_device *dev,	u8 LoadFWStatus)
{
	struct r8192_priv 	*priv = ieee80211_priv(dev);
	RT_STATUS	rtStatus = RT_STATUS_SUCCESS;
	rt_firmware	*pFirmware = priv->pFirmware;
	int			PollingCnt = 1000;
	//u8	 	tmpU1b, CPUStatus = 0;
	u8	 	CPUStatus = 0;
	u32		tmpU4b;
	//bool		bOrgIMREnable;	

	RT_TRACE(COMP_FIRMWARE, "--->FirmwareCheckReady(): LoadStaus(%d),", LoadFWStatus);

	pFirmware->FWStatus = (FIRMWARE_8192S_STATUS)LoadFWStatus;	
	if( LoadFWStatus == FW_STATUS_LOAD_IMEM)
	{				
		do
		{//Polling IMEM code done.
			CPUStatus = read_nic_byte(dev, TCR);
			if(CPUStatus& IMEM_CODE_DONE)
				break;	
			
			udelay(5);
		}while(PollingCnt--);		
		if(!(CPUStatus & IMEM_CHK_RPT) || PollingCnt <= 0)
		{
			RT_TRACE(COMP_ERR, "FW_STATUS_LOAD_IMEM FAIL CPU, Status=%x\r\n", CPUStatus);
			return false;			
		}
	}
	else if( LoadFWStatus == FW_STATUS_LOAD_EMEM)
	{//Check Put Code OK and Turn On CPU
		do
		{//Polling EMEM code done.
			CPUStatus = read_nic_byte(dev, TCR);
			if(CPUStatus& EMEM_CODE_DONE)
				break;
			
			udelay(5);
		}while(PollingCnt--);		
		if(!(CPUStatus & EMEM_CHK_RPT))
		{
			RT_TRACE(COMP_ERR, "FW_STATUS_LOAD_EMEM FAIL CPU, Status=%x\r\n", CPUStatus);
			return false;
		}

		// Turn On CPU
		rtStatus = FirmwareEnableCPU(dev);
		if(rtStatus != RT_STATUS_SUCCESS)
		{
			RT_TRACE(COMP_ERR, "Enable CPU fail ! \n" );
			return false;
		}		
	}
	else if( LoadFWStatus == FW_STATUS_LOAD_DMEM)
	{
		do
		{//Polling DMEM code done
			CPUStatus = read_nic_byte(dev, TCR);
			if(CPUStatus& DMEM_CODE_DONE)
				break;		
			
			udelay(5);
		}while(PollingCnt--);
		
		if(!(CPUStatus & DMEM_CODE_DONE))
		{
			RT_TRACE(COMP_ERR, "Polling  DMEM code done fail ! CPUStatus(%#x)\n", CPUStatus);
			return false;
		}

		RT_TRACE(COMP_FIRMWARE, "DMEM code download success, CPUStatus(%#x)\n", CPUStatus);		
                
//              PollingCnt = 100; // Set polling cycle to 10ms.
              PollingCnt = 10000; // Set polling cycle to 10ms.  
              
		do
		{//Polling Load Firmware ready
			CPUStatus = read_nic_byte(dev, TCR);
			if(CPUStatus & FWRDY)
				break;		
						
			udelay(100);
		}while(PollingCnt--);	
		
		RT_TRACE(COMP_FIRMWARE, "Polling Load Firmware ready, CPUStatus(%x)\n", CPUStatus);		

		//if(!(CPUStatus & LOAD_FW_READY))
		//if((CPUStatus & LOAD_FW_READY) != 0xff)
		if((CPUStatus & LOAD_FW_READY) != LOAD_FW_READY)
		{
			RT_TRACE(COMP_ERR, "Polling Load Firmware ready fail ! CPUStatus(%x)\n", CPUStatus);
			return false;
		}

	       //
              // <Roger_Notes> USB interface will update reserved followings parameters later!! 
              // 2008.08.28.
              //
#ifdef RTL8192SE
		//write_nic_dword(dev, RQPN, 0x10101010);
		//write_nic_byte(dev,  0xAB, 0x80);
#endif 

	       //
              // <Roger_Notes> If right here, we can set TCR/RCR to desired value 
              // and config MAC lookback mode to normal mode. 2008.08.28.
              //
              tmpU4b = read_nic_dword(dev,TCR);
		write_nic_dword(dev, TCR, (tmpU4b&(~TCR_ICV)));

		tmpU4b = read_nic_dword(dev, RCR);
		write_nic_dword(dev, RCR, 
			(tmpU4b|RCR_APPFCS|RCR_APP_ICV|RCR_APP_MIC));
		
		RT_TRACE(COMP_FIRMWARE, "FirmwareCheckReady(): Current RCR settings(%#x)\n", tmpU4b);
		
#if (defined (RTL8192SU_FPGA_2MAC_VERIFICATION) ||defined (RTL8192SU_ASIC_VERIFICATION))
#ifdef NOT_YET   //YJ,TMP		
		priv->TransmitConfig = read_nic_dword(dev, TCR);
		RT_TRACE(COMP_FIRMWARE, "FirmwareCheckReady(): Current TCR settings(%x)\n", priv->TransmitConfig);		
		pHalData->FwRsvdTxPageCfg = read_nic_byte(dev, FW_RSVD_PG_CRTL);
#endif		
#endif
		
		// Set to normal mode.
		write_nic_byte(dev, LBKMD_SEL, LBK_NORMAL);		

	}	

	RT_TRACE(COMP_FIRMWARE, "<---FirmwareCheckReady(): LoadFWStatus(%d), rtStatus(%x)\n", LoadFWStatus, rtStatus);
	return (rtStatus == RT_STATUS_SUCCESS) ? true:false;		
}

//
// Description:   This routine is to update the RF types in FW header partially.
//
// Created by Roger, 2008.12.24.
//
u8 FirmwareHeaderMapRfType(struct net_device *dev)
{	
	struct r8192_priv 	*priv = ieee80211_priv(dev);
	switch(priv->rf_type)
	{
		case RF_1T1R: 	return 0x11;
		case RF_1T2R: 	return 0x12;
		case RF_2T2R: 	return 0x22;
		case RF_2T2R_GREEN: 	return 0x92;
		default:
			RT_TRACE(COMP_INIT, "Unknown RF type(%x)\n",priv->rf_type);
			break;
	}
	return 0x22;
}


//
// Description:   This routine is to update the private parts in FW header partially.
//
// Created by Roger, 2008.12.18.
//
void FirmwareHeaderPriveUpdate(struct net_device *dev, PRT_8192S_FIRMWARE_PRIV 	pFwPriv)
{
	struct r8192_priv 	*priv = ieee80211_priv(dev);
#ifdef RTL8192SU
	// Update USB endpoint number for RQPN settings.
	pFwPriv->usb_ep_num = priv->EEPROMUsbEndPointNumber; // endpoint number: 4, 6 and 11.
	RT_TRACE(COMP_INIT, "FirmwarePriveUpdate(): usb_ep_num(%#x)\n", pFwPriv->usb_ep_num);
#endif

	// Update RF types for RATR settings.
	pFwPriv->rf_config = FirmwareHeaderMapRfType(dev);
}



bool FirmwareDownload92S(struct net_device *dev)
{	
	struct r8192_priv 	*priv = ieee80211_priv(dev);
	bool				rtStatus = true;
	const char 		*pFwImageFileName[1] = {"RTL8192SU/rtl8192sfw.bin"};
	u8				*pucMappedFile = NULL;
	u32				ulFileLength, ulInitStep = 0;	
	u8				FwHdrSize = RT_8192S_FIRMWARE_HDR_SIZE;	
	rt_firmware		*pFirmware = priv->pFirmware;
	u8				FwStatus = FW_STATUS_INIT;
	PRT_8192S_FIRMWARE_HDR		pFwHdr = NULL;
	PRT_8192S_FIRMWARE_PRIV		pFwPriv = NULL;
	int 				rc;
	const struct firmware 	*fw_entry;
	u32				file_length = 0;
	
	pFirmware->FWStatus = FW_STATUS_INIT;

	RT_TRACE(COMP_FIRMWARE, " --->FirmwareDownload92S()\n");

	//3//
	//3 //<1> Open Image file, and map file to contineous memory if open file success.
	//3  //        or read image file from array. Default load from BIN file
	//3//
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
	priv->firmware_source = FW_SOURCE_HEADER_FILE;
#else
	priv->firmware_source = FW_SOURCE_IMG_FILE;// We should decided by Reg.
#endif

	switch( priv->firmware_source )
	{
		case FW_SOURCE_IMG_FILE:		
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,5,0)		
			if(pFirmware->szFwTmpBufferLen == 0)
			{

				rc = request_firmware(&fw_entry, pFwImageFileName[ulInitStep],&priv->udev->dev);//===>1
				if(rc < 0 ) {
					RT_TRACE(COMP_ERR, "request firmware fail!\n");
					goto DownloadFirmware_Fail;
				} 

				if(fw_entry->size > sizeof(pFirmware->szFwTmpBuffer)) 
				{
					RT_TRACE(COMP_ERR, "img file size exceed the container buffer fail!\n");
					release_firmware(fw_entry);
					goto DownloadFirmware_Fail;
				}

				memcpy(pFirmware->szFwTmpBuffer,fw_entry->data,fw_entry->size);
				pFirmware->szFwTmpBufferLen = fw_entry->size;
				release_firmware(fw_entry);

				pucMappedFile = pFirmware->szFwTmpBuffer;
				file_length = pFirmware->szFwTmpBufferLen;

				//Retrieve FW header.
				pFirmware->pFwHeader = (PRT_8192S_FIRMWARE_HDR) pucMappedFile;
				pFwHdr = pFirmware->pFwHeader;
				RT_TRACE(COMP_FIRMWARE,"signature:%x, version:%x, size:%x, imemsize:%x, sram size:%x\n", \
						pFwHdr->Signature, pFwHdr->Version, pFwHdr->DMEMSize, \
						pFwHdr->IMG_IMEM_SIZE, pFwHdr->IMG_SRAM_SIZE);
				pFirmware->FirmwareVersion =  byte(pFwHdr->Version ,0);
				if ((pFwHdr->IMG_IMEM_SIZE==0) || (pFwHdr->IMG_IMEM_SIZE > sizeof(pFirmware->FwIMEM)))
				{
					RT_TRACE(COMP_ERR, "%s: memory for data image is less than IMEM required\n",\
							__FUNCTION__);
					goto DownloadFirmware_Fail;
				} else {
					pucMappedFile+=FwHdrSize;

					//Retrieve IMEM image.
					memcpy(pFirmware->FwIMEM, pucMappedFile, pFwHdr->IMG_IMEM_SIZE);
					pFirmware->FwIMEMLen = pFwHdr->IMG_IMEM_SIZE;
				}

				if (pFwHdr->IMG_SRAM_SIZE > sizeof(pFirmware->FwEMEM))
				{
					RT_TRACE(COMP_ERR, "%s: memory for data image is less than EMEM required\n",\
							__FUNCTION__);
					goto DownloadFirmware_Fail;
				} else {
					pucMappedFile += pFirmware->FwIMEMLen;

					/* Retriecve EMEM image */
					memcpy(pFirmware->FwEMEM, pucMappedFile, pFwHdr->IMG_SRAM_SIZE);//===>6 
					pFirmware->FwEMEMLen = pFwHdr->IMG_SRAM_SIZE;
				}


			}
#endif
			break;	

		case FW_SOURCE_HEADER_FILE:	
#if 1
#define Rtl819XFwImageArray Rtl8192SUFwImgArray
			//2008.11.10 Add by tynli.
			pucMappedFile = Rtl819XFwImageArray;
			ulFileLength = ImgArrayLength;

			RT_TRACE(COMP_INIT,"Fw download from header.\n");
			/* Retrieve FW header*/
			pFirmware->pFwHeader = (PRT_8192S_FIRMWARE_HDR) pucMappedFile;
			pFwHdr = pFirmware->pFwHeader;
			RT_TRACE(COMP_FIRMWARE,"signature:%x, version:%x, size:%x, imemsize:%x, sram size:%x\n", \
					pFwHdr->Signature, pFwHdr->Version, pFwHdr->DMEMSize, \
					pFwHdr->IMG_IMEM_SIZE, pFwHdr->IMG_SRAM_SIZE);
			pFirmware->FirmwareVersion =  byte(pFwHdr->Version ,0);

			if ((pFwHdr->IMG_IMEM_SIZE==0) || (pFwHdr->IMG_IMEM_SIZE > sizeof(pFirmware->FwIMEM)))
			{
				printk("FirmwareDownload92S(): memory for data image is less than IMEM required\n");	
				goto DownloadFirmware_Fail;
			} else {				
				pucMappedFile+=FwHdrSize;
				//Retrieve IMEM image.
				memcpy(pFirmware->FwIMEM, pucMappedFile, pFwHdr->IMG_IMEM_SIZE);
				pFirmware->FwIMEMLen = pFwHdr->IMG_IMEM_SIZE;				
			}

			if (pFwHdr->IMG_SRAM_SIZE > sizeof(pFirmware->FwEMEM))
			{
				printk(" FirmwareDownload92S(): memory for data image is less than EMEM required\n");					
				goto DownloadFirmware_Fail;
			} else {	
				pucMappedFile+= pFirmware->FwIMEMLen;

				//Retriecve EMEM image.
				memcpy(pFirmware->FwEMEM, pucMappedFile, pFwHdr->IMG_SRAM_SIZE);
				pFirmware->FwEMEMLen = pFwHdr->IMG_SRAM_SIZE;		
			}
#endif
			break;
		default:
			break;
	}			

	FwStatus = FirmwareGetNextStatus(pFirmware->FWStatus);
	while(FwStatus!= FW_STATUS_READY)
	{
		// Image buffer redirection.
		switch(FwStatus)
		{
			case FW_STATUS_LOAD_IMEM:				
				pucMappedFile = pFirmware->FwIMEM;
				ulFileLength = pFirmware->FwIMEMLen;					
				break;

			case FW_STATUS_LOAD_EMEM:				
				pucMappedFile = pFirmware->FwEMEM;
				ulFileLength = pFirmware->FwEMEMLen;					
				break;

			case FW_STATUS_LOAD_DMEM:			
				/* <Roger_Notes> Partial update the content of header private. 2008.12.18 */
                                pFwHdr = pFirmware->pFwHeader;
                                pFwPriv = (PRT_8192S_FIRMWARE_PRIV)&pFwHdr->FWPriv;
				FirmwareHeaderPriveUpdate(dev, pFwPriv);
				pucMappedFile = (u8*)(pFirmware->pFwHeader)+RT_8192S_FIRMWARE_HDR_EXCLUDE_PRI_SIZE;
				ulFileLength = FwHdrSize-RT_8192S_FIRMWARE_HDR_EXCLUDE_PRI_SIZE;
				break;
					
			default:
				RT_TRACE(COMP_ERR, "Unexpected Download step!!\n");
				goto DownloadFirmware_Fail;
				break;
		}		
		
		//3//
		//3// <2> Download image file 
	//3	//
		rtStatus = FirmwareDownloadCode(dev, pucMappedFile, ulFileLength);	
		
		if(rtStatus != true)
		{
			RT_TRACE(COMP_ERR, "FirmwareDownloadCode() fail ! \n" );
			goto DownloadFirmware_Fail;
		}	
		
		//3//
		//3// <3> Check whether load FW process is ready 
	//3	// 		
		rtStatus = FirmwareCheckReady(dev, FwStatus);

		if(rtStatus != true)
		{
			RT_TRACE(COMP_ERR, "FirmwareDownloadCode() fail ! \n");
			goto DownloadFirmware_Fail;
		}
		
		FwStatus = FirmwareGetNextStatus(pFirmware->FWStatus);
	}

	RT_TRACE(COMP_FIRMWARE, "Firmware Download Success!!\n");	
	return rtStatus;	
	
	DownloadFirmware_Fail:	
	RT_TRACE(COMP_ERR, "Firmware Download Fail!!%x\n",read_nic_word(dev, TCR));	
	rtStatus = false;
	return rtStatus;	
}
#else
void firmware_init_param(struct net_device *dev)
{
	struct r8192_priv 	*priv = ieee80211_priv(dev);
	rt_firmware		*pfirmware = priv->pFirmware;

	pfirmware->cmdpacket_frag_thresold = GET_COMMAND_PACKET_FRAG_THRESHOLD(MAX_TRANSMIT_BUFFER_SIZE);
}

/*
 * segment the img and use the ptr and length to remember info on each segment
 *
 */
bool fw_download_code(struct net_device *dev, u8 *code_virtual_address, u32 buffer_len)
{
	struct r8192_priv   *priv = ieee80211_priv(dev);
	bool 		    rt_status = true;
	//u16		    frag_threshold;
	u16		    frag_length, frag_offset = 0;
	//u16		    total_size;
	int		    i;
	
	//rt_firmware	    *pfirmware = priv->pFirmware;
	struct sk_buff	    *skb;
	unsigned char	    *seg_ptr;
	cb_desc		    *tcb_desc;	
	u8                  bLastIniPkt;
#ifdef RTL8192SE
	fw_SetRQPN(dev);	// For 92SE only
#endif

#ifndef RTL8192SU	
	if(buffer_len >= 64000-USB_HWDESC_HEADER_LEN)
	{
		return rt_status;
	}
	firmware_init_param(dev);
	//Fragmentation might be required
	frag_threshold = pfirmware->cmdpacket_frag_thresold;
#endif

	do {
#ifndef RTL8192SU 
		if((buffer_len - frag_offset) > frag_threshold) {
			frag_length = frag_threshold ;
			bLastIniPkt = 0;

		} else 
#endif
		{
			frag_length = buffer_len - frag_offset;
			bLastIniPkt = 1;

		}

		/* Allocate skb buffer to contain firmware info and tx descriptor info 
		 * add 4 to avoid packet appending overflow.
		 * */
		#ifdef RTL8192U
		skb  = dev_alloc_skb(USB_HWDESC_HEADER_LEN + frag_length + 4); 
		#else
		skb  = dev_alloc_skb(frag_length + 4); 
		#endif
		memcpy((unsigned char *)(skb->cb),&dev,sizeof(dev));
		tcb_desc = (cb_desc*)(skb->cb + MAX_DEV_ADDR_SIZE);
		tcb_desc->queue_index = TXCMD_QUEUE;
		tcb_desc->bCmdOrInit = DESC_PACKET_TYPE_INIT;
		tcb_desc->bLastIniPkt = bLastIniPkt;

		#ifdef RTL8192U
		skb_reserve(skb, USB_HWDESC_HEADER_LEN);
		#endif
		seg_ptr = skb->data;
		/* 
		 * Transform from little endian to big endian 
                 * and pending  zero
		 */
		for(i=0 ; i < frag_length; i+=4) {
			*seg_ptr++ = ((i+0)<frag_length)?code_virtual_address[i+3]:0;
			*seg_ptr++ = ((i+1)<frag_length)?code_virtual_address[i+2]:0;
			*seg_ptr++ = ((i+2)<frag_length)?code_virtual_address[i+1]:0;
			*seg_ptr++ = ((i+3)<frag_length)?code_virtual_address[i+0]:0;
		}
		tcb_desc->txbuf_size= (u16)i;
		skb_put(skb, i);

		if(!priv->ieee80211->check_nic_enough_desc(dev,tcb_desc->queue_index)||
			(!skb_queue_empty(&priv->ieee80211->skb_waitQ[tcb_desc->queue_index]))||\
			(priv->ieee80211->queue_stop) ) {
			RT_TRACE(COMP_FIRMWARE,"=====================================================> tx full!\n");
			skb_queue_tail(&priv->ieee80211->skb_waitQ[tcb_desc->queue_index], skb);
		} else {
			priv->ieee80211->softmac_hard_start_xmit(skb,dev);
		}
		
		code_virtual_address += frag_length;
		frag_offset += frag_length;

	}while(frag_offset < buffer_len);
	
	return rt_status;

#if 0
cmdsend_downloadcode_fail:	
	rt_status = false;
	RT_TRACE(COMP_ERR, "CmdSendDownloadCode fail !!\n");
	return rt_status;
#endif
}

bool
fwSendNullPacket(
	struct net_device *dev,
	u32			Length
)
{
	bool	rtStatus = true;
	struct r8192_priv   *priv = ieee80211_priv(dev);
	struct sk_buff	    *skb;
	cb_desc		    *tcb_desc;	
	unsigned char	    *ptr_buf;
	bool	bLastInitPacket = false;
	
	//PlatformAcquireSpinLock(dev, RT_TX_SPINLOCK);

	//Get TCB and local buffer from common pool. (It is shared by CmdQ, MgntQ, and USB coalesce DataQ)
	skb  = dev_alloc_skb(Length+ 4);
	memcpy((unsigned char *)(skb->cb),&dev,sizeof(dev));
	tcb_desc = (cb_desc*)(skb->cb + MAX_DEV_ADDR_SIZE);
	tcb_desc->queue_index = TXCMD_QUEUE;
	tcb_desc->bCmdOrInit = DESC_PACKET_TYPE_INIT;
	tcb_desc->bLastIniPkt = bLastInitPacket;
	ptr_buf = skb_put(skb, Length);
	memset(ptr_buf,0,Length);
	tcb_desc->txbuf_size= (u16)Length;

	if(!priv->ieee80211->check_nic_enough_desc(dev,tcb_desc->queue_index)||
			(!skb_queue_empty(&priv->ieee80211->skb_waitQ[tcb_desc->queue_index]))||\
			(priv->ieee80211->queue_stop) ) {
			RT_TRACE(COMP_FIRMWARE,"===================NULL packet==================================> tx full!\n");
			skb_queue_tail(&priv->ieee80211->skb_waitQ[tcb_desc->queue_index], skb);
		} else {
			priv->ieee80211->softmac_hard_start_xmit(skb,dev);
		}
	
	//PlatformReleaseSpinLock(dev, RT_TX_SPINLOCK);	
	return rtStatus;
}

//-----------------------------------------------------------------------------
// Procedure:    Check whether main code is download OK. If OK, turn on CPU
//
// Description:   CPU register locates in different page against general register. 
//			    Switch to CPU register in the begin and switch back before return				
//				
//				
// Arguments:   The pointer of the dev
//        
// Returns:
//        NDIS_STATUS_FAILURE - the following initialization process should be terminated
//        NDIS_STATUS_SUCCESS - if firmware initialization process success
//-----------------------------------------------------------------------------
bool CPUcheck_maincodeok_turnonCPU(struct net_device *dev)
{
	bool		rt_status = true;
	int		check_putcodeOK_time = 200000, check_bootOk_time = 200000;
	u32	 	CPU_status = 0;

	/* Check whether put code OK */
	do {
		CPU_status = read_nic_dword(dev, CPU_GEN);

		if(CPU_status&CPU_GEN_PUT_CODE_OK)
			break;

	}while(check_putcodeOK_time--);

	if(!(CPU_status&CPU_GEN_PUT_CODE_OK)) {
		RT_TRACE(COMP_ERR, "Download Firmware: Put code fail!\n");
		goto CPUCheckMainCodeOKAndTurnOnCPU_Fail;
	} else {
		RT_TRACE(COMP_FIRMWARE, "Download Firmware: Put code ok!\n");
	}

	/* Turn On CPU */
	CPU_status = read_nic_dword(dev, CPU_GEN);
	write_nic_byte(dev, CPU_GEN, (u8)((CPU_status|CPU_GEN_PWR_STB_CPU)&0xff));
	mdelay(1000);

	/* Check whether CPU boot OK */
	do {
		CPU_status = read_nic_dword(dev, CPU_GEN);

		if(CPU_status&CPU_GEN_BOOT_RDY)
			break;
	}while(check_bootOk_time--);

	if(!(CPU_status&CPU_GEN_BOOT_RDY)) {
		goto CPUCheckMainCodeOKAndTurnOnCPU_Fail;
	} else {
		RT_TRACE(COMP_FIRMWARE, "Download Firmware: Boot ready!\n");
	}

	return rt_status;

CPUCheckMainCodeOKAndTurnOnCPU_Fail:	
	RT_TRACE(COMP_ERR, "ERR in %s()\n", __FUNCTION__);
	rt_status = FALSE;
	return rt_status;
}

bool CPUcheck_firmware_ready(struct net_device *dev)
{

	bool		rt_status = true;
	int		check_time = 200000;
	u32		CPU_status = 0;

	/* Check Firmware Ready */
	do {
		CPU_status = read_nic_dword(dev, CPU_GEN);

		if(CPU_status&CPU_GEN_FIRM_RDY)
			break;
		
	}while(check_time--);

	if(!(CPU_status&CPU_GEN_FIRM_RDY))
		goto CPUCheckFirmwareReady_Fail;
	else
		RT_TRACE(COMP_FIRMWARE, "Download Firmware: Firmware ready!\n");

	return rt_status;
	
CPUCheckFirmwareReady_Fail:
	RT_TRACE(COMP_ERR, "ERR in %s()\n", __FUNCTION__);
	rt_status = false;
	return rt_status;

}

bool init_firmware(struct net_device *dev)
{
	struct r8192_priv 	*priv = ieee80211_priv(dev);
	bool			rt_status = TRUE;

	u8			*firmware_img_buf[3] = { &rtl8190_fwboot_array[0], 
						   	 &rtl8190_fwmain_array[0],
						   	 &rtl8190_fwdata_array[0]};

	u32			firmware_img_len[3] = { sizeof(rtl8190_fwboot_array), 
						   	sizeof(rtl8190_fwmain_array),
						   	sizeof(rtl8190_fwdata_array)};
	u32			file_length = 0;
	u8			*mapped_file = NULL;
	u32			init_step = 0;
	opt_rst_type_e	rst_opt = OPT_SYSTEM_RESET;
	firmware_init_step_e 	starting_state = FW_INIT_STEP0_BOOT;

	rt_firmware		*pfirmware = priv->pFirmware;
	const struct firmware 	*fw_entry;
	const char *fw_name[3] = { "RTL8192U/boot.img",
                           "RTL8192U/main.img",
			   "RTL8192U/data.img"};
	int rc;

	RT_TRACE(COMP_FIRMWARE, " PlatformInitFirmware()==>\n");

	if (pfirmware->firmware_status == FW_STATUS_0_INIT ) { 
		/* it is called by reset */
		rst_opt = OPT_SYSTEM_RESET;
		starting_state = FW_INIT_STEP0_BOOT;
		// TODO: system reset

	}else if(pfirmware->firmware_status == FW_STATUS_5_READY) {
		/* it is called by Initialize */
		rst_opt = OPT_FIRMWARE_RESET;
		starting_state = FW_INIT_STEP2_DATA;
	}else {
		 RT_TRACE(COMP_FIRMWARE, "PlatformInitFirmware: undefined firmware state\n");
	}
	
	/*
	 * Download boot, main, and data image for System reset. 
	 * Download data image for firmware reseta
	 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
	priv->firmware_source = FW_SOURCE_HEADER_FILE;
#else
	priv->firmware_source = FW_SOURCE_IMG_FILE;
#endif
	for(init_step = starting_state; init_step <= FW_INIT_STEP2_DATA; init_step++) {
		/*
		 * Open Image file, and map file to contineous memory if open file success.
		 * or read image file from array. Default load from IMG file
		 */
		if(rst_opt == OPT_SYSTEM_RESET) {
			switch(priv->firmware_source) {
				case FW_SOURCE_IMG_FILE:
				#if LINUX_VERSION_CODE > KERNEL_VERSION(2,5,0)
					rc = request_firmware(&fw_entry, fw_name[init_step],&priv->udev->dev);
					if(rc < 0 ) {
						RT_TRACE(COMP_ERR, "request firmware fail!\n");
						goto download_firmware_fail;
					} 
					
					if(fw_entry->size > sizeof(pfirmware->firmware_buf)) {
						RT_TRACE(COMP_ERR, "img file size exceed the container buffer fail!\n");
						goto download_firmware_fail;
					}

					if(init_step != FW_INIT_STEP1_MAIN) {
						memcpy(pfirmware->firmware_buf,fw_entry->data,fw_entry->size);
						mapped_file = pfirmware->firmware_buf;
						file_length = fw_entry->size;
					} else {
					#ifdef RTL8190P
						memcpy(pfirmware->firmware_buf,fw_entry->data,fw_entry->size);
						mapped_file = pfirmware->firmware_buf;
						file_length = fw_entry->size;
					#else
						memset(pfirmware->firmware_buf,0,128);
						memcpy(&pfirmware->firmware_buf[128],fw_entry->data,fw_entry->size);
						mapped_file = pfirmware->firmware_buf;
						file_length = fw_entry->size + 128;
					#endif
					}
					pfirmware->firmware_buf_size = file_length;
					#endif
					break;	

				case FW_SOURCE_HEADER_FILE:
					mapped_file =  firmware_img_buf[init_step];
					file_length  = firmware_img_len[init_step];
					if(init_step == FW_INIT_STEP2_DATA) {
						memcpy(pfirmware->firmware_buf, mapped_file, file_length);
						pfirmware->firmware_buf_size = file_length;
					}
					break;

				default:
					break;
			}


		}else if(rst_opt == OPT_FIRMWARE_RESET ) {
			/* we only need to download data.img here */
			mapped_file = pfirmware->firmware_buf;
			file_length = pfirmware->firmware_buf_size;
		}

		/* Download image file */
		/* The firmware download process is just as following,
		 * 1. that is each packet will be segmented and inserted to the wait queue.
		 * 2. each packet segment will be put in the skb_buff packet.
		 * 3. each skb_buff packet data content will already include the firmware info 
		 *   and Tx descriptor info
		 * */
		rt_status = fw_download_code(dev,mapped_file,file_length);
		#if LINUX_VERSION_CODE > KERNEL_VERSION(2,5,0)
		if(rst_opt == OPT_SYSTEM_RESET) {
			release_firmware(fw_entry);
		}
		#endif

		if(rt_status != TRUE) {
			goto download_firmware_fail;
		}

		switch(init_step) {
			case FW_INIT_STEP0_BOOT:
				/* Download boot
				 * initialize command descriptor.  
				 * will set polling bit when firmware code is also configured
				 */
				pfirmware->firmware_status = FW_STATUS_1_MOVE_BOOT_CODE;
#ifdef RTL8190P
				// To initialize IMEM, CPU move code  from 0x80000080, hence, we send 0x80 byte packet
				rt_status = fwSendNullPacket(dev, RTL8190_CPU_START_OFFSET);
				if(rt_status != true)
				{
					RT_TRACE(COMP_INIT, "fwSendNullPacket() fail ! \n");
					goto  download_firmware_fail;						
				}
#endif
				//mdelay(1000);
				/* 
				 * To initialize IMEM, CPU move code  from 0x80000080, 
				 * hence, we send 0x80 byte packet 
				 */
				break;

			case FW_INIT_STEP1_MAIN:
				/* Download firmware code. Wait until Boot Ready and Turn on CPU */
				pfirmware->firmware_status = FW_STATUS_2_MOVE_MAIN_CODE;

				/* Check Put Code OK and Turn On CPU */
				rt_status = CPUcheck_maincodeok_turnonCPU(dev);
				if(rt_status != TRUE) {	
					RT_TRACE(COMP_ERR, "CPUcheck_maincodeok_turnonCPU fail!\n");
					goto download_firmware_fail;
				}

				pfirmware->firmware_status = FW_STATUS_3_TURNON_CPU;
				break;

			case FW_INIT_STEP2_DATA:
				/* download initial data code */
				pfirmware->firmware_status = FW_STATUS_4_MOVE_DATA_CODE;
				mdelay(1);

				rt_status = CPUcheck_firmware_ready(dev);
				if(rt_status != TRUE) {				
					RT_TRACE(COMP_ERR, "CPUcheck_firmware_ready fail(%d)!\n",rt_status);
					goto download_firmware_fail;
				}

				/* wait until data code is initialized ready.*/
				pfirmware->firmware_status = FW_STATUS_5_READY;
				break;
		}
	}

	RT_TRACE(COMP_FIRMWARE, "Firmware Download Success\n");
	//assert(pfirmware->firmware_status == FW_STATUS_5_READY, ("Firmware Download Fail\n"));

	return rt_status;	

download_firmware_fail:	
	RT_TRACE(COMP_ERR, "ERR in %s()\n", __FUNCTION__);
	rt_status = FALSE;
	return rt_status;

}
#endif

