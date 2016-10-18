#ifndef __EFFECT_IMX134_SUNNY_H__

#define __EFFECT_IMX134_SUNNY_H__

{
#if 0
/* BLC */
	{0x1c58b, 0xff}, //avoid false contour Richard@0323
	{0x1c58c, 0xff}, //avoid false contour Richard@0323

/* AEC */
	{0x1c14a, 0x03},
	{0x1c14b, 0x0a},
	{0x1c14c, 0x0f},//aec fast step//
	{0x1c14e, 0x08},//slow step//08
	{0x1c140, 0x01},//banding
	{0x1c13e, 0x02},//real gain mode for OV8830

	{0x66401, 0x00},//window weight
	{0x66402, 0x20},//StatWin_Left
	{0x66403, 0x00},
	{0x66404, 0x20},//StatWin_Top
	{0x66405, 0x00},
	{0x66406, 0x20},//StatWin_Right
	{0x66407, 0x00},
	{0x66408, 0x28}, //StatWin_Bottom
	{0x66409, 0x01}, //definiton ofthe center 3x3 window
	{0x6640a, 0x00}, //nWin_Left
	{0x6640d, 0x00},
	{0x6640e, 0xc0}, //nWin_Top
	{0x66411, 0x06},
	{0x66412, 0x00}, //nWin_Width
	{0x66415, 0x04},
	{0x66416, 0x80}, //nWin_Height
	{0x6642e, 0x01},//nWin_Weight_0 weight pass
	{0x6642f, 0x01},//nWin_Weight_1
	{0x66430, 0x01},//nWin_Weight_2
	{0x66431, 0x01},//nWin_Weight_3
	{0x66432, 0x02},//nWin_Weight_4
	{0x66433, 0x02},//nWin_Weight_5
	{0x66434, 0x02},//nWin_Weight_6
	{0x66435, 0x02},//nWin_Weight_7
	{0x66436, 0x04},//nWin_Weight_8
	{0x66437, 0x02},//nWin_Weight_9
	{0x66438, 0x02},//nWin_Weight_10
	{0x66439, 0x02},//nWin_Weight_11
	{0x6643a, 0x02},//nWin_Weight_12
	{0x6644e, 0x03},//nWin_Weight_Shift
	{0x6644f, 0x04},//black level
	{0x66450, 0xf8},//saturate level
	{0x6645b, 0x1a},//black weight1
	{0x6645d, 0x1a},//black weight2
	{0x66460, 0x04},//saturate per1
	{0x66464, 0x0a},//saturate per2
	{0x66467, 0x14},//saturate weight1
	{0x66469, 0x14},//saturate weight2
	//auto AE control

/* Raw Stretch */
	{0x65020, 0x01},//RAW Stretch Target0x01-->00
	{0x66500, 0x28},
	{0x66501, 0x01},
	{0x66502, 0x8f},//0xff
	{0x66503, 0x0b},//0x0f
	{0x1c1b0, 0xff},
	{0x1c1b1, 0xff},
	{0x1c1b2, 0x01},
	{0x65905, 0x08},
	{0x66301, 0x02},//high level step
	{0x66302, 0xe0},//ref bin
	{0x66303, 0x06},//PsPer0 0a
	{0x66304, 0x10},//PsPer1
	{0x1c5a4, 0x01},//use new high stretch
	{0x1c5a5, 0x20},//stretch low step
	{0x1c5a6, 0x20},//stretch high step
	{0x1c5a7, 0x08},//stretch slow range
	{0x1c5a8, 0x02},//stretch slow step
	{0x1c1b8, 0x10},//ratio scale

	{0x1c5a2, 0x04},//target stable range
	{0x1c5a3, 0x06},//stretch target slow range

/* De-noise */
	{0x65604, 0x00},//Richard for new curve 0314
	{0x65605, 0x00},//Richard for new curve 0314
	{0x65606, 0x00},//Richard for new curve 0314
	{0x65607, 0x00},//Richard for new curve 0314

	{0x65510, 0x0F}, //G dns slope change from 0x4 to 0xf Richard 0320
	{0x65511, 0x1F}, //G dns slope change from 0x4 to 0xf Richard 0320
	{0x6551a, SONYIMX134_ISP_YDENOISE_COFF_1X},//Raw G Dns improve white pixel 20120728
	{0x6551b, SONYIMX134_ISP_YDENOISE_COFF_2X},//gain  2X
	{0x6551c, SONYIMX134_ISP_YDENOISE_COFF_4X},//gain  4X
	{0x6551d, SONYIMX134_ISP_YDENOISE_COFF_4X},//gain  8X
	{0x6551e, SONYIMX134_ISP_YDENOISE_COFF_4X},//gain 16X
	{0x6551f, SONYIMX134_ISP_YDENOISE_COFF_4X},//gain 32X
	{0x65520, SONYIMX134_ISP_YDENOISE_COFF_4X},//gain 64X
	{0x65522, 0x00},//RAW BR De-noise
	{0x65523, SONYIMX134_ISP_UVDENOISE_COFF_1X},//gain 1X
	{0x65524, 0x00},
	{0x65525, SONYIMX134_ISP_UVDENOISE_COFF_2X},//gain 2X
	{0x65526, 0x00},
	{0x65527, SONYIMX134_ISP_UVDENOISE_COFF_4X},//gain 4X
	{0x65528, 0x00},
	{0x65529, SONYIMX134_ISP_UVDENOISE_COFF_4X},//gain 8X
	{0x6552a, 0x00},
	{0x6552b, SONYIMX134_ISP_UVDENOISE_COFF_4X},//gain 16X
	{0x6552c, 0x00},
	{0x6552d, SONYIMX134_ISP_UVDENOISE_COFF_4X},//gain 32X
	{0x6552e, 0x00},
	{0x6552f, SONYIMX134_ISP_UVDENOISE_COFF_4X},//gain 64X

	{0x65c00, 0x03},//UV De-noise: gain 1X
	{0x65c01, 0x05},//gain 2X
	{0x65c02, 0x08},//gain 4X
	{0x65c03, 0x1f},//gain 8X
	{0x65c04, 0x1f},//gain 16X
	{0x65c05, 0x1f},//gain 32X

/* sharpeness */
	{0x65600, 0x00},
	{0x65601, 0x30},//0319
	{0x65602, 0x00},
	{0x65603, 0xC0},
	{0x65608, 0x08},
	{0x65609, 0x20},
	{0x6560c, 0x04},
	{0x6560d, 0x0A}, //low gain sharpness, 20120814 0x20->0x30
	{0x6560e, 0x10},//MinSharpenTp
	{0x6560f, 0x60},//MaxSharpenTp
	{0x65610, 0x10},//MinSharpenTm
	{0x65611, 0x60},//MaxSharpenTm
	{0x65613, 0x10},//SharpenAlpha
	{0x65615, 0x04},//HFreq_thre
	{0x65617, 0x08},//HFreq_coef

/* auto uv saturation */
	{0x1c4e8, 0x01},//Enable
	{0x1c4e9, 0xbf},// gain threshold1 40-->0b
	{0x1c4ea, 0xf7},//gain threshold2 78-->0d
	{0x1c4eb, 0x90}, //UV max saturation
	{0x1c4ec, 0x70}, //UV min saturation

/* Global Gamma */
	{0x1c49b, 0x01},
	{0x1c49c, 0x02},
	{0x1c49d, 0x01}, //gamma 2.0 0310
	{0x1c49e, 0x02},
	{0x1c49f, 0x01}, //gamma 2.0 0310
	{0x1c4a0, 0x00},
	{0x1c4a1, 0x18},
	{0x1c4a2, 0x00},
	{0x1c4a3, 0x88}, //gamma 2.0 0310 //avoid false contour Richard@0323


/* Tone Mapping */
	//contrast curve change for skin over exposure 20120728
	//low gain
	{0x1C4C0, 0x1b},
	{0x1C4C1, 0x2b},
	{0x1C4C2, 0x39},
	{0x1C4C3, 0x46},
	{0x1C4C4, 0x50},
	{0x1C4C5, 0x5a},
	{0x1C4C6, 0x66},
	{0x1C4C7, 0x72},
	{0x1C4C8, 0x7f},
	{0x1C4C9, 0x8c},
	{0x1C4CA, 0x9a},
	{0x1C4CB, 0xa8},
	{0x1C4CC, 0xb8},
	{0x1C4CD, 0xC9},
	{0x1C4CE, 0xe2},

	{0x1c4d4, 0x20},//EDR scale
	{0x1c4d5, 0x20},//EDR scale
	{0x1c4cf, 0x80},
	{0x65a00, 0x1b},
	{0x65a01, 0xc0}, //huiyan 0801

	//dark boost
	{0x1c4b0, 0x02},
	{0x1c4b1, 0x80},

	//YUV curve gain control,expoure frist
	{0x1c1b3, 0x20}, //Gain thre1
	{0x1c1b4, 0x50}, //Gain thre2

	{0x1c1b5, 0x01}, //EDR gain control
	{0x1c1b6, 0x01}, //Curve Gain control
	{0x1c1b7, 0x40}, //after gamma cut ratio

	//Manual UV curve
	{0x1C998, 0x01},
	{0x1C999, 0x02},
	{0x1C99A, 0x01},
	{0x1C99B, 0x10},
	{0x1C99C, 0x01},
	{0x1C99D, 0x16},
	{0x1C99E, 0x01},
	{0x1C99F, 0x16},
	{0x1C9A0, 0x01},
	{0x1C9A1, 0x16},
	{0x1C9A2, 0x01},
	{0x1C9A3, 0x16},
	{0x1C9A4, 0x01},
	{0x1C9A5, 0x16},
	{0x1C9A6, 0x01},
	{0x1C9A7, 0x16},
	{0x1C9A8, 0x01},
	{0x1C9A9, 0x16},
	{0x1C9AA, 0x01},
	{0x1C9AB, 0x16},
	{0x1C9AC, 0x01},
	{0x1C9AD, 0x16},
	{0x1C9AE, 0x01},
	{0x1C9AF, 0x16},
	{0x1C9B0, 0x01},
	{0x1C9B1, 0x16},
	{0x1C9B2, 0x01},
	{0x1C9B3, 0x08},
	{0x1C9B4, 0x00},
	{0x1C9B5, 0xe6},
	{0x1C9B6, 0x00},
	{0x1C9B7, 0xaa},

/* LENC */
	{0x1c247, 0x00},//three profile,color temperature based lens shading correction mode 1: enable 0: disable
	{0x1c24c, 0x00},
	{0x1c24d, 0x40},
	{0x1c24e, 0x00},
	{0x1c24f, 0x80},
	{0x1c248, 0x40},
	{0x1c24a, 0x20},
	{0x1c574, 0x00},
	{0x1c575, 0x20},
	{0x1c576, 0x00},
	{0x1c577, 0xf0},
	{0x1c578, 0x40},

	{0x65200, 0x0d},
	{0x65206, 0x3c},
	{0x65207, 0x04},
	{0x65208, 0x3c},
	{0x65209, 0x04},
	{0x6520a, 0x33},
	{0x6520b, 0x0c},
	{0x65214, 0x28},
	{0x65216, 0x20},
	{0x1d93d, 0x08},
	{0x1d93e, 0x00},
	{0x1d93f, 0x40},

	{0x1d940, 0xfc},
	{0x1d942, 0x04},
	{0x1d941, 0xfc},
	{0x1d943, 0x04},

	{0x1d944, 0x00},
	{0x1d946, 0x00},
	{0x1d945, 0xfc},
	{0x1d947, 0x04},

/* OVISP LENC setting for D65 Long Exposure (HDR/3D) */
	{0x1c264, 0x14},//Y1
	{0x1c265, 0xb },
	{0x1c266, 0x8 },
	{0x1c267, 0x8 },
	{0x1c268, 0xb },
	{0x1c269, 0x12},
	{0x1c26a, 0x7 },//Y2
	{0x1c26b, 0x4 },
	{0x1c26c, 0x2 },
	{0x1c26d, 0x2 },
	{0x1c26e, 0x4 },
	{0x1c26f, 0x7 },
	{0x1c270, 0x3 },//Y3
	{0x1c271, 0x1 },
	{0x1c272, 0x0 },
	{0x1c273, 0x0 },
	{0x1c274, 0x1 },
	{0x1c275, 0x3 },
	{0x1c276, 0x3 },//Y4
	{0x1c277, 0x1 },
	{0x1c278, 0x0 },
	{0x1c279, 0x0 },
	{0x1c27a, 0x1 },
	{0x1c27b, 0x3 },
	{0x1c27c, 0x5 },//Y5
	{0x1c27d, 0x3 },
	{0x1c27e, 0x2 },
	{0x1c27f, 0x2 },
	{0x1c280, 0x3 },
	{0x1c281, 0x6 },
	{0x1c282, 0x12},//Y6
	{0x1c283, 0x9 },
	{0x1c284, 0x6 },
	{0x1c285, 0x7 },
	{0x1c286, 0xa },
	{0x1c287, 0xf },
	{0x1c288, 0x20},//Cb1
	{0x1c289, 0x20},
	{0x1c28a, 0x20},
	{0x1c28b, 0x20},
	{0x1c28c, 0x20},
	{0x1c28d, 0x20},//Cb2
	{0x1c28e, 0x20},
	{0x1c28f, 0x20},
	{0x1c290, 0x20},
	{0x1c291, 0x20},
	{0x1c292, 0x20},//Cb3
	{0x1c293, 0x20},
	{0x1c294, 0x20},
	{0x1c295, 0x20},
	{0x1c296, 0x20},
	{0x1c297, 0x20},//Cb4
	{0x1c298, 0x20},
	{0x1c299, 0x20},
	{0x1c29a, 0x20},
	{0x1c29b, 0x20},
	{0x1c29c, 0x20},//Cb5
	{0x1c29d, 0x20},
	{0x1c29e, 0x20},
	{0x1c29f, 0x20},
	{0x1c2a0, 0x20},
	{0x1c2a1, 0x1e}, //Cr1
	{0x1c2a2, 0x1f},
	{0x1c2a3, 0x1f},
	{0x1c2a4, 0x1f},
	{0x1c2a5, 0x1e},
	{0x1c2a6, 0x1f}, //Cr2
	{0x1c2a7, 0x20},
	{0x1c2a8, 0x20},
	{0x1c2a9, 0x20},
	{0x1c2aa, 0x1f},
	{0x1c2ab, 0x1f}, //Cr3
	{0x1c2ac, 0x20},
	{0x1c2ad, 0x20},
	{0x1c2ae, 0x20},
	{0x1c2af, 0x1f},
	{0x1c2b0, 0x1f}, //Cr4
	{0x1c2b1, 0x20},
	{0x1c2b2, 0x20},
	{0x1c2b3, 0x20},
	{0x1c2b4, 0x1f},
	{0x1c2b5, 0x1e}, //cr5
	{0x1c2b6, 0x1f},
	{0x1c2b7, 0x1f},
	{0x1c2b8, 0x1f},
	{0x1c2b9, 0x1e},

/* AWB */
	{0x66201, 0x52},
	{0x66203, 0x14},//crop window
	{0x66211, 0xe8},//awb top limit
	{0x66212, 0x04},//awb bottom limit

	//{0x1c17c, 0x01},//CT mode
	{0x1c182, 0x04},
	{0x1c183, 0x00},//MinNum
	{0x1c184, 0x04},//AWB Step
	{0x1c58d, 0x00},//LimitAWBAtD65Enable

	{0x1c1be, 0x00},//AWB offset
	{0x1c1bf, 0x00},
	{0x1c1c0, 0x00},
	{0x1c1c1, 0x00},

	{0x1c1aa, 0x00},//avgAllEnable
	{0x1c1ad, 0x02},//weight of A
	{0x1c1ae, 0x10},//weight of D65
	{0x1c1af, 0x04},//weight of CWF

	{0x1c5ac, 0x80},//pre-gain
	{0x1c5ad, 0x80},
	{0x1c5ae, 0x80},

	{0x1ccce, 0x02},//awb shift,open=02

	/* new AWB shift */
	{0x1d902, 0x01},//Enable new awb shift
	{0x1d8e0, 0x00},//nAWBShiftBrThres1
	{0x1d8e1, 0xe0},
	{0x1d8e2, 0x08},//nAWBShiftBrThres2
	{0x1d8e3, 0xb0},
	{0x1d8f4, 0x30},//nAWBShiftBrThres3
	{0x1d8f5, 0x00},
	{0x1d8f6, 0xc0},//nAWBShiftBrThres4
	{0x1d8f7, 0x00},
	{0x1d8f2, 0x04},//nAWBShiftLowBrightThres
	{0x1d8f3, 0x22},//nAWBShiftHighBrightThres

	{0x1d8e9, 0x41},//A CT
	{0x1d8ea, 0x69},//cwf CT
	{0x1d8eb, 0xb8},//D CT

	{0x1d9c8, 0x7f},//B gain for A High light
	{0x1d9c9, 0x7f},//B gain for A
	{0x1d9ca, 0x7f},//B gain for A
	{0x1d9cb, 0x7D},//B gain for A low light

	{0x1d9cc, 0x86},//R gain for A High light
	{0x1d9cd, 0x86},//R gain for A
	{0x1d9ce, 0x86},//R gain for A
	{0x1d9cf, 0x8e},//R gain for A low light

	{0x1d9d0, 0x7e},//B gain for cwf High light
	{0x1d9d1, 0x82},//B gain for cwf
	{0x1d9d2, 0x82},//B gain for cwf
	{0x1d9d3, 0x81},//B gain for cwf low light

	{0x1d9d4, 0x83},//R gain for cwf High light
	{0x1d9d5, 0x84},//R gain for cwf
	{0x1d9d6, 0x84},//R gain for cwf
	{0x1d9d7, 0x8e},//R gain for cwf low light

	{0x1d9d8, 0x82},//B gain for D High light
	{0x1d9d9, 0x80},//B gain for D
	{0x1d9da, 0x80},//B gain for D
	{0x1d9db, 0x7f},//B gain for D low light

	{0x1d9dc, 0x7e},//R gain for D High light
	{0x1d9dd, 0x83},//R gain for D
	{0x1d9de, 0x83},//R gain for D
	{0x1d9df, 0x8e},//R gain for D low light



	// for detect indoor/outdoor awb shift on cwf light,epxo*gain>>8
	{0x1c5b4, 0x02},//C indoor/outdoor switch
	{0x1c5b5, 0xff},//C indoor/outdoor switch
	{0x1c5b6, 0x04},//C indoor/outdoor switch
	{0x1c5b7, 0xff},//C indoor/outdoor switch

	//add awb  shift detect parameter according 0x1c734~0x1c736
	{0x1ccd5, 0x3a}, //CT_A, 2012.06.02 yuanyabin
	{0x1ccd6, 0x66}, //CT_C
	{0x1ccd7, 0xac}, //CT_D

	{0x1c5cd, 0x00},//ori0x01 high light awb shift, modified by Jiangtao to avoid blurish when high CT 0310
	{0x1c5ce, 0x00},
	{0x1c5cf, 0xf0},
	{0x1c5d0, 0x01},
	{0x1c5d1, 0x20},
	{0x1c5d2, 0x03},
	{0x1c5d3, 0x00},
	{0x1c5d4, 0x40},
	{0x1c5d5, 0xa0},
	{0x1c5d6, 0xb0},
	{0x1c5d7, 0xe8},
	{0x1c5d8, 0x40},
	{0x1c1c2, 0x00},
	{0x1c1c3, 0x20},

	{0x66206, 0x12}, //center(cwf) window threshold D0
	{0x66207, 0x24}, //A threshold, range DX  0x15
	{0x66208, 0x15}, //day threshold, range DY 0xd
	{0x66209, 0x83}, //CWF X
	{0x6620a, 0x6a}, //CWF Y
	{0x6620b, 0xe3}, //K_A_X2Y, a slop
	{0x6620c, 0xa5}, //K_D65_Y2X, d slop
	{0x6620d, 0x53}, //D65 Limit
	{0x6620e, 0x37}, //A Limit
	{0x6620f, 0x75}, //D65 split
	{0x66210, 0x5c}, //A split
	{0x66201, 0x52},

	//add ccm detect parameter according 0x1c734~0x1c736
	{0x1c1c8 ,0x01}, //center CT, CWF
	{0x1c1c9 ,0x3f},
	{0x1c1cc, 0x00},//daylight
	{0x1c1cd, 0xbd},
	{0x1c1d0, 0x02},//a
	{0x1c1d1, 0x2a},

	{0x1c254, 0x0 },
	{0x1c255, 0xce},
	{0x1c256, 0x0 },
	{0x1c257, 0xe7},
	{0x1c258, 0x1 },
	{0x1c259, 0x69},
	{0x1c25a, 0x1 },
	{0x1c25b, 0xd2},


/* Color matrix */
	{0x1C1d8, 0x02},//center matrix,
	{0x1C1d9, 0x63},
	{0x1C1da, 0xFE},
	{0x1C1db, 0xA4},
	{0x1C1dc, 0xFF},
	{0x1C1dd, 0xF9},
	{0x1C1de, 0xFF},
	{0x1C1df, 0xDE},
	{0x1C1e0, 0x01},
	{0x1C1e1, 0x9C},
	{0x1C1e2, 0xFF},
	{0x1C1e3, 0x86},
	{0x1C1e4, 0x00},
	{0x1C1e5, 0x22},
	{0x1C1e6, 0xFE},
	{0x1C1e7, 0xA5},
	{0x1C1e8, 0x02},
	{0x1C1e9, 0x39},

	{0x1C1FC, 0xFF},//cmx left delta,daylight
	{0x1C1FD, 0xB8},
	{0x1C1FE, 0x00},
	{0x1C1FF, 0x3C},
	{0x1C200, 0x00},
	{0x1C201, 0x0C},
	{0x1C202, 0xFF},
	{0x1C203, 0xB1},
	{0x1C204, 0x00},
	{0x1C205, 0x15},
	{0x1C206, 0x00},
	{0x1C207, 0x3A},
	{0x1C208, 0xFF},
	{0x1C209, 0xE6},
	{0x1C20A, 0x00},
	{0x1C20B, 0x5B},
	{0x1C20C, 0xFF},
	{0x1C20D, 0xBF},

	{0x1C220, 0x00},//cmx right delta,a light
	{0x1C221, 0xD8},
	{0x1C222, 0xFF},
	{0x1C223, 0x6C},
	{0x1C224, 0xFF},
	{0x1C225, 0xBC},
	{0x1C226, 0xFF},
	{0x1C227, 0xD1},
	{0x1C228, 0x00},
	{0x1C229, 0x4F},
	{0x1C22A, 0xFF},
	{0x1C22B, 0xE0},
	{0x1C22C, 0xFF},
	{0x1C22D, 0xB7},
	{0x1C22E, 0x00},
	{0x1C22F, 0xFF},
	{0x1C230, 0xFF},
	{0x1C231, 0x4A},

	/* dpc */
	{0x65409, 0x08},
	{0x6540a, 0x08},
	{0x6540b, 0x08},
	{0x6540c, 0x08},
	{0x6540d, 0x0c},
	{0x6540e, 0x08},
	{0x6540f, 0x08},
	{0x65410, 0x08},
	{0x65408, 0x0b},

//dynamic high gain Y curve for dark color change 20120728
	{0x1d963, 0x1e},
	{0x1d964, 0x2e},
	{0x1d965, 0x39},
	{0x1d966, 0x46},
	{0x1d967, 0x50},
	{0x1d968, 0x5a},
	{0x1d969, 0x66},
	{0x1d96a, 0x72},
	{0x1d96b, 0x7e},
	{0x1d96c, 0x8a},
	{0x1d96d, 0x96},
	{0x1d96e, 0xa4},
	{0x1d96f, 0xb5},
	{0x1d970, 0xC9},
	{0x1d971, 0xe2},

/* auto uv saturation */
	{0x1d8fe, 0x01}, //UV cut gain control
	{0x1d8ff, 0x50}, //low gain thres
	{0x1d900, 0x70}, //high gain thres
	{0x1d97f, 0x14}, //UV cut low bright thres
	{0x1d973, 0x20}, //UV cut high bright thres
	{0x1d972, 0x01}, //adaptive gamma enable
	{0x1d974, 0x01}, //low gain gamma
	{0x1d975, 0xe6},
	{0x1d976, 0x01}, //high gain gamma
	{0x1d977, 0xc0},
	{0x1d978, 0x01}, //dark image gamma
	{0x1d979, 0xb3},
	{0x1d97a, 0x88}, //low gain slope
	{0x1d97b, 0x50}, //high gain slope
	{0x1d97c, 0x38}, //dark image slope
	{0x1d97d, 0x14}, //low bright thres
	{0x1d97e, 0x20}, //high bright thres

	{0x1d99e, 0x01}, //dynamic UV curve
	//low gain UV curve 1/2
	{0x1d904, 0x81},
	{0x1d905, 0x88},
	{0x1d906, 0x8b},
	{0x1d907, 0x8b},
	{0x1d908, 0x8b},
	{0x1d909, 0x8b},
	{0x1d90a, 0x8b},
	{0x1d90b, 0x8b},
	{0x1d90c, 0x8b},
	{0x1d90d, 0x8b},
	{0x1d90e, 0x8b},
	{0x1d90f, 0x8b},
	{0x1d910, 0x8b},
	{0x1d911, 0x84},
	{0x1d912, 0x73},
	{0x1d913, 0x55},
	//high gain UV curve 1/2
	{0x1d914, 0x81},
	{0x1d915, 0x88},
	{0x1d916, 0x8b},
	{0x1d917, 0x8b},
	{0x1d918, 0x8b},
	{0x1d919, 0x8b},
	{0x1d91a, 0x8b},
	{0x1d91b, 0x8b},
	{0x1d91c, 0x8b},
	{0x1d91d, 0x8b},
	{0x1d91e, 0x8b},
	{0x1d91f, 0x8b},
	{0x1d920, 0x8b},
	{0x1d921, 0x84},
	{0x1d922, 0x73},
	{0x1d923, 0x55},

	{0x1d924, 0x01}, //enable
	{0x1d950, 0x00},//Br thres,super highlight threshold
	{0x1d951, 0x08},//Br thres
	{0x1d952, 0x30},//Br Ratio,Ratio for transition region
	{0x1d8dc, 0x00},//Br thres0
	{0x1d8dd, 0x80},//Br thres0
	{0x1d8de, 0x44},//Br thres1
	{0x1d8df, 0x34},//Br thres1
	{0x1d8da, 0x10},//Br Ratio0
	{0x1d8db, 0x06},//Br Ratio1
	{0x1d949, 0x12}, //super highlight cwf thres //66206
	{0x1d925, 0x12}, //highlight cwf thres //66206
	{0x1d926, 0x12}, //middlelight cwf thres
	{0x1d927, 0x14}, //lowlight cwf thres

	{0x1d94a, 0x25}, //super highlight A thres //66207
	{0x1d928, 0x25}, //highlight A thres //66207
	{0x1d929, 0x31}, //middlelight A thres
	{0x1d92a, 0x33}, //lowlight A thres

	{0x1d94b, 0x17}, //super highlight D thres //66208
	{0x1d92b, 0x17}, //highlight D thres //66208
	{0x1d92c, 0x17}, //middlelight D thres
	{0x1d92d, 0x1a}, //lowlight D thres

	{0x1d94c, 0x51}, //super highlight D limit //6620d
	{0x1d92e, 0x50}, //highlight D limit //6620d
	{0x1d92f, 0x50}, //middlelight D limit
	{0x1d930, 0x4F}, //lowlight D limit

	{0x1d94d, 0x36}, //super highlight A limit //6620e
	{0x1d931, 0x36}, //highlight A limit //6620e
	{0x1d932, 0x32}, //middlelight A limit
	{0x1d933, 0x2E}, //lowlight A limit

	{0x1d94e, 0x71}, //super highlight D split //6620f
	{0x1d934, 0x71}, //highlight D split //6620f
	{0x1d935, 0x71}, //middlelight D split
	{0x1d936, 0x71}, //lowlight D split

	{0x1d94f, 0x5c},	//super highlight A split //66210
	{0x1d937, 0x5c}, //highlight A split //66210
	{0x1d938, 0x5c}, //middlelight A split
	{0x1d939, 0x5c}, //lowlight A split
	{0x1d998, 0x1},
#endif
	/* end symbol of isp_int_regs */
	{0x0, 0},
},

