
#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

size_t binlog(const uint8_t * bytes, size_t bytes_len, char * out,
              size_t out_len)
{
    size_t required = 1; // always need null termination
    if (out_len != 0)
    {
        *out = '\0';
    }
    // count and print
    for (; bytes_len > 0; bytes_len--, bytes++)
    {
        uint8_t byte = *bytes;

        if (isprint(byte) && byte != '\\')
        {
            required += 1;
            if (out_len > 1)
            {
                snprintf(out, 2, "%c", byte);
                out_len--;
                out++;
            }
        }
        else if (byte == '\\' || byte == '\n' || byte == '\t' || byte == '\f' ||
                 byte == '\v' || byte == '\r')
        {
            static const char * kCodes = "hnvlr";
            char code = (byte == '\\') ? '\\' : kCodes[byte - '\t'];
            required += 2;
            if (out_len > 2)
            {
                snprintf(out, 3, "\\%c", code);
                out_len -= 2;
                out += 2;
            }
        }
        else
        {
            static const size_t kBinCodeLen = sizeof("\\xFF") - 1;

            required += kBinCodeLen;
            if (out_len > kBinCodeLen)
            {
                snprintf(out, kBinCodeLen + 1, "\\x%02X", byte);
                out_len -= kBinCodeLen;
                out += kBinCodeLen;
            }
        }
    }

    return required;
}

struct Out
{
    char lguard;
    char out[1024];
};

bool check_out(Out & out, size_t out_len, const char * expected)
{
    bool ret = false;
    // verify no underflow
    if (out.lguard != '\xEE')
    {
        fprintf(stderr, "underflow");
        goto bail;
    }

    // verify output
    if (out_len && strcmp(out.out, expected))
    {
        fprintf(stderr, "output differs: got \"%s\", expected \"%s\"\n",
                out.out, expected);
        goto bail;
    }

    // verify nothing else is touched
    for (size_t i = out_len; i < sizeof(out.out); i++)
    {
        if (out.out[i] != '\xFE')
        {
            fprintf(stderr, "overflow\n");
            goto bail;
        }
    }
    ret = true;

bail:
    out.lguard = 0xEE;
    memset(out.out, 0xFE, sizeof(out.out));
    return ret;
}

int main(void)
{
    Out out;

    out.lguard = 0xEE;

    memset(out.out, 0xFE, sizeof(out.out));

    // simplest case, nothing written
    assert(binlog(NULL, 0, NULL, 0) == 1 && check_out(out, 0, ""));

    // simple case
    assert(binlog((const uint8_t *) "a", 1, out.out, 1) == 2 &&
           check_out(out, 1, ""));

    // simple case one written
    assert(binlog((const uint8_t *) "a", 1, out.out, 2) == 2 &&
           check_out(out, 2, "a"));

    // getting more complicated
    assert(binlog((const uint8_t *) "\xff", 1, out.out, 1) == 5);
    assert(check_out(out, 1, ""));

    // nothing output if we need 4 chars
    for (size_t out_len = 0; out_len < 5; out_len++)
    {
        assert(binlog((const uint8_t *) "\xff", 1, out.out, out_len) == 5);

        assert(check_out(out, out_len, ""));
    }

    // finally got something
    assert(binlog((const uint8_t *) "\xff", 1, out.out, 5) == 5);
    assert(check_out(out, 5, "\\xFF"));

    uint8_t buf[256];
    for (size_t i = 0; i < sizeof(buf); i++)
    {
        buf[i] = i & 255;
    }
    assert(binlog(buf, sizeof(buf), out.out, sizeof(out.out)) == 731);
    assert(check_out(
        out, sizeof(out.out),
        "\\x00\\x01\\x02\\x03\\x04\\x05\\x06\\x07\\x08\\h\\n\\v\\l\\r\\x0E\\x0F"
        "\\x10\\x11\\x12\\x13\\x14\\x15\\x16\\x17\\x18\\x19\\x1A\\x1B\\x1C\\x1D"
        "\\x1E\\x1F "
        "!\"#$%&'()*+,-./"
        "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\\\]^_`"
        "abcdefghijklmnopqrstuvwxyz{|}~"
        "\\x7F\\x80\\x81\\x82\\x83\\x84\\x85\\x86\\x87\\x88\\x89\\x8A\\x8B\\x8C"
        "\\x8D\\x8E\\x8F\\x90\\x91\\x92\\x93\\x94\\x95\\x96\\x97\\x98\\x99\\x9A"
        "\\x9B\\x9C\\x9D\\x9E\\x9F\\xA0\\xA1\\xA2\\xA3\\xA4\\xA5\\xA6\\xA7\\xA8"
        "\\xA9\\xAA\\xAB\\xAC\\xAD\\xAE\\xAF\\xB0\\xB1\\xB2\\xB3\\xB4\\xB5\\xB6"
        "\\xB7\\xB8\\xB9\\xBA\\xBB\\xBC\\xBD\\xBE\\xBF\\xC0\\xC1\\xC2\\xC3\\xC4"
        "\\xC5\\xC6\\xC7\\xC8\\xC9\\xCA\\xCB\\xCC\\xCD\\xCE\\xCF\\xD0\\xD1\\xD2"
        "\\xD3\\xD4\\xD5\\xD6\\xD7\\xD8\\xD9\\xDA\\xDB\\xDC\\xDD\\xDE\\xDF\\xE0"
        "\\xE1\\xE2\\xE3\\xE4\\xE5\\xE6\\xE7\\xE8\\xE9\\xEA\\xEB\\xEC\\xED\\xEE"
        "\\xEF\\xF0\\xF1\\xF2\\xF3\\xF4\\xF5\\xF6\\xF7\\xF8\\xF9\\xFA\\xFB\\xFC"
        "\\xFD\\xFE\\xFF"));

    buf[255] = '\0';
    printf("%s\n", buf + 1);
    return 0;
}
