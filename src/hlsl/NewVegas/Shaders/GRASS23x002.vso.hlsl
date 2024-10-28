uniform float3         DiffuseDir       : register(c0);    // c0
uniform float3         DiffuseColor     : register(c1);    // c1
uniform float3         ScaleMask        : register(c2);    // c2
uniform float4         WindData         : register(c4);    // c4
uniform float4         AlphaParam       : register(c5);    // c5
uniform float4         AmbientColor     : register(c6);    // c6
uniform float4         AddlParams       : register(c7);    // c7
uniform row_major float4x4 ModelViewProj : register(c9);   // c9-c12
uniform float4         FogColor         : register(c14);   // c14
uniform float4         FogParam         : register(c15);   // c15
uniform float4         InstanceData[2]  : register(c20);   // c20-c21

// Constants (from shader intrinsics)
// def c3, 1, 0, -0.5, 0.00999999978
static const float4 c3   = float4(1.0, 0.0, -0.5, 0.00999999978);   
// def c8, 0.75, 0.25, 0.0078125, 0         // c3
static const float4 c8   = float4(0.75, 0.25, 0.0078125, 0.0);   
// def c13, 0.159154937, 0.5, 6.28318548, -3.14159274
static const float4 c13  = float4(0.159154937, 0.5, 6.28318548, -3.14159274); // c13
// def c16, -1, 0, 1, 0
static const float4 c16  = float4(-1.0, 0.0, 1.0, 0.0);                       // c16
struct VS_INPUT
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 texcoord0 : TEXCOORD0;
    float1 texcoord1 : TEXCOORD1;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord0 : TEXCOORD0;
    float4 texcoord4 : TEXCOORD4;
    float4 texcoord5 : TEXCOORD5;
};