/* ae_params_s */
{
	/* ae_target_s */
	{
        {0xd, 0xf},
		{0x1a, 0x1c},
		{0x2c, 0x4c},
		{0x61, 0x62},
		{0x76, 0x78},
        /* end */
	},

		/* ae_win_params */
	{

		/* win2x2*/
		{85, 85},
		/* win3x3 */
		{{60, 60}, {60, 60}, {100, 100},},
		/* roi_enable */
		false,
		/* default_stat_win weights */
		//{1, 1, 1, 1, 9, 9, 9, 9, 18, 9, 9, 9, 9},
		{1, 1, 1, 1, 2, 2, 2, 2, 4, 2, 2, 2, 2},

		3,
		/* enhanced_stat_win */
		{1, 1, 1, 1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},

		8,
	},
	/* max_expo_gap */
	14,
	/* iso_max */
	3100,
	/* iso_min */
	100,
{
	/* gain_th_low */
	0x24,
	/* gain_th_high */
	0x60,
	/* reserve */
	0x00,
	/* reserve */
	0x00,
	},
	{
	/* low2mid manual iso*/
	310,
	/* mid2high manual iso*/
	180,
	/* high2mid manual iso*/
	120,
	/* mid2low manual iso*/
	120,
	},

	/* preview_fps_max */
	30,
	/* preview_fps_middle */
	15,
	/* preview_fps_min */
	10,
	/* capture_fps_min */
	8,

	/* expo_night */
	5,
	/* expo_action */
	100,
	/* cap_expo_table */
	{
		{30, 100}, /* max expo is 3 band(50Hz), 4band(60Hz) */
		//{23, 180}, /* max expo is 4 band(50Hz), 5band(60Hz) */
		{20, 200}, /* max expo is 5 band(50Hz), 6band(60Hz) */

		/* Below is Lower capture frame rate (or larger exposure) than normal frame rate. */
		{14, 600}, /* max expo is 7 band(50Hz), 8band(60Hz) */
		{10, 800}, /* max expo is 10 band(50Hz),  12band(60Hz)*/
		{8, 8000}, /* max expo is 12 band(50Hz), 15band(60Hz) */
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0},
	},
	/* ev_numerator */
	635,
	/* u32 ev_denominator */
	1000,

},

