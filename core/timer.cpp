#include "io.hpp"
#include "interrupt.hpp"

#define PIT_CHAN_0 0x40
#define PIT_CTRL 0x43

extern "C" void TimerInterruptHandler();

namespace Timer {
    void Init() {
        Number divisor = 4772;  // freq = 250.038 Hz
        OutputByte(PIT_CTRL, 0x36);
        OutputByte(PIT_CHAN_0, (divisor & 0xff));
        OutputByte(PIT_CHAN_0, (divisor >> 8));
        Interrupt::Setup(IRQ(0), TimerInterruptHandler);
        Interrupt::UnmaskIRQ(0);
    }
};

