# Author: Biren Patel
# Target: MIPS 32
# Description: Assemble the machine code for [ori $8,$9,0x004A] into register 25

        .text
        .globl  start

start:
        xor     $25, $25, $25           # clear register

                                        # ~~~ assemble ~~~

        ori     $11, $0, 0xD            # load pattern for the ori opcode
        sll     $11, $11, 0x1A          # shift opcode into position 26-31
        or      $25, $25, $11           # load opcode to target register

        ori     $12, $0, 0x9            # load pattern for source register
        sll     $12, $12, 0x15          # shift pattern into position 21-25
        or      $25, $25, $12           # load source pattern to target register

        ori     $13, $0, 0x8            # load pattern for destination
        sll     $13, $13, 0x10          # shift pattern into position 16-20
        or      $25, $25, $13           # load destination to target register

        ori     $14, $0, 0x4A           # load unsigned const for position 0-15
        or      $25, $25, $14           # load to target, no left shift

                                        # ~~~ disassemble ~~~

        srl     $7, $25, 0x1A           # load opcode from bits 26-31
        andi    $10, $25, 0xFFFF        # load unsigned const value after mask

        srl     $8, $25, 0x15           # load opcode + source
        andi    $8, $8, 0x1F            # remove opcode

        srl     $9, $25, 0x10           # load opcode + source + destination
        andi    $9, $9, 0x1F            # remove opcode and source

                                        # ~~~ debugging ~~~

        srl     $11, $11, 0x1A          # shift opcode to verify register 7
        srl     $12, $12, 0x15          # shift source to verify register 8
        srl     $13, $13, 0x10          # shift destination to verify register 9
        ori     $8, $9, 0x004A          # verify instruction in the text segment