VS_OUTPUT main(VS_INPUT IN)
{
    VS_OUTPUT OUT;

    // Temporary registers
    float4 r0 = float4(0.0, 0.0, 0.0, 0.0);
    float4 r1 = float4(0.0, 0.0, 0.0, 0.0);
    float4 r2 = float4(0.0, 0.0, 0.0, 0.0);
    float4 r3 = float4(0.0, 0.0, 0.0, 0.0);
    int a0_x = 0;

    // --- Begin Shader Instructions ---

    // frc r0.x, v3.x
    r0.x = frac(IN.texcoord1.x);

    // add r0.x, -r0.x, v3.x
    r0.x = IN.texcoord1.x - r0.x;

    // mova a0.x, r0.x
    a0_x = (int)r0.x; // Correctly rounds to nearest int and clamps between 0 and 1

    // mov r0.xyw, c3
    r0.xyw = c3.xyw; // 1.0

    // mad r1, c20[a0.x].xyzx, r0.xxxy, r0.yyyx
    r1 = (InstanceData[a0_x].xyzx * r0.xxxy) + r0.yyyx; // (c20[a0.x].x * r0.x) + r0.y

    // Transform position using ModelViewProj matrix
    r2 = mul(ModelViewProj, r1); // Equivalent to dp4 r2.x, c9, r1 ... dp4 r2.w, c12, r1

    // dp4 r0.y, r2, r2
    r0.y = dot(r2, r2);

    // rsq r0.y, r0.y
    r0.y = rsqrt(r0.y);

    // rcp r0.y, r0.y
    r0.y = 1.0 / r0.y;

    // add r0.y, r0.y, -c5.z
    r0.y = (-r0.y) + AlphaParam.z;

    // rcp r0.z, c5.w
    r0.z = 1.0 / AlphaParam.w;

    // mul_sat r0.y, r0.y, r0.z
    r0.y = saturate(r0.y * r0.z);

    // add o3.w, -r0.y, c3.x
    OUT.texcoord5.w = (-r0.y) + c3.x;

    // frc r1, c20[a0_x]
    r1 = frac(InstanceData[a0_x]);

    // add r1.xyz, r1, c3.z
    r1.xyz = (r1 + c3.z).xyz; // c3.z = -0.5

    // mad r0.y, r1.w, c8.x, c8.y
    r0.y = (r1.w * c8.x) + c8.y;

    // add r1.xyz, r1, r1
    r1.xyz = (r1 + r1).xyz;

    // abs r1_abs, r1
    float4 r1_abs = abs(r1);

    // sge r2.xy, r1_abs.yzzw, r1_abs.x
    r2.xy = step(r1_abs.x, r1_abs.yz);

    // mul r0.z, r2.y, r2.x
    r0.z = r2.y * r2.x;

    // mul r2.xyz, r1.zzxw, c16
    r2.xyz = (r1.zzxw * c16).xyz;

    // mad r3.xyz, r1.zzyw, c16.yxzw, -r2
    r3.xyz = ((r1.zzyw * c16.yxzw) + (-r2)).xyz;

    // mad r2.xyz, r0.z, r3, r2.xyz
    r2.xyz = (r0.z * r3.xyz) + r2.xyz;

    // nrm r3.xyz, r2
    r3.xyz = normalize(r2.xyz);

    // mul r2.xyz, r1.yzxw, r3.zxyw
    r2.xyz = (r1.yzxw * r3.zxyw).xyz;

    // mad r2.xyz, r3.yzxw, r1.zxyw, -r2
    r2.xyz = ((r3.yzxw * r1.zxyw) + (-r2)).xyz;

    // mul r0.z, r0.w, c20[a0.x].w
    r0.z = r0.w * InstanceData[a0_x].w;

    // mad r0.xzw, r0.z, c2.xyyz, r0.x
    r0.xzw = ((r0.z * ScaleMask.xyyz) + r0.x).xzw;

    // mul r0.xzw, r0, v0.xyyz
    r0.xzw = (r0 * IN.position.xyyz).xzw;

    // mul r3.xyz, r3.xyz, r0.z
    r3.xyz = r3.xyz * r0.z;

    // mad r2.xyz, r2, r0.x, r3
    r2.xyz = ((r2 * r0.x) + r3).xyz;

    // mad r0.xzw, r1.xyyz, r0.w, r2.xyyz
    r0.xzw = ((r1.xyyz * r0.w) + r2.xyyz).xzw;

    // dp3_sat r1.x, c0, r1
    r1.x = saturate(dot(DiffuseDir, r1.xyz));

    // add r1.y, c20[a0.x].y, c20[a0.x].x
    r1.y = InstanceData[a0_x].y + InstanceData[a0_x].x;

    // mov r1.w, c4.w
    r1.w = WindData.w;

    // mad r1.y, r1.y, c8.z, r1.w
    r1.y = (r1.y * c8.z) + r1.w;

    // mad r1.y, r1.y, c13.x, c13.y
    r1.y = (r1.y * c13.x) + c13.y;

    // frc r1.y, r1.y
    r1.y = frac(r1.y);

    // mad r1.y, r1.y, c13.z, c13.w
    r1.y = (r1.y * c13.z) + c13.w;

    // sincos r2.y, r1.y
    r2.y = sin(r1.y);

    // mul r1.y, r2.y, c4.z
    r1.y = r2.y * WindData.z;

    // mul r1.z, v1.w, v1.w
    r1.z = IN.color.w * IN.color.w;

    // mul r1.y, r1.y, r1.z
    r1.y = r1.y * r1.z;

    // mul r2.xy, r1.y, c4
    r2.xy = (r1.y * WindData).xy;

    // mov r2.z, c3.y
    r2.z = c3.y; // 0.0

    // add r0.xzw, r0, r2.xyyz
    r0.xzw = (r0 + r2.xyyz).xzw;

    // add r2.xyz, r0.xzww, c20[a0.x]
    r2.xyz = (r0.xzww + InstanceData[a0_x]).xyz;

    // mov r2.w, c3.x
    r2.w = c3.x; // 1.0

    // dp4 o0.w, c12, r2
    OUT.position.w = dot(ModelViewProj[3], r2);

    // mul o2, r0.y, c6
    OUT.texcoord4 = r0.y * float4((DiffuseColor.rgb/4.0), AmbientColor.a);

    // mul r0.xyz, r0.y, v1
    r0.xyz = r0.y * IN.color.xyz;

    // mul r0.xyz, r1.x, r0
    r0.xyz = r1.x * r0.xyz;

    // mul r0.xyz, r0, c1
    r0.xyz = (r0.xyz * DiffuseColor);

    // mul o3.xyz, r0, c7.x
    OUT.texcoord5.xyz = r0.xyz * AddlParams.x;

    // dp4 r0.x, c9, r2
    r0 = mul(ModelViewProj, r2);

    // mov OUT.position.xyz, r0
    OUT.position.xyz = r0.xyz;

    // rsq r0.x, r0.w
    r0.x = rsqrt(r0.w);

    // rcp r0.x, r0.x
    r0.x = 1.0 / r0.x;

    // add r0.x, -r0.x, c15.x
    r0.x =  (-r0.x) + FogParam.x;

    // rcp r0.y, c15.y
    r0.y = 1.0 / FogParam.y;

    // mul_sat r0.x, r0.x, r0.y
    r0.x = saturate(r0.x * r0.y);

    // add r0.x, -r0.x, c3.x
    r0.x = (-r0.x) + c3.x;

    // log r0.x, r0.x
    r0.x = log(r0.x);

    // mul r0.x, r0.x, c15.z
    r0.x = r0.x * FogParam.z;

    // exp o4.w, r0.x
    OUT.color.w = exp(r0.x);

    // mov o1.xy, v2
    OUT.texcoord0 = IN.texcoord0;

    // mov o4.xyz, c14
    OUT.color.xyz = FogColor.xyz;

    return OUT;
}
