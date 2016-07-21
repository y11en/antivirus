#include <math.h>

#define NO_COLOR 0x1000000U
    typedef unsigned int html_color;

#define COLORS_NOT_SIMILIAR      0
#define COLORS_SIMILIAR          1
#define COLORS_PROBABLE_SIMILIAR 2
    int rgb_colors_similiarity(html_color a, html_color b);

/*
 * Colors matching routine based on L*a*b* colors representation.
 */


struct RGB {
    double R, G, B;
};

struct XYZ {
    double x, y, z;
};

struct Lab {
    double L, a, b;
};

struct Luv {
    double L, u, v;
};


/*
 * XXX Modifys rgb!
 */
static int RGBtoXYZ(struct RGB *rgb, struct XYZ *xyz)
{
    /* http://www.easyrgb.com/math.php?MATH=M2#text2 */

    rgb->R = 100.0*((rgb->R > 0.04045) ? pow(((rgb->R + 0.055)/1.055),2.4) : (rgb->R / 12.92));
    rgb->G = 100.0*((rgb->G > 0.04045) ? pow(((rgb->G + 0.055)/1.055),2.4) : (rgb->G / 12.92));
    rgb->B = 100.0*((rgb->B > 0.04045) ? pow(((rgb->B + 0.055)/1.055),2.4) : (rgb->B / 12.92));

    /* Observer. = 2°, Illuminant = D65 */

    xyz->x = 0.412453*rgb->R + 0.357580*rgb->G + 0.180423*rgb->B;
    xyz->y = 0.212671*rgb->R + 0.715160*rgb->G + 0.072169*rgb->B;
    xyz->z = 0.019334*rgb->R + 0.119193*rgb->G + 0.950227*rgb->B;

    return 0;
}

static int XYZtoLuv(struct XYZ *xyz, struct Luv *luv)
{
    static struct XYZ ref_white = { 95.047, 100.000, 108.883 };
    double Yn = xyz->y/ref_white.y;
    double u1, v1, un, vn;

    if(Yn > 0.008856)
        luv->L = 116.0*pow(Yn, 1.0/3.0) - 16;
    else
        luv->L = 903.3*(Yn);

#define EPS 0.00000001

    if(xyz->x <= EPS)
        u1 = 0.0;
    else
        u1 = 4.0*xyz->x/(xyz->x+15.0*xyz->y+3.0*xyz->z);
    
    if(xyz->y <= EPS)
        v1 = 0.0;
    else
        v1 = 9.0*xyz->y/(xyz->x+15.0*xyz->y+3.0*xyz->z);

    un = 4.0*ref_white.x/(ref_white.x+15.0*ref_white.y+3.0*ref_white.z);
    vn = 9.0*ref_white.y/(ref_white.x+15.0*ref_white.y+3.0*ref_white.z);

    luv->u = 13.0*luv->L*(u1 - un);
    luv->v = 13.0*luv->L*(v1 - vn);

#undef EPS

    return 0;
}

/* XXX modifies xyz! */
static int XYZtoLab(struct XYZ *xyz, struct Lab *lab)
{
    /* http://www.easyrgb.com/math.php?MATH=M7#text7 */

    /* Observer. = 2°, Illuminant = D65 */

    static struct XYZ ref_white = { 95.047, 100.000, 108.883 };

    xyz->x /= ref_white.x;
    xyz->y /= ref_white.y;
    xyz->z /= ref_white.z;

    if(xyz->y > 0.008856)
        lab->L = 116.0*pow(xyz->y, 1.0/3.0) - 16;
    else
        lab->L = 903.3*(xyz->y);

#define F(t) (((t) > 0.008856) ? pow((t), 1.0/3.0) : 7.787*(t)+ (16.0/116.0))

    lab->a = 500.0*(F(xyz->x)-F(xyz->y));
    lab->b = 200.0*(F(xyz->y)-F(xyz->z));

#undef F

    return 0;
}


static int RGBtoLab(struct RGB *rgb, struct Lab *lab)
{
    struct XYZ xyz;

    if(RGBtoXYZ(rgb, &xyz))
        return 1;

    if(XYZtoLab(&xyz, lab))
        return 1;

    return 0;
}

static int RGBtoLuv(struct RGB *rgb, struct Luv *luv)
{
    struct XYZ xyz;

    if(RGBtoXYZ(rgb, &xyz))
        return 1;

    if(XYZtoLuv(&xyz, luv))
        return 1;

    return 0;
}

int rgb_colors_similiarity(html_color a, html_color b)
{
    struct RGB rgb_a = { ((a >> 16) & 0xFF), ((a >> 8) & 0xFF), (a & 0xFF) };
    struct RGB rgb_b = { ((b >> 16) & 0xFF), ((b >> 8) & 0xFF), (b & 0xFF) };
    struct Lab lab_a, lab_b;
    double distance = 0.0;

    rgb_a.R /= 255.0; rgb_a.G /= 255.0; rgb_a.B /= 255.0;
    rgb_b.R /= 255.0; rgb_b.G /= 255.0; rgb_b.B /= 255.0;

    if(RGBtoLab(&rgb_a, &lab_a))
        return 0;
    if(RGBtoLab(&rgb_b, &lab_b))
        return 0;

    lab_a.L -= lab_b.L;
    lab_a.a -= lab_b.a;
    lab_a.b -= lab_b.b;

    distance = lab_a.L*lab_a.L + lab_a.a*lab_a.a + lab_a.b*lab_a.b;

    if(distance < 200.0)
        return COLORS_SIMILIAR;
    else if(distance < 500.0)
        return COLORS_PROBABLE_SIMILIAR;
    else
        return COLORS_NOT_SIMILIAR;
}
