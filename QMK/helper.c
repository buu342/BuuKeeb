#include QMK_KEYBOARD_H
#include "helper.h"

/*==============================
    safe_rgb_brightness

    Direct Mode in OpenRGB purposefully ignores the brightness value
    defined by the keyboard config. This function is used to push
    the RGB values down to a safe level so the LEDs don't pull
    too much current.

    This functionality can be disabled by commenting
    LIMIT_DIRECTMODE_BRIGHTNESS out in config.h

    @param Input RGB data 
    @return Safe RGB color values
==============================*/

rgb_t safe_rgb_brightness(rgb_t rgb)
{
    hsv_t hsv = rgb_to_hsv(rgb);

    // Limit brightness
    #ifdef LIMIT_DIRECTMODE_BRIGHTNESS
        hsv.v = (((uint16_t)hsv.v) * RGB_MATRIX_MAXIMUM_BRIGHTNESS) / 255;
    #endif

    // Handle backlight brightness settings
    hsv.v = (((uint16_t)hsv.v) * rgb_matrix_get_val()) / RGB_MATRIX_MAXIMUM_BRIGHTNESS;

    // Convert back to RGB
    return hsv_to_rgb(hsv);
}


/*==============================
    rgb_to_hsv

    Converts an RGB value to HSV using the same logic as QMK.    

    QMK uses the CIE1931 curve to calculate the HSV values as per our perception.
    The CIE1931 implementation uses a table that was generated via Python code,
    which is available from this (now dead) website (use Web Archive):
    http://jared.geek.nz/2013/feb/linear-led-pwm

    I took that code and inverted the logic. This is the code used to generate
    the inverted table:

    ```
    INPUT_SIZE = 255
    OUTPUT_SIZE = 255
    INT_TYPE = 'const unsigned char'

    def cie1931(L):
        L = L*100.0
        if L <= 8:
            return (L/902.3)
        else:
            return ((L+16.0)/116.0)**3


    x = range(INPUT_SIZE + 1)
    cie = [
        round(cie1931(float(L) / INPUT_SIZE) * OUTPUT_SIZE)
        for L in x
    ]

    inverse = []
    for Y in range(OUTPUT_SIZE + 1):
        best_x = 0
        best_error = abs(cie[0] - Y)

        for X in range(1, INPUT_SIZE + 1):
            error = abs(cie[X] - Y)

            if error < best_error:
                best_x = X
                best_error = error

        inverse.append(best_x)

    f = open('cie1931_inverse.h', 'w')
    f.write('// Inverse CIE1931 correction table\n')
    f.write('// Automatically generated\n\n')

    f.write('%s cie_inverse[%d] = {\n' % (INT_TYPE, OUTPUT_SIZE + 1))
    f.write('\t')
    for i, value in enumerate(inverse):
        f.write('%d, ' % value)
        if i % 10 == 9:
            f.write('\n\t')

    f.write('\n};\n\n')
    f.close()
    ```

    @param The RGB value to convert 
    @return The converted HSV value
==============================*/

const unsigned char CIE1931_CURVE_INVERSE[256] PROGMEM = {
    0,   5,   14,  23,  31,  37,  42,  47,  51,  55, 
    58,  62,  65,  68,  71,  73,  76,  78,  81,  83, 
    85,  87,  89,  91,  93,  95,  97,  99,  100, 102, 
    104, 105, 107, 109, 110, 112, 113, 114, 116, 117, 
    119, 120, 121, 122, 124, 125, 126, 127, 129, 130, 
    131, 132, 133, 134, 135, 137, 138, 139, 140, 141, 
    142, 143, 144, 145, 146, 147, 148, 149, 150, 150, 
    151, 152, 153, 154, 155, 156, 157, 158, 158, 159, 
    160, 161, 162, 163, 163, 164, 165, 166, 167, 167, 
    168, 169, 170, 170, 171, 172, 173, 173, 174, 175, 
    176, 176, 177, 178, 178, 179, 180, 180, 181, 182, 
    183, 183, 184, 185, 185, 186, 187, 187, 188, 188, 
    189, 190, 190, 191, 192, 192, 193, 193, 194, 195, 
    195, 196, 197, 197, 198, 198, 199, 199, 200, 201, 
    201, 202, 202, 203, 203, 204, 205, 205, 206, 206, 
    207, 207, 208, 208, 209, 210, 210, 211, 211, 212, 
    212, 213, 213, 214, 214, 215, 215, 216, 216, 217, 
    217, 218, 218, 219, 219, 220, 220, 221, 221, 222, 
    222, 223, 223, 224, 224, 225, 225, 226, 226, 227, 
    227, 228, 228, 229, 229, 230, 230, 230, 231, 231, 
    232, 232, 233, 233, 234, 234, 235, 235, 235, 236, 
    236, 237, 237, 238, 238, 238, 239, 239, 240, 240, 
    241, 241, 242, 242, 242, 243, 243, 244, 244, 244, 
    245, 245, 246, 246, 247, 247, 247, 248, 248, 249, 
    249, 249, 250, 250, 251, 251, 251, 252, 252, 253, 
    253, 253, 254, 254, 255, 255, 
};

hsv_t rgb_to_hsv(rgb_t rgb)
{
    hsv_t   hsv;
    uint8_t max, min;
    uint16_t delta;

    max   = MAX(rgb.r, MAX(rgb.g, rgb.b));
    min   = MIN(rgb.r, MIN(rgb.g, rgb.b));
    delta = max - min;

    // Undo the CIE1931 brightness curve.
    hsv.v = pgm_read_byte(&CIE1931_CURVE_INVERSE[max]);

    // Grayscale
    if (delta == 0)
    {
        hsv.h = 0;
        hsv.s = 0;
        return hsv;
    }

    // Saturation
    hsv.s = ((uint16_t)delta*255)/max;

    // Hue
    if (max == rgb.r)
    {
        int16_t h = (((int16_t)rgb.g - rgb.b)*85)/(delta*2);
        if (h < 0)
            h += 256;
        hsv.h = h;
    } 
    else if (max == rgb.g)
        hsv.h = 85 + (((int16_t)rgb.b - rgb.r)*85)/(delta*2);
    else
        hsv.h = 171 + (((int16_t)rgb.r - rgb.g)*85)/(delta*2);

    // Done
    return hsv;
}