/* af_param */
{
	/*focus_area*/
	{
		/* percent_w */
		25,
		/* percent_w */
		20,
		/* max_zoom_ratio */
		0x200,
		/* min_height_ratio */
		5,
		/* weight[25] */
		{0},
	},
	/* focus_algo_s */
	{
		/* contrast_stat_mode */
		YUV_EDGE_STAT,
		/* contrast_threshold */
		{0x04, 0x10, 0x80, 0x140},
		/* param_judge */
		{0x18, 0x08, 0xc0, 0x04, 95},
		/* infinity_hold_frames */
		2,
		/* rewind_hold_frames */
		1,
		/* try_dir_range */
		5,
		/* param_caf_trigger */
		{3, 3, 4, 10, 0x28, 25, 16, 100, 0xc, 4, 4, 2000, 2, 6, 15, 6, 4, 0x40},
		/* calc_coarse_size */
		(1280 * 720),
	},
},

/* manual_wb_param */
{
	/* b_gai   gb_gain  gr_gain r_gain */
	{0x0000, 0x0000, 0x0000, 0x0000}, /* AWB not care about it */
	{0x012c, 0x0080, 0x0080, 0x0089}, /* INCANDESCENT 2800K */
	{0x00f2, 0x0080, 0x0080, 0x00b9}, /* FLUORESCENT 4200K */
	{0x00a0, 0x00a0, 0x00a0, 0x00a0}, /* WARM_FLUORESCENT, y36721 todo */
	{0x00d1, 0x0080, 0x0080, 0x00d2}, /* DAYLIGHT 5000K */
	{0x00b0, 0x0080, 0x0080, 0x00ec}, /* CLOUDY_DAYLIGHT 6500K*/
	{0x00a0, 0x00a0, 0x00a0, 0x00a0}, /* TWILIGHT, y36721 todo */
	{0x0168, 0x0080, 0x0080, 0x0060}, /* CANDLELIGHT, 2300K */
},


