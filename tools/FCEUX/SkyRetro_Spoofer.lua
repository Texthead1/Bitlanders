emu.print("Running SkyRetro spoofer")

local SKYRETRO_CMD_PORT = 0x4028

local MAX_OPERANDS = 3

local operands = 0

OPCODES = {
    [0x00] = "RST",
    [0x10] = "AON",
    [0x20] = "RGB",
    [0x21] = "LDR",
    [0x22] = "LDG",
    [0x23] = "LDB",
    [0x28] = "COL",
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
    [0x40] = "RGBL",
    [0x41] = "RGBR",
    [0x42] = "RGBT",
    [0x48] = "COLL",
    [0x49] = "COLR",
    [0x4A] = "TRP",
    [0x50] = "INR",
    [0x51] = "ING",
    [0x52] = "INB",
    [0x60] = "SON",
    [0x68] = "VIB",
    [0x70] = "WAIT",
    [0x74] = "BUF",
    [0x7A] = "NOW",
    [0x90] = "AOFF",
    [0xA8] = "BCOL",
    [0xC0] = "FLL",
    [0xC1] = "FLR",
    [0xD0] = "DER",
    [0xD1] = "DEG",
    [0xD2] = "DEB",
    [0xE0] = "SOFF",
    [0xF8] = "CBUF",
    [0xF0] = "WAIL",
    [0xF4] = "BUFL",
    [0xFA] = "NOP",
}

OPERAND_COUNTS = {
    [0x00] = 0, [0x10] = 0, [0x20] = 0, [0x21] = 1, [0x22] = 1, [0x23] = 1, [0x28] = 3,
    [0x30] = 1, [0x31] = 0, [0x32] = 0, [0x33] = 0, [0x34] = 0, [0x35] = 0, [0x36] = 0,
    [0x37] = 0, [0x38] = 0, [0x39] = 0, [0x3A] = 0, [0x3B] = 0, [0x40] = 0, [0x41] = 0,
    [0x42] = 0, [0x48] = 3, [0x49] = 3, [0x4A] = 1, [0x50] = 0, [0x51] = 0, [0x52] = 0,
    [0x60] = 0, [0x68] = 1, [0x70] = 1, [0x74] = 0, [0x7A] = 0, [0x80] = 0, [0xA8] = 0,
    [0xC0] = 1, [0xC1] = 1, [0xD0] = 0, [0xD1] = 0, [0xD2] = 0, [0xE0] = 0, [0xF8] = 1,
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