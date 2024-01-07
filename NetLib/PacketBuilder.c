#include "PacketBuilder.h"

static Int32 kPacketBuilderBufferIndex = 0;
static UInt8 kPacketBuilderBuffer[PACKET_BUILDER_BUFFER_SIZE * PACKET_BUILDER_BUFFER_COUNT] = { 0 };

PacketRef _PacketInit(
    Int32 Length,
    Bool Extended
) {
    assert(0 < Length && Length <= PACKET_BUILDER_BUFFER_SIZE);
    kPacketBuilderBufferIndex = (kPacketBuilderBufferIndex + 1) % PACKET_BUILDER_BUFFER_COUNT;
    UInt8* Buffer = &kPacketBuilderBuffer[0] + PACKET_BUILDER_BUFFER_SIZE * kPacketBuilderBufferIndex;
    memset(Buffer, 0, Length);
    PacketRef Packet = (PacketRef)Buffer;
    Packet->Magic = (Extended) ? 0xC8F3 : 0xB7E2;
    Packet->Length = (UInt16)Length;
    return Packet;
}

UInt8* PacketAppendMemory(
    Int32 Length
) {
    UInt8* Buffer = &kPacketBuilderBuffer[0] + PACKET_BUILDER_BUFFER_SIZE * kPacketBuilderBufferIndex;
    PacketRef Packet = (PacketRef)Buffer;
    assert(0 < Length && Packet->Length + Length <= PACKET_BUILDER_BUFFER_SIZE);
    UInt8* Memory = Buffer + Packet->Length;
    memset(Memory, 0, Length);
    Packet->Length += (UInt16)Length;
    return Memory;
}

UInt8* PacketAppendMemoryCopy(
    UInt8* Source,
    Int32 Length
) {
    UInt8* Buffer = &kPacketBuilderBuffer[0] + PACKET_BUILDER_BUFFER_SIZE * kPacketBuilderBufferIndex;
    PacketRef Packet = (PacketRef)Buffer;
    assert(0 < Length && Packet->Length + Length <= PACKET_BUILDER_BUFFER_SIZE);
    UInt8* Memory = Buffer + Packet->Length;
    memcpy(Memory, Source, Length);
    Packet->Length += (UInt16)Length;
    return Memory;
}

Void PacketLogBytes(
    PacketRef Packet
) {
    UInt8* Buffer = (UInt8*)Packet;

    fprintf(stdout, "Packet Size: %d\n", Packet->Length);

    for (Int32 Index = 0; Index < Packet->Length; Index++) {
        fprintf(stdout, "%02hhX ", Buffer[Index]);
    }

    fprintf(stdout, "\n");
}