/* rcc */
{
	/* rcc_enable */
	false,
	/* frame_interval */
	8,
	/* detect_range */
	50,
	/* rect_row_num */
	5,
	/* rect_col_num */
	5,
	/* preview_width_high */
	960,
	/* preview_width_low */
	320,
	/* uv_resample_high */
	8,
	/* uv_resample_middle */
	4,
	/* uv_resample_low */
	2,
	/* refbin_low */
	0x80,
	/* refbin_high */
	0xf0,
	/* v_th_high */
	160,
	/* v_th_low */
	130,
},


/* sharpness_cfg */
{
	{
		/* preview_shapness */
		0x0A,    //hefei modify for imx134 sensor
		/* cap_shapness */
		0x0A,
	},

},


/* dns */
{
    /*raw_dns_coff*/
    {{0x06,0x0C,0x16,0x20,0x20,0x20},{0x0C,0x0E,0x10,0x12,0x13,0x14},{0x08,0x0a,0x0c,0x0d,0x0e,0x0e}},
    {{0x0A,0x0C,0x16,0x24,0x36,0x50},{0x10,0x12,0x14,0x15,0x16,0x18},{0x10,0x12,0x14,0x15,0x16,0x18}},
    {{0x0A,0x0C,0x16,0x24,0x36,0x50},{0x10,0x12,0x14,0x15,0x16,0x18},{0x10,0x12,0x14,0x15,0x16,0x18}},
    /*g_dns_caputure_1_band*/
    0x0A,
    /*g_dns_flash_1_band*/
    0x0A,
    {0x03,0x05,0x08,0x1f,0x1f,0x1f},
    {0x03,0x05,0x08,0x1f,0x1f,0x1f},
    {0x03,0x05,0x08,0x1f,0x1f,0x1f},
},


