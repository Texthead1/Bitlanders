emu.print("Running SkyRetro spoofer")

local SKYRETRO_CMD_PORT = 0x4028
local MAX_OPERANDS = 3
local operands = 0

OPCODES = {
    [0x00] = "RST",
    [0x01] = "AON",
    [0x10] = "RGB",
    [0x11] = "LDR",
    [0x12] = "LDG",
    [0x13] = "LDB",
    [0x14] = "RGBL",
    [0x15] = "RGBR",
    [0x16] = "RGBT",
    [0x18] = "COL",
    [0x19] = "COLL",
    [0x1A] = "COLR",
    [0x1B] = "TRP",
    [0x20] = "SON",
    [0x28] = "VIB",
    [0x30] = "ELM",
    [0x31] = "ELER",
    [0x32] = "ELWA",
    [0x33] = "ELAI",
    [0x34] = "ELFI",
    [0x35] = "ELLF",
    [0x36] = "ELDE",
    [0x37] = "ELMG",
    [0x38] = "ELTC",
    [0x39] = "ELLT",
    [0x3A] = "ELDR",
    [0x3B] = "ELKS",
    [0x60] = "INR",
    [0x61] = "ING",
    [0x62] = "INB",
    [0x70] = "WAIT",
    [0x74] = "BUF",
    [0x78] = "CBUF",
    [0x7A] = "NOW",
    [0x81] = "AOFF",
    [0x94] = "FLL",
    [0x95] = "FLR",
    [0x98] = "BCOL",
    [0xA0] = "SOFF",
    [0xE0] = "DER",
    [0xE1] = "DEG",
    [0xE2] = "DEB",
    [0xF0] = "WAIL",
    [0xF4] = "BUFL",
    [0xFA] = "NOP",
}

OPERAND_COUNTS = {
    [0x00] = 0, [0x01] = 0, [0x10] = 0, [0x11] = 1, [0x12] = 1, [0x13] = 1, [0x14] = 0,
    [0x15] = 0, [0x16] = 0, [0x18] = 3, [0x19] = 3, [0x1A] = 3, [0x1B] = 1, [0x20] = 0,
    [0x28] = 1, [0x30] = 1, [0x31] = 0, [0x32] = 0, [0x33] = 0, [0x34] = 0, [0x35] = 0,
    [0x36] = 0, [0x37] = 0, [0x38] = 0, [0x39] = 0, [0x3A] = 0, [0x3B] = 0, [0x60] = 0,
    [0x61] = 0, [0x62] = 0, [0x70] = 1, [0x74] = 0, [0x78] = 0, [0x7A] = 0, [0x81] = 0,
    [0x94] = 1, [0x95] = 1, [0x98] = 0, [0xA0] = 0, [0xE0] = 0, [0xE1] = 0, [0xE2] = 0,
    [0xF0] = 0, [0xF4] = 0, [0xFA] = 0,
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