emu.print("Running SkyRetro spoofer")

local SKYRETRO_CMD_PORT = 0x4028
local MAX_OPERANDS = 3

local operands = 0

OPCODES = {
    [0x00] = "RST",
    [0x01] = "AON",
    [0x30] = "LDR",
    [0x31] = "LDG",
    [0x32] = "LDB",
    [0x33] = "RGB",
    [0x34] = "COL",
    [0xea] = "NOP"
}

OPERAND_COUNTS = {
    [0x00] = 0,
    [0x01] = 0,
    [0x30] = 1,
    [0x31] = 1,
    [0x32] = 1,
    [0x33] = 0,
    [0x34] = 3,
    [0xea] = 0
}

op_buffer = {}

function ConcatOperands(arr)
    local out = {}
    local arr_len = #arr

    if arr_len > 0 then
        for i = 1, arr_len do
            out[#out+1] = string.format("%02X", arr[i])
        end
    end

    for i = arr_len + 1, MAX_OPERANDS do
        out[#out+1] = ""
    end

    return table.concat(out, "\t")
end

function PrintCMDPORT(_, _, value)
    if operands > 0 then
        op_buffer[#op_buffer + 1] = value
        operands = operands - 1

        local op = OPCODES[op_buffer[0]] or "UNK"

        if operands == 0 then
            emu.print(string.format("%04X\t%02X\t%02X\t%s\t%s", memory.getregister("pc"), SKYRETRO_CMD_PORT, op_buffer[0], ConcatOperands(op_buffer), op))
            for i = #op_buffer, 1, -1 do
                op_buffer[i] = nil
            end
        end
    else
        local op = OPCODES[value] or "UNK"
        operands = OPERAND_COUNTS[value] or 0

        if operands == 0 then
            emu.print(string.format("%04X\t%02X\t%02X\t%s\t%s", memory.getregister("pc"), SKYRETRO_CMD_PORT, value, ConcatOperands(op_buffer), op))
        else
            op_buffer[0] = value
        end
    end
end

memory.registerwrite(SKYRETRO_CMD_PORT, 1, PrintCMDPORT)