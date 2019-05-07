#ifndef GENERIC_ISR_H
#define GENERIC_ISR_H 

extern "C" void generic_interrupt_handler(unsigned number);
void load_generic_isr();

extern "C" {
    void isr_wrapper_0x0(void);
    void isr_wrapper_0x1(void);
    void isr_wrapper_0x2(void);
    void isr_wrapper_0x3(void);
    void isr_wrapper_0x4(void);
    void isr_wrapper_0x5(void);
    void isr_wrapper_0x6(void);
    void isr_wrapper_0x7(void);
    void isr_wrapper_0x8(void);
    void isr_wrapper_0x9(void);
    void isr_wrapper_0xa(void);
    void isr_wrapper_0xb(void);
    void isr_wrapper_0xc(void);
    void isr_wrapper_0xd(void);
    void isr_wrapper_0xe(void);
    void isr_wrapper_0xf(void);

    void isr_wrapper_0x10(void);
    void isr_wrapper_0x11(void);
    void isr_wrapper_0x12(void);
    void isr_wrapper_0x13(void);
    void isr_wrapper_0x14(void);
    void isr_wrapper_0x15(void);
    void isr_wrapper_0x16(void);
    void isr_wrapper_0x17(void);
    void isr_wrapper_0x18(void);
    void isr_wrapper_0x19(void);
    void isr_wrapper_0x1a(void);
    void isr_wrapper_0x1b(void);
    void isr_wrapper_0x1c(void);
    void isr_wrapper_0x1d(void);
    void isr_wrapper_0x1e(void);
    void isr_wrapper_0x1f(void);

    void isr_wrapper_0x20(void);
    void isr_wrapper_0x21(void);
    void isr_wrapper_0x22(void);
    void isr_wrapper_0x23(void);
    void isr_wrapper_0x24(void);
    void isr_wrapper_0x25(void);
    void isr_wrapper_0x26(void);
    void isr_wrapper_0x27(void);
    void isr_wrapper_0x28(void);
    void isr_wrapper_0x29(void);
    void isr_wrapper_0x2a(void);
    void isr_wrapper_0x2b(void);
    void isr_wrapper_0x2c(void);
    void isr_wrapper_0x2d(void);
    void isr_wrapper_0x2e(void);
    void isr_wrapper_0x2f(void);

    void isr_wrapper_0x30(void);
    void isr_wrapper_0x31(void);
    void isr_wrapper_0x32(void);
    void isr_wrapper_0x33(void);
    void isr_wrapper_0x34(void);
    void isr_wrapper_0x35(void);
    void isr_wrapper_0x36(void);
    void isr_wrapper_0x37(void);
    void isr_wrapper_0x38(void);
    void isr_wrapper_0x39(void);
    void isr_wrapper_0x3a(void);
    void isr_wrapper_0x3b(void);
    void isr_wrapper_0x3c(void);
    void isr_wrapper_0x3d(void);
    void isr_wrapper_0x3e(void);
    void isr_wrapper_0x3f(void);

    void isr_wrapper_0x40(void);
    void isr_wrapper_0x41(void);
    void isr_wrapper_0x42(void);
    void isr_wrapper_0x43(void);
    void isr_wrapper_0x44(void);
    void isr_wrapper_0x45(void);
    void isr_wrapper_0x46(void);
    void isr_wrapper_0x47(void);
    void isr_wrapper_0x48(void);
    void isr_wrapper_0x49(void);
    void isr_wrapper_0x4a(void);
    void isr_wrapper_0x4b(void);
    void isr_wrapper_0x4c(void);
    void isr_wrapper_0x4d(void);
    void isr_wrapper_0x4e(void);
    void isr_wrapper_0x4f(void);

    void isr_wrapper_0x50(void);
    void isr_wrapper_0x51(void);
    void isr_wrapper_0x52(void);
    void isr_wrapper_0x53(void);
    void isr_wrapper_0x54(void);
    void isr_wrapper_0x55(void);
    void isr_wrapper_0x56(void);
    void isr_wrapper_0x57(void);
    void isr_wrapper_0x58(void);
    void isr_wrapper_0x59(void);
    void isr_wrapper_0x5a(void);
    void isr_wrapper_0x5b(void);
    void isr_wrapper_0x5c(void);
    void isr_wrapper_0x5d(void);
    void isr_wrapper_0x5e(void);
    void isr_wrapper_0x5f(void);

    void isr_wrapper_0x60(void);
    void isr_wrapper_0x61(void);
    void isr_wrapper_0x62(void);
    void isr_wrapper_0x63(void);
    void isr_wrapper_0x64(void);
    void isr_wrapper_0x65(void);
    void isr_wrapper_0x66(void);
    void isr_wrapper_0x67(void);
    void isr_wrapper_0x68(void);
    void isr_wrapper_0x69(void);
    void isr_wrapper_0x6a(void);
    void isr_wrapper_0x6b(void);
    void isr_wrapper_0x6c(void);
    void isr_wrapper_0x6d(void);
    void isr_wrapper_0x6e(void);
    void isr_wrapper_0x6f(void);

    void isr_wrapper_0x70(void);
    void isr_wrapper_0x71(void);
    void isr_wrapper_0x72(void);
    void isr_wrapper_0x73(void);
    void isr_wrapper_0x74(void);
    void isr_wrapper_0x75(void);
    void isr_wrapper_0x76(void);
    void isr_wrapper_0x77(void);
    void isr_wrapper_0x78(void);
    void isr_wrapper_0x79(void);
    void isr_wrapper_0x7a(void);
    void isr_wrapper_0x7b(void);
    void isr_wrapper_0x7c(void);
    void isr_wrapper_0x7d(void);
    void isr_wrapper_0x7e(void);
    void isr_wrapper_0x7f(void);

    void isr_wrapper_0x80(void);
    void isr_wrapper_0x81(void);
    void isr_wrapper_0x82(void);
    void isr_wrapper_0x83(void);
    void isr_wrapper_0x84(void);
    void isr_wrapper_0x85(void);
    void isr_wrapper_0x86(void);
    void isr_wrapper_0x87(void);
    void isr_wrapper_0x88(void);
    void isr_wrapper_0x89(void);
    void isr_wrapper_0x8a(void);
    void isr_wrapper_0x8b(void);
    void isr_wrapper_0x8c(void);
    void isr_wrapper_0x8d(void);
    void isr_wrapper_0x8e(void);
    void isr_wrapper_0x8f(void);

    void isr_wrapper_0x90(void);
    void isr_wrapper_0x91(void);
    void isr_wrapper_0x92(void);
    void isr_wrapper_0x93(void);
    void isr_wrapper_0x94(void);
    void isr_wrapper_0x95(void);
    void isr_wrapper_0x96(void);
    void isr_wrapper_0x97(void);
    void isr_wrapper_0x98(void);
    void isr_wrapper_0x99(void);
    void isr_wrapper_0x9a(void);
    void isr_wrapper_0x9b(void);
    void isr_wrapper_0x9c(void);
    void isr_wrapper_0x9d(void);
    void isr_wrapper_0x9e(void);
    void isr_wrapper_0x9f(void);
}

#endif
