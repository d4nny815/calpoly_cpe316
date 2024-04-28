from math import sin, pi

CPU_FREQ = 4e6
T_WAVE = 10e-3
LUT_SPACE = 2 ** 8
LUT_SPACE = 350
NEXT_CCR = int(T_WAVE * CPU_FREQ / LUT_SPACE)

def main():
    print("#ifndef SRC_WAVEFORM_LUT_H_")
    print("#define SRC_WAVEFORM_LUT_H_")
    print()
    print('#include "stm32l476xx.h"')
    print()
    print(f"#define WAVE_LUT_SIZE {LUT_SPACE}")
    print(f"#define WAVE_LUT_MASK ({LUT_SPACE} - 1)")
    print(f"#define NEXT_CCR {NEXT_CCR}")
    print("\n")
    triangle_lut()
    sine_lut()
    sawtooth_lut()
    
    print()
    print("#endif /* SRC_WAVEFORM_LUT_H_ */")


def sawtooth_lut():
    VPP = 3000 
    slope = VPP / LUT_SPACE
    print("static const uint16_t SAWTOOTH_LUT[WAVE_LUT_SIZE] = {", end='')
    for i in range(LUT_SPACE):
        volt = vout(i * slope)
        print(f"{volt}", end='')
        if (i != LUT_SPACE - 1): print(", ", end='')
        
    print("};")
    return

def sine_lut():
    VPP = 3000 
    RADIANS = 2 * pi
    print("static const uint16_t SINE_LUT[WAVE_LUT_SIZE] = {", end='')
    for i in range(LUT_SPACE):
        x = (VPP / 2) * sin(float(i) / LUT_SPACE  * RADIANS) + (VPP / 2)
        volt = vout(x)
        print(f"{volt}", end='')
        if (i != LUT_SPACE - 1): print(", ", end='')
    print("};")
    return

def triangle_lut():
    V_M = 3000 
    print("static const uint16_t TRIANGLE_LUT[WAVE_LUT_SIZE] = {", end='')
    for i in range(LUT_SPACE):
        x = 0;
        if i == LUT_SPACE / 2:
            x = V_M
        elif i < LUT_SPACE / 2:
            x = (V_M / (LUT_SPACE * .5)) * i
        else:
            x = (-V_M / (LUT_SPACE * .5)) * (i - LUT_SPACE / 2) + V_M
        volt = vout(x)
        print(f"{volt}", end='')
        if (i != LUT_SPACE - 1): print(", ", end='')
    print("};")
    return

def vout(x):
    return int(0x3 << 12 | (0xfff & int(x / 3300 * 0xfff)))

    

if __name__ == "__main__":
    main()