/* scene_param: uv_saturation */
{
	0x80, /* CAMERA_SCENE_AUTO */
	0x80, /* CAMERA_SCENE_ACTION */
	0x90, /* CAMERA_SCENE_PORTRAIT */
	0x98, /* CAMERA_SCENE_LANDSPACE */
	0x80, /* CAMERA_SCENE_NIGHT */
	0x90, /* CAMERA_SCENE_NIGHT_PORTRAIT */
	0x70, /* CAMERA_SCENE_THEATRE */
	0x80, /* CAMERA_SCENE_BEACH */
	0x80, /* CAMERA_SCENE_SNOW */
	0x80, /* CAMERA_SCENE_SUNSET */
	0x80, /* CAMERA_SCENE_STEADYPHOTO */
	0x80, /* CAMERA_SCENE_FIREWORKS */
	0x80, /* CAMERA_SCENE_SPORTS */
	0x80, /* CAMERA_SCENE_PARTY */
	0x80, /* CAMERA_SCENE_CANDLELIGHT */
	0x80, /* CAMERA_SCENE_BARCODE */
	0x80, /* CAMERA_SCENE_FLOWERS */
},

/* flash_param */
{
	/* gain */
	{0xc2, 0x80, 0x80, 0xe8},

	/* aceawb_step */
	{0x06, 0x18, 0x18, 0x10, 0x0c},
	/* videoflash_level */
	LUM_LEVEL2,
	/* assistant_af_params*/
	{0xA0, 0x10, 0x30, 30, LUM_LEVEL1},
	/* flash_capture */
	{0x70, LUM_LEVEL2, LUM_LEVEL5, 0x04, 0x30, 30, 0xe0, 0x20, 0x140},
},

#endif //__EFFECT_IMX134_SUNNY_H__

