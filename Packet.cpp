#include "Packet.h"

namespace EngineCore {
	Packet* Packet::createFromUDP(UDPpacket* packet) {
		Packet* self = new Packet();

		self->data = packet->data;
		self->len = packet->len;
		self->pointer = 0;
		self->address = packet->address;

		self->code = self->read<Uint16>();
		self->packetID = self->read<Uint16>();
		self->ack = self->read<Uint16>();
		self->bitfield = self->read<Uint16>();

		return self;
	}
	Packet Packet::create(Uint32 len, Uint32 pointer) {
		Packet self;

		self.pointer = pointer;
		self.len = len + pointer;

		self.data = new byte[self.len];

		return self;
	}
}