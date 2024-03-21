#include "nice.h"
#include <stdlib.h>

#include "cc1101.h"

static void PWM_Encode(uint8_t* in , uint8_t* out, size_t size_in){
    for(size_t i = 0;i < size_in ;i++) {
        for (int8_t b = 0 ; b < 8 ; b++) {
            if (in[i] & (1 << (7 - b))) {
                SET_BIT_ARRAY(out, (i*8+b)*3   , (PWM_TRUE & 0b100) >> 2);
                SET_BIT_ARRAY(out, (i*8+b)*3 +1, (PWM_TRUE & 0b010) >> 1);
                SET_BIT_ARRAY(out, (i*8+b)*3 +2, (PWM_TRUE & 0b001)     );
            } else {
                SET_BIT_ARRAY(out, (i*8+b)*3   , (PWM_FALSE & 0b100) >> 2);
                SET_BIT_ARRAY(out, (i*8+b)*3 +1, (PWM_FALSE & 0b010) >> 1);
                SET_BIT_ARRAY(out, (i*8+b)*3 +2, (PWM_FALSE & 0b001)     );
            }
        }
    }
    
}

static void Nice_ReformatPacket(uint8_t* pwm_data)
{
	uint8_t tmp;
	uint8_t tmp2;

	// Adding manual 1 bit preamble
	tmp         = pwm_data[0] & 1;
	pwm_data[0] = (pwm_data[0] >> 1) | (1 << 7);

	for (uint8_t i = 1; i < NICE_PACKETSIZE; i++) {
		tmp2        = pwm_data[i] & 1;
		pwm_data[i] = (pwm_data[i] >> 1) | (tmp << 7);
		tmp         = tmp2;
	}

	// Triming useless bits
	pwm_data[4] &= ~0b11;
}

/* Packet must be  5 bytes long */
static void Nice_MakePacket(niceModule* this, uint8_t* packet)
{
	uint8_t data_encode[NICE_PACKETSIZE] = {0};
	uint16_t data_tmp                    = ((this->packet.id & 1023) << 2) | (this->packet.channel & 0b11);
	uint8_t data[2]                      = { data_tmp >> 4, (data_tmp & 0b1111) << 4 };

	PWM_Encode(data, data_encode, 2);
	Nice_ReformatPacket(data_encode);

	for (size_t i = 0 ; i < NICE_PACKETSIZE ; i++)
		packet[i] = data_encode[i];
}

static uint32_t Nice_AutoGeneratePacket(remoteModule* super, uint32_t field, uint8_t* packet, size_t size)
{
	CHECK_OBJ 0;
	if (size < NICE_PACKETSIZE) return 0;

	niceModule* this = (niceModule*)(super->this);
	uint32_t ret     = 0;

	if (field == ID) {
		this->packet.id = (this->packet.id + 1) % NICE_MAX_ID;
		ret = this->packet.id;
	} else if (field == CHANNEL) {
		this->packet.channel = (this->packet.channel + 1) % NICE_MAX_CHANNEL;
		ret = this->packet.channel;
	}

	Nice_MakePacket(this, packet);
	return ret;
}

static void Nice_GeneratePacket(remoteModule* super, void* _packet_src, uint8_t* packet, size_t size)
{
	CHECK_OBJ;
	if (size < NICE_PACKETSIZE) return;

	niceModule* this = (niceModule*)(super->this);

	if (_packet_src) {
		nicePacket* packet_src = (nicePacket*)(_packet_src);
		this->packet           = *packet_src;
	}

	Nice_MakePacket(this, packet);
	return;
}

static void Nice_SetPacket(remoteModule* super, void* _packet)
{
	CHECK_OBJ;
	if (!_packet) return;

	niceModule* this       = (niceModule*)(super->this);
	nicePacket* packet = (nicePacket*)(_packet);
	
	this->packet.id      = packet->id;
	this->packet.channel = packet->channel;

	return;
}

static void Nice_Destructor(remoteModule* super) {
	CHECK_OBJ;

	S_FREE(super->this);
}

void Nice_Init(remoteModule* super, tranceivers tranceiver)
{
	niceModule* this          = malloc(sizeof(niceModule));

	this->tranceiver          = tranceiver;
	this->packet.id           = 0;
	this->packet.channel      = 0;
	super->this               = this;
	super->Destructor         = &Nice_Destructor;
	super->GetRfSettings      = &Nice_GetRfSettings;
	super->AutoGeneratePacket = &Nice_AutoGeneratePacket;
	super->GeneratePacket     = &Nice_GeneratePacket;
	super->SetPacket          = &Nice_SetPacket;
}
