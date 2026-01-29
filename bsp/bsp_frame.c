#include "bsp_frame.h"
#include "bsp_timer.h"

volatile uint16_t data_count = 0;
uint8_t  rx_fifo_buf[RX_FIFO_SIZE] = {0};
volatile int16_t  rx_fifo_start;
volatile uint16_t rx_fifo_end;

extern volatile uint16_t time_sec;
uint16_t test_time = 3599;
extern uint8_t timer_state;

uint32_t bsp_create_frame(uint8_t *msg_buf, uint8_t *tx_buf, uint32_t tx_data_len, func_code_t func_code)
{
	uint32_t index = 0;

	msg_buf[index++] = FRAME_HEAD;

	msg_buf[index++] = func_code & 0xFF;

	msg_buf[index++] = tx_data_len & 0xFF;

	for(uint32_t i = 0; i < tx_data_len; i++)
	{
			msg_buf[index++] = *tx_buf;
	}

	uint16_t crc16 = CRC16_CalcBufCrc(msg_buf, index);
	msg_buf[index++] = (uint8_t)(crc16 >> 8) & 0xFF;
	msg_buf[index++] = (uint8_t)(crc16 >> 0) & 0xFF;

	msg_buf[index++] = FRAME_END;

	return index;
}

bool resolve_frame(uint8_t data, rx_msg_frame_t *rx_msg)
{
	static frame_t frame_state = STATE_WAIT_HEAD;
	static uint16_t rx_index = 0;

	switch(frame_state)
	{
		case STATE_WAIT_HEAD:
			if(data == FRAME_HEAD)
			{
				frame_state = STATE_WAIT_FUNC;
			}
		break;

		case STATE_WAIT_FUNC:
			rx_msg->func_code |= data & 0xff;
			frame_state = STATE_WAIT_LEN;
		break;
		
		case STATE_WAIT_LEN:
		{
			rx_msg->msg_len |= data & 0xff;
			data_count = rx_msg->msg_len; //count
				
			if( (rx_msg->msg_len <= 0) || (rx_msg->msg_len > 512) )
			{
				frame_state = STATE_WAIT_HEAD;
			}

			rx_msg->data = (uint8_t*)malloc(rx_msg->msg_len + HEAD_SIZE + LEN_SIZE + FUNC_CODE_SIZE);
			if(rx_msg->data == NULL)
			{
				frame_state = STATE_WAIT_HEAD;
				return false;
			}

			rx_index = 0;
			rx_msg->data[rx_index++] = FRAME_HEAD;
			rx_msg->data[rx_index++] = rx_msg->func_code & 0xff;
			rx_msg->data[rx_index++] = rx_msg->msg_len & 0xff;

			frame_state = STATE_WAIT_DATA;
		}
		break;

		case STATE_WAIT_DATA:
		{
			rx_msg->data[rx_index++] = data;

			data_count--;
			if(data_count == 0)
			{
				frame_state = STATE_WAIT_CRC_1;
				rx_index = 0;
			}
		}
		break;

		case STATE_WAIT_CRC_1:
		{
				rx_msg->msg_crc |= (data & 0xff) << 8;
				frame_state = STATE_WAIT_CRC_2;
		}
		break;

		case STATE_WAIT_CRC_2:
		{
			uint16_t crc16_value = 0;
			rx_msg->msg_crc |= data & 0xff;
			frame_state = STATE_WAIT_END;
			crc16_value = CRC16_CalcBufCrc(rx_msg->data, rx_msg->msg_len + HEAD_SIZE + LEN_SIZE + FUNC_CODE_SIZE);

			if(crc16_value != rx_msg->msg_crc)
			{
				NS_LOG_INFO("crc1: 0x%x crc2 :0x%x ...\r\n",crc16_value ,rx_msg->msg_crc);
				free(rx_msg->data);
				frame_state = STATE_WAIT_HEAD;
			}
		}
		break;

		case STATE_WAIT_END:
		{
			if(data != FRAME_END)
			{
				free(rx_msg->data);
				frame_state = STATE_WAIT_HEAD;
				break;
			}

			frame_state = STATE_SUCCESS;
			break;
		}

		default:
			frame_state = STATE_WAIT_HEAD;    
			break;
	}
	
	if(frame_state != STATE_SUCCESS)
	{
		return false;
	}

	frame_state = STATE_WAIT_HEAD;
	return true;
}

uint8_t app_recv_data_fifo_enter(const uint8_t *p_data, uint16_t len)
{
	while(len--)
	{
		rx_fifo_buf[rx_fifo_end++] = *p_data++;
		if(rx_fifo_end == USART_RX_FIFO_SIZE)
		{
			rx_fifo_end = 0;
		}
		if(rx_fifo_end == rx_fifo_start)
		{
			rx_fifo_start++;
			//rx_full_state = 1;
		}
	}
	return len;
}

