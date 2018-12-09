#include <rtthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <dfs_posix.h>
#include <finsh.h>
#include "libYmodem.h"

static rt_device_t uart=0;

/**
  * @brief  Receive byte from sender
  * @param  c: Character
  * @param  timeout: Timeout
  * @retval 0: Byte received
  *        -1: Timeout
  */
static  int32_t Receive_Byte (uint8_t *c, uint32_t timeout)
{
	if(uart==0)return -1;
		
	while (timeout-- > 0){
		if (rt_device_read(uart,0,c,1) == 1){
			return 0;
		}else{
			rt_thread_delay(1);
		}
	}
	return -1;
}

/**
  * @brief  Send a byte
  * @param  c: Character
  * @retval 0: Byte sent
  */
static uint32_t Send_Byte (uint8_t c)
{
	if(uart)
		rt_device_write(uart,0,&c,1);
	return 0;
}

/**
  * @brief  Receive a packet from sender
  * @param  data
  * @param  length
  * @param  timeout
  *     0: end of transmission
  *    -1: abort by sender
  *    >0: packet length
  * @retval 0: normally return
  *        -1: timeout or packet error
  *         1: abort by user
  */
static int32_t Receive_Packet (uint8_t *data, int32_t *length, uint32_t timeout)
{
  uint16_t i, packet_size;
  uint8_t c;
  *length = 0;
  if (Receive_Byte(&c, timeout) != 0)
  {
    return -1;
  }
  switch (c)
  {
    case SOH:
      packet_size = PACKET_SIZE;
      break;
    case STX:
      packet_size = PACKET_1K_SIZE;
      break;
    case EOT:
      return 0;
    case CA:
      if ((Receive_Byte(&c, timeout) == 0) && (c == CA))
      {
        *length = -1;
        return 0;
      }
      else
      {
        return -1;
      }
    case ABORT1:
    case ABORT2:
      return 1;
    default:
      return -1;
  }
  *data = c;
  for (i = 1; i < (packet_size + PACKET_OVERHEAD); i ++)
  {
    if (Receive_Byte(data + i, timeout) != 0)
    {
      return -1;
    }
  }
  if (data[PACKET_SEQNO_INDEX] != ((data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff))
  {
    return -1;
  }
  *length = packet_size;
  return 0;
}

/**
  * @brief  Receive a file using the ymodem protocol.
  * @param  buf: Address of the first byte.
  * @retval The size of the file.
  */
//#define DEBUG(x) write(dfp,x,sizeof(x));
#define DEBUG(x)
int32_t Ymodem_Receive (void)
{
	static rt_err_t (*lastRxFunc)(rt_device_t dev, rt_size_t size);
	{
		uart=0;
		uart=rt_console_get_device();
		if(uart){
			lastRxFunc=uart->rx_indicate;
			uart->rx_indicate=0;
		}else{
			return -10;
		}
	}
	int fp=-1;
	DEBUG("debug start\r\n");
	uint8_t *packet_data=0;
	packet_data=rt_malloc(PACKET_1K_SIZE + PACKET_OVERHEAD);
	if(packet_data==RT_NULL){
		DEBUG("packet data malloc failed\r\n");
		return 0;
	}
	char fileName[FILE_NAME_LENGTH],file_size[FILE_SIZE_LENGTH];
	uint8_t	*file_ptr;
	int32_t i, packet_length, session_done, file_done, packets_received, errors, session_begin, size = 0;
	for (session_done = 0, errors = 0, session_begin = 0; ;) {
		for (packets_received = 0, file_done = 0; ;) {
			switch (Receive_Packet(packet_data, &packet_length, NAK_TIMEOUT)) {
			case 0:
				errors = 0;
				switch (packet_length) {
					/* Abort by sender */
				case - 1:
					DEBUG("Abort by sender\r\n");
					size=0;
					Send_Byte(ACK);
					goto Ymodem_Receive_exit;
					//return 0;
					/* End of transmission */
				case 0:
					file_done = 1;
					Send_Byte(ACK);
					break;
					/* Normal packet */
				default:
					if ((packet_data[PACKET_SEQNO_INDEX] & 0xff) != (packets_received & 0xff)) {
						Send_Byte(NAK);
					} else {
						if (packets_received == 0) {
							/* Filename packet */
							if (packet_data[PACKET_HEADER] != 0) {
								/* Filename packet has valid data */
								for (i = 0, file_ptr = packet_data + PACKET_HEADER; (*file_ptr != 0) && (i < FILE_NAME_LENGTH);) {
									fileName[i++] = *file_ptr++;
								}
								fileName[i++] = '\0';
								for (i = 0, file_ptr ++; (*file_ptr != ' ') && (i < FILE_SIZE_LENGTH);) {
									file_size[i++] = *file_ptr++;
								}
								file_size[i++] = '\0';
								size=atoi(file_size);
								DEBUG("open new file\r\n");
								if(fp>=0)close(fp);
								fp=open(fileName,DFS_O_TRUNC|DFS_O_RDWR,0);
								//if file is invalid,send NACK
								if(fp<0){
									DEBUG("open new file failed\r\n");
									Send_Byte(CA);
									Send_Byte(CA);
									
								}else{
									DEBUG("open new file succ\r\n");
									//send ACK
									Send_Byte(ACK);
									Send_Byte(CRC16);
								}
							}
							/* Filename packet is empty, end session */
							else {
								DEBUG("recv new file succ\r\n");
								file_done = 1;
								session_done = 1;
								errors=0;
								close(fp);
								fp=-1;
								Send_Byte(ACK);
								break;
							}
						}
						/* Data packet */
						else {
							int length_calc=packet_length;
							if(fd_get(fp)->size + packet_length > size){
								length_calc=size - fd_get(fp)->size;
								if(length_calc<=0)length_calc=0;
							}
							if (write(fp,packet_data + PACKET_HEADER,length_calc)>=0) {
								DEBUG("write new packet succ\r\n");
								Send_Byte(ACK);
							} else { /* An error occurred while writing to Flash memory */
								/* End session */
								DEBUG("write new packet failed\r\n");
								Send_Byte(CA);
								Send_Byte(CA);
								size=-2;
								goto Ymodem_Receive_exit;
								//return -2;
							}
						}
						packets_received ++;
						session_begin = 1;
					}
				}
				break;
			case 1:
				Send_Byte(CA);
				Send_Byte(CA);
				size=-3;
				goto Ymodem_Receive_exit;
				//return -3;
			default:
				if (session_begin > 0) {
					DEBUG("error +1\r\n");
					errors ++;
				}
				if (errors > MAX_ERRORS) {
					Send_Byte(CA);
					Send_Byte(CA);
					DEBUG("max error exit\r\n");
					goto Ymodem_Receive_exit;
					//return 0;
				}
				Send_Byte(CRC16);
				break;
			}
			if (file_done != 0) {
				break;
			}
		}
		if (session_done != 0) {
			break;
		}
	}
Ymodem_Receive_exit:
	DEBUG("function exited\r\n");
	if(fp>=0)close(fp);
	if(packet_data)rt_free(packet_data);
	if(uart)uart->rx_indicate=lastRxFunc;
	{
		struct stat fstate;
		rt_kprintf("ymodem:ymodem_filesize=%d,error=%d\r\n",size,errors);
		stat(fileName,&fstate);
		rt_kprintf("ymodem:true_filesize=%d\r\n",fstate.st_size);
	}
	return (int32_t)size;
}
FINSH_FUNCTION_EXPORT_ALIAS(Ymodem_Receive,uploadFile,"upload file using ymodem")

/**
  * @brief  check response using the ymodem protocol
  * @param  buf: Address of the first byte
  * @retval The size of the file
  */
int32_t Ymodem_CheckResponse(uint8_t c)
{
  return 0;
}

/**
  * @brief  Prepare the first block
  * @param  timeout
  *     0: end of transmission
  * @retval None
  */
void Ymodem_PrepareIntialPacket(uint8_t *data, const uint8_t* fileName, uint32_t *length)
{
  uint16_t i, j;
  char file_ptr[10];
  
  /* Make first three packet */
  data[0] = SOH;
  data[1] = 0x00;
  data[2] = 0xff;
  
  /* Filename packet has valid data */
  for (i = 0; (fileName[i] != '\0') && (i < FILE_NAME_LENGTH);i++)
  {
     data[i + PACKET_HEADER] = fileName[i];
  }

  data[i + PACKET_HEADER] = 0x00;
  
  sprintf(file_ptr,"%d", *length);
  for (j =0, i = i + PACKET_HEADER + 1; file_ptr[j] != '\0' ; )
  {
     data[i++] = file_ptr[j++];
  }
  
  for (j = i; j < PACKET_SIZE + PACKET_HEADER; j++)
  {
    data[j] = 0;
  }
}

/**
  * @brief  Prepare the data packet
  * @param  timeout
  *     0: end of transmission
  * @retval None
  */
void Ymodem_PreparePacket(uint8_t *SourceBuf, uint8_t *data, uint8_t pktNo, uint32_t sizeBlk)
{
  uint16_t i, size, packetSize;
  uint8_t* file_ptr;
  
  /* Make first three packet */
  packetSize = sizeBlk >= PACKET_1K_SIZE ? PACKET_1K_SIZE : PACKET_SIZE;
  size = sizeBlk < packetSize ? sizeBlk :packetSize;
  if (packetSize == PACKET_1K_SIZE)
  {
     data[0] = STX;
  }
  else
  {
     data[0] = SOH;
  }
  data[1] = pktNo;
  data[2] = (~pktNo);
  file_ptr = SourceBuf;
  
  /* Filename packet has valid data */
  for (i = PACKET_HEADER; i < size + PACKET_HEADER;i++)
  {
     data[i] = *file_ptr++;
  }
  if ( size  <= packetSize)
  {
    for (i = size + PACKET_HEADER; i < packetSize + PACKET_HEADER; i++)
    {
      data[i] = 0x1A; /* EOF (0x1A) or 0x00 */
    }
  }
}

/**
  * @brief  Update CRC16 for input byte
  * @param  CRC input value 
  * @param  input byte
  * @retval None
  */
uint16_t UpdateCRC16(uint16_t crcIn, uint8_t byte)
{
  uint32_t crc = crcIn;
  uint32_t in = byte | 0x100;

  do
  {
    crc <<= 1;
    in <<= 1;
    if(in & 0x100)
      ++crc;
    if(crc & 0x10000)
      crc ^= 0x1021;
  }
  
  while(!(in & 0x10000));

  return crc & 0xffffu;
}


/**
  * @brief  Cal CRC16 for YModem Packet
  * @param  data
  * @param  length
  * @retval None
  */
uint16_t Cal_CRC16(const uint8_t* data, uint32_t size)
{
  uint32_t crc = 0;
  const uint8_t* dataEnd = data+size;

  while(data < dataEnd)
    crc = UpdateCRC16(crc, *data++);
 
  crc = UpdateCRC16(crc, 0);
  crc = UpdateCRC16(crc, 0);

  return crc&0xffffu;
}

/**
  * @brief  Cal Check sum for YModem Packet
  * @param  data
  * @param  length
  * @retval None
  */
uint8_t CalChecksum(const uint8_t* data, uint32_t size)
{
  uint32_t sum = 0;
  const uint8_t* dataEnd = data+size;

  while(data < dataEnd )
    sum += *data++;

  return (sum & 0xffu);
}

/**
  * @brief  Transmit a data packet using the ymodem protocol
  * @param  data
  * @param  length
  * @retval None
  */
void Ymodem_SendPacket(uint8_t *data, uint16_t length)
{
  uint16_t i;
  i = 0;
  while (i < length)
  {
    Send_Byte(data[i]);
    i++;
  }
}



