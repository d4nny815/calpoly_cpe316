from math import sin, pi


def main():
    # sawtooth_lut()
    # sine_lut()
    triangle_lut()


def sawtooth_lut():
    LUT_SPACE = 2 ** 8
    VPP = 3000 
    slope = VPP / LUT_SPACE
    print("const uint16_t SAWTOOTH_LUT[LUT_SIZE + 1] = {", end='')
    for i in range(LUT_SPACE):
        volt = vout(i * slope)
        print(f"{volt}", end='')
        if (i != LUT_SPACE - 1): print(", ", end='')
        
    print("};")
    return

def sine_lut():
    LUT_SPACE = 2 ** 8
    VPP = 3000 
    RADIANS = 2 * pi
    print("const uint16_t SINE_LUT[LUT_SIZE + 1] = {", end='')
    for i in range(LUT_SPACE):
        x = (VPP / 2) * sin(float(i) / LUT_SPACE  * RADIANS) + (VPP / 2)
        volt = vout(x)
        print(f"{volt}", end='')
        if (i != LUT_SPACE - 1): print(", ", end='')
    print("};")
    return

def triangle_lut():
    LUT_SPACE = 2 ** 8
    V_M = 3000 
    print("const uint16_t TRIANGLE_LUT[LUT_SIZE + 1] = {", end='')
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