bool ble_rx_msg(void)
{
	static rx_msg_frame_t rx_msg = {0};

	//相等代表未接收到数据   不相等代表有数据待处理
	if(rx_fifo_end == rx_fifo_start) 
	{
			return false;
	}
	//到了末尾则回环
	if(rx_fifo_start == USART_RX_FIFO_SIZE)
	{
			rx_fifo_start = 0;
	}
	
	BuzzerOn(BUZZER_PORT, BUZZER_PIN);

	//先解帧，而且是一个字节一个字节的放进去解开，先放进去的是fifobuf中的第一个，也就是帧头1，还有帧头2，当然他们都不会触发这个if语句中的内容但会在解帧过程中进行校验
	//顺利通过后，自然会到达rx_msg.fuc_code这一帧，那就是现在的代码了
	//所以帧不是所有都进来读的，而是挨着挨着读的，如果前面的头校验
	//注意看resolve_frame最后的return，校验完成没问题了才是true，rx_msg才真实有效
	if(resolve_frame(rx_fifo_buf[rx_fifo_start++], &rx_msg) == true)
	{
		uint8_t msg_buf[16] = {0};
		uint16_t send_size = 0;
		
		switch(rx_msg.func_code)
		{
			case FUNC_CODE_TIMING:
			{
				uint16_t data_index = HEAD_SIZE + LEN_SIZE + FUNC_CODE_SIZE;
				uint8_t tx_buf[2]   = {0xff,0xff};   //返回数据，可以是任意自定义值，在另一端可以通过解析该值做出具体的操作
				time_sec = rx_msg.data[data_index] * 60;
				//开计时器
				timer_state = 1;
				//继电器之所以在这一下开启，是因为如果写在timer中的话，会反复调用
				RelayOn(RELAY_PORT, RELAY_PIN);
				
				send_size = bsp_create_frame(msg_buf, tx_buf, sizeof(tx_buf), FUNC_CODE_TIMING_ACK);
//				for(int i = 0; i < send_size; i++)
//				{
//						NS_LOG_INFO("send : 0x%x \r\n",msg_buf[i]);
//				}
				rdtss_send_notify(msg_buf, send_size);
			}
			break;
			
			case FUNC_CODE_STOP:
			{
				uint8_t tx_buf[2]   = {0xff,0xff};
				//关计时器
				timer_state = 0;
				send_size = bsp_create_frame(msg_buf, tx_buf, sizeof(tx_buf), FUNC_CODE_STOP_ACK);
				rdtss_send_notify(msg_buf, send_size);
			}
			break;
			
			case FUNC_CODE_GET_TIME:
			{
				uint8_t tx_buf[2] = {0};
				tx_buf[0] = (uint8_t)((time_sec >> 8) & 0xFF);
				tx_buf[1] = (uint8_t)(time_sec & 0xFF);
				
				send_size = bsp_create_frame(msg_buf, &tx_buf[0], sizeof(tx_buf), FUNC_CODE_GET_TIME_ACK);
				
				rdtss_send_notify(msg_buf, send_size);
			}
			break;
			
			case FUNC_CODE_PAUSE:
			{
				uint8_t tx_buf[2] = {0};
				tx_buf[0] = (uint8_t)((time_sec >> 8) & 0xFF);
				tx_buf[1] = (uint8_t)(time_sec & 0xFF);
				
				//暂停态
				timer_state = 2;
				
				send_size = bsp_create_frame(msg_buf, &tx_buf[0], sizeof(tx_buf), FUNC_CODE_PAUSE_ACK);
				rdtss_send_notify(msg_buf, send_size);
			}
			break;
			
			case FUNC_CODE_RESUME:
			{
				uint8_t tx_buf[2] = {0};
				tx_buf[0] = (uint8_t)((time_sec >> 8) & 0xFF);
				tx_buf[1] = (uint8_t)(time_sec & 0xFF);
				
				//重新继续
				timer_state = 1;
				
				send_size = bsp_create_frame(msg_buf, &tx_buf[0], sizeof(tx_buf), FUNC_CODE_RESUME_ACK);
				rdtss_send_notify(msg_buf, send_size);
			}
			break;
			
			default:
        NS_LOG_INFO("not expected function code %d\r\n", rx_msg.func_code);
        return false;
		}
		
		//重要，malloc完必须free，并且要用memset清除结构体的内容
		free(rx_msg.data);
		memset(&rx_msg, 0, sizeof(rx_msg_frame_t));
		BuzzerOff(BUZZER_PORT, BUZZER_PIN);
		return true;
	}
	
	return false;
}
