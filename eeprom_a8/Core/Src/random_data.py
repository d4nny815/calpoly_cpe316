from random import randint

# MEM_SIZE = 1 << 15 # 32KB
ARR_SIZE = 100

def main():
    print("#ifndef SRC_BYTES_H_")
    print("#define SRC_BYTES_H_")
    print("\n#include \"main.h\"")    
    print(f"#define ARR_SIZE {ARR_SIZE}\n")    


    print(f'static uint16_t ADDRS[ARR_SIZE] = ',end='')
    print("{", end='')    
    for i in range(ARR_SIZE):
        print(f"{randint(0, (1 << 16) - 1)}", end='')
        if i != ARR_SIZE - 1:
            print(", ", end='')
    print("};\n")  

    
    print(f'static uint8_t SEND_BYTES[ARR_SIZE] = ',end='')
    print("{", end='')    
    for i in range(ARR_SIZE):
        print(f"{randint(0, 255)}", end='')
        if i != ARR_SIZE - 1:
            print(", ", end='')
    print("};\n")        

    print(f'static uint8_t REC_BYTES[ARR_SIZE] = ',end='')
    print("{0", end='')    
    print("};\n") 


    print("#endif /* SRC_BYTES_H_ */")

main()