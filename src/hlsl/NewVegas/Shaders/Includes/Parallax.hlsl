#ifdef TERRAIN
float4 TESR_TerrainParallaxData : register(c35);
float4 TESR_TerrainParallaxExtraData : register(c36);
#else
float4 TESR_ParallaxData : register(c35);
#endif

#ifdef TERRAIN
// Complex Parallax Materials for Community Shaders
// https://www.artstation.com/blogs/andreariccardi/3VPo/a-new-approach-for-parallax-mapping-presenting-the-contact-refinement-parallax-mapping-technique
float getTerrainHeight(float2 coords, float2 dx, float2 dy, float blendFactor, int texCount, sampler2D tex[7], float blends[7], out float weights[7]) {
    weights = blends;

    float blendPower = blendFactor * 4;
    float total = 0;
    [unroll] for (int i = 0; i < texCount; i++){
        weights[i] = pow(abs(blends[i]), 1 + 1 * blendFactor);
        if (weights[i] > 0.0) {
            weights[i] *= 0.001 + pow(abs(tex2Dgrad(tex[i], coords, dx, dy).a), blendPower);
        }
        total += weights[i];
    }
    
    float invtotal = rcp(total);
	
    [unroll] for (i = 0; i < texCount; i++)
    {
        weights[i] *= invtotal;
    }
    
    return pow(total, rcp(blendPower));
}
#endif

#ifdef TERRAIN
float2 getParallaxCoords(float distance, float2 coords, float2 dx, float2 dy, float3 viewDirTS, int texCount, sampler2D tex[7], float blends[7], out float weights[7]) {
#else
float2 getParallaxCoords(float distance, float2 coords, float2 dx, float2 dy, float3 viewDirTS, sampler2D heightMap) {
#endif
    #ifdef TERRAIN
        // Check if parallax is active first.
        if (!TESR_TerrainParallaxData.x) {
            weights = blends;
            return coords;
        }
    
        // Variables.
        bool highQuality = TESR_TerrainParallaxData.w;
        float maxDistance = TESR_TerrainParallaxExtraData.x;
        float height = TESR_TerrainParallaxExtraData.y;
    #else
        bool highQuality = false;
        float maxDistance = 2048;
        float height = 0.1 * TESR_ParallaxData.x;
    #endif
    
    float distanceBlend = saturate(distance / maxDistance);
    float quality = saturate(1.0 - distanceBlend);
    
    #ifdef TERRAIN
        float blendFactor = TESR_TerrainParallaxData.z ? quality : 0.25;
    #endif

    viewDirTS = normalize(viewDirTS);

    #ifdef TERRAIN
        // Fix for angles.
        viewDirTS.z = ((viewDirTS.z * 0.7) + 0.3);
        viewDirTS.xy /= viewDirTS.z;
    #endif

    float maxHeight = height;
    float minHeight = maxHeight * 0.5;

    float2 output;
    if (distanceBlend < 1.0)
    {
        int numSteps;
        
        if (highQuality) {
            numSteps = lerp(4, 64, quality);
            numSteps = clamp((numSteps / 4) * 4, 4, 64);
        }
        else {
            numSteps = lerp(4, 32, quality);
            numSteps = clamp((numSteps / 4) * 4, 4, 32);
        }

        float stepSize = rcp((float) numSteps);

        float2 offsetPerStep = viewDirTS.xy * float2(maxHeight, maxHeight) * stepSize.xx;
        float2 prevOffset = viewDirTS.xy * float2(minHeight, minHeight) + coords.xy;

        float prevBound = 1.0;
        float prevHeight = 1.0;

        float2 pt1 = 0;
        float2 pt2 = 0;

        // Need fastopt otherwise compile times are crazy.
        [loop][fastopt] while (numSteps > 0) {
            float4 currentOffset[2];
            currentOffset[0] = prevOffset.xyxy - float4(1, 1, 2, 2) * offsetPerStep.xyxy;
            currentOffset[1] = prevOffset.xyxy - float4(3, 3, 4, 4) * offsetPerStep.xyxy;
            float4 currentBound = prevBound.xxxx - float4(1, 2, 3, 4) * stepSize;

            float4 currHeight;
            
            #ifdef TERRAIN
                currHeight.x = getTerrainHeight(currentOffset[0].xy, dx, dy, blendFactor, texCount, tex, blends, weights);
                currHeight.y = getTerrainHeight(currentOffset[0].zw, dx, dy, blendFactor, texCount, tex, blends, weights);
                currHeight.z = getTerrainHeight(currentOffset[1].xy, dx, dy, blendFactor, texCount, tex, blends, weights);
                currHeight.w = getTerrainHeight(currentOffset[1].zw, dx, dy, blendFactor, texCount, tex, blends, weights);
            #else
                currHeight.x = tex2Dgrad(heightMap, currentOffset[0].xy, dx, dy).r;
                currHeight.y = tex2Dgrad(heightMap, currentOffset[0].zw, dx, dy).r;
                currHeight.z = tex2Dgrad(heightMap, currentOffset[1].xy, dx, dy).r;
                currHeight.w = tex2Dgrad(heightMap, currentOffset[1].zw, dx, dy).r;
            #endif

            bool4 testResult = currHeight >= currentBound;

            [branch] if (any(testResult))
            {
                [flatten] if (testResult.w)
                {
                    pt1 = float2(currentBound.w, currHeight.w);
                    pt2 = float2(currentBound.z, currHeight.z);
                }
                [flatten] if (testResult.z)
                {
                    pt1 = float2(currentBound.z, currHeight.z);
                    pt2 = float2(currentBound.y, currHeight.y);
                }
                [flatten] if (testResult.y)
                {
                    pt1 = float2(currentBound.y, currHeight.y);
                    pt2 = float2(currentBound.x, currHeight.x);
                }
                [flatten] if (testResult.x)
                {
                    pt1 = float2(currentBound.x, currHeight.x);
                    pt2 = float2(prevBound, prevHeight);
                }
                break;
            }

            prevOffset = currentOffset[1].zw;
            prevBound = currentBound.w;
            prevHeight = currHeight.w;
            numSteps -= 4;
        }
        
        float delta2 = pt2.x - pt2.y;
        float delta1 = pt1.x - pt1.y;

        float denominator = delta2 - delta1;

        float parallaxAmount = 0.0;
        if (denominator == 0.0)
        {
            parallaxAmount = 0.0;
        }
        else
        {
            parallaxAmount = (pt1.x * delta2 - pt2.x * delta1) / denominator;
        }
        
        distanceBlend *= distanceBlend;
        
        float offset = (1.0 - parallaxAmount) * -maxHeight + minHeight;
        return lerp(viewDirTS.xy * offset + coords.xy, coords, distanceBlend);
    }
    
    #ifdef TERRAIN
        weights = blends;
    #endif
    return coords;
}

#ifdef TERRAIN
float2 getParallaxCoordsWS(    //  -- terrain path – keeps weights[]
    float   distanceWS,
    float2  coords,
    float2  dxUV,   float2  dyUV,
    float3  viewDirWS,
    float3  T,      float3  B,      float3  N,
    float   signTB,
    int     texCount,
    sampler2D tex[7],
    float   blends[7],
    out float weights[7]
)
#else
float2 getParallaxCoordsWS(    //  -- non-terrain path
    float   distanceWS,
    float2  coords,
    float2  dxUV,   float2  dyUV,
    float3  viewDirWS,
    float3  T,      float3  B,      float3  N,
    float   signTB,
    sampler2D heightMap
)
#endif
{
    //──────────────── original constant handling ──────────────
#ifdef TERRAIN
    if (!TESR_TerrainParallaxData.x)    // parallax disabled
    {
        weights = blends;
        return coords;
    }

    bool   highQuality  = TESR_TerrainParallaxData.w;
    float  maxDistance  = TESR_TerrainParallaxExtraData.x;
    float  heightScale  = TESR_TerrainParallaxExtraData.y;
#else
    bool   highQuality  = false;
    float  maxDistance  = 2048.0;
    float  heightScale  = 0.1 * TESR_ParallaxData.x;
#endif
    //──────────────── world-space → tangent-space view vector ──
    float3 viewDirTS =
        float3( dot(viewDirWS, T),              // X
                dot(viewDirWS, B) * signTB,     // Y
                dot(viewDirWS, N) );            // Z

    viewDirTS = normalize(viewDirTS);

#ifdef TERRAIN
    // vanilla “angle fix”
    viewDirTS.z  =  viewDirTS.z * 0.7 + 0.3;
    viewDirTS.xy /= viewDirTS.z;
#endif
    //──────────────── quality / LOD handling ───────────────────
    float   distBlend = saturate(distanceWS / maxDistance);
    float   quality   = saturate(1.0 - distBlend);

#ifdef TERRAIN
    float blendFactor = TESR_TerrainParallaxData.z ? quality : 0.25;
#endif
    float   maxHeight = heightScale;
    float   minHeight = maxHeight * 0.5;

    //-----------------------------------------------------------
    //  Steep-parallax search  (code is verbatim from original,
    //  only 'viewDirTS' comes from world space now)
    //-----------------------------------------------------------
    if (distBlend < 1.0)
    {
        int numSteps;
        if (highQuality)
        {
            numSteps = lerp(4, 64, quality);
            numSteps = clamp((numSteps / 4) * 4, 4, 64);
        }
        else
        {
            numSteps = lerp(4, 32, quality);
            numSteps = clamp((numSteps / 4) * 4, 4, 32);
        }

        float  stepSize      = rcp((float)numSteps);
        float2 offsetPerStep = viewDirTS.xy * maxHeight * stepSize;
        float2 prevOffset    = viewDirTS.xy * minHeight + coords;

        float  prevBound  = 1.0;
        float  prevHeight = 1.0;
        float2 pt1 = 0, pt2 = 0;

        [loop][fastopt] while (numSteps > 0)
        {
            float4 curOff[2];
            curOff[0] = prevOffset.xyxy - float4(1,1,2,2) * offsetPerStep.xyxy;
            curOff[1] = prevOffset.xyxy - float4(3,3,4,4) * offsetPerStep.xyxy;
            float4 curBound = prevBound.xxxx - float4(1,2,3,4) * stepSize;

            float4 curHeight;

#ifdef TERRAIN
            curHeight.x = getTerrainHeight(curOff[0].xy, dxUV, dyUV,
                                           blendFactor, texCount, tex, blends, weights);
            curHeight.y = getTerrainHeight(curOff[0].zw, dxUV, dyUV,
                                           blendFactor, texCount, tex, blends, weights);
            curHeight.z = getTerrainHeight(curOff[1].xy, dxUV, dyUV,
                                           blendFactor, texCount, tex, blends, weights);
            curHeight.w = getTerrainHeight(curOff[1].zw, dxUV, dyUV,
                                           blendFactor, texCount, tex, blends, weights);
#else
            curHeight.x = tex2Dgrad(heightMap, curOff[0].xy, dxUV, dyUV).r;
            curHeight.y = tex2Dgrad(heightMap, curOff[0].zw, dxUV, dyUV).r;
            curHeight.z = tex2Dgrad(heightMap, curOff[1].xy, dxUV, dyUV).r;
            curHeight.w = tex2Dgrad(heightMap, curOff[1].zw, dxUV, dyUV).r;
#endif
            bool4 hit = curHeight >= curBound;

            [branch] if (any(hit))
            {
                if (hit.w) { pt1 = float2(curBound.w, curHeight.w);
                             pt2 = float2(curBound.z, curHeight.z); }
                else if (hit.z){pt1 = float2(curBound.z, curHeight.z);
                                pt2 = float2(curBound.y, curHeight.y); }
                else if (hit.y){pt1 = float2(curBound.y, curHeight.y);
                                pt2 = float2(curBound.x, curHeight.x); }
                else           {pt1 = float2(curBound.x, curHeight.x);
                                pt2 = float2(prevBound , prevHeight ); }
                break;
            }

            prevOffset  = curOff[1].zw;
            prevBound   = curBound.w;
            prevHeight  = curHeight.w;
            numSteps   -= 4;
        }

        float delta2 = pt2.x - pt2.y;
        float delta1 = pt1.x - pt1.y;
        float denom  = delta2 - delta1;
        float parallaxAmt = (denom == 0.0) ? 0.0 :
                            (pt1.x * delta2 - pt2.x * delta1) / denom;

        distBlend *= distBlend;          // smooth fade-out with distance

        float offset = (1.0 - parallaxAmt) * -maxHeight + minHeight;
        return lerp(viewDirTS.xy * offset + coords, coords, distBlend);
    }

#ifdef TERRAIN
    weights = blends;                    // no search ⇒ give caller raw blends
#endif
    return coords;
}
#ifdef TERRAIN
    float getParallaxShadowMultipler(float distance, float2 coords, float2 dx, float2 dy, float3 lightTS, int texCount, float blends[7], sampler2D tex[7]) {
        if (!TESR_TerrainParallaxData.y)
            return 1.0;
        
        float maxDistance = TESR_TerrainParallaxExtraData.x;
        float shadowsIntensity = TESR_TerrainParallaxExtraData.z;
        
        float quality = 1.0 - distance / maxDistance;
        
        if (quality > 0.0)
        {
            float weights[7] = { 0, 0, 0, 0, 0, 0, 0 };
            float sh0 = getTerrainHeight(coords, dx, dy, quality, texCount, tex, blends, weights);

            const float2 rayDir = lightTS.xy * 0.1;
            float4 multipliers = rcp((float4(1, 2, 3, 4)));

            float4 sh = getTerrainHeight(coords + rayDir * multipliers.x, dx, dy, quality, texCount, tex, blends, weights);
            if (quality > 0.25)
                sh.y = getTerrainHeight(coords + rayDir * multipliers.y, dx, dy, quality, texCount, tex, blends, weights);
            if (quality > 0.5)
                sh.z = getTerrainHeight(coords + rayDir * multipliers.z, dx, dy, quality, texCount, tex, blends, weights);
            if (quality > 0.75)
                sh.w = getTerrainHeight(coords + rayDir * multipliers.w, dx, dy, quality, texCount, tex, blends, weights);
            
            return 1.0 - saturate(dot(max(0, sh - sh0), 1.0) * shadowsIntensity) * quality;
        }
        
        return 1.0;
    }
    float getParallaxShadowMultiplerWS(
        float   distanceWS,
        float2  coords,
        float2  dxUV,  float2  dyUV,
        float3  lightWS,
        float3  T,     float3  B,
        float   signTB,
        int     texCount,
        float   blends[7],
        sampler2D tex[7]
    )
    {
        // --- user-tunable constants coming from CS registers ----------
        if (!TESR_TerrainParallaxData.y) return 1.0;        // shadows disabled

        const float maxDistance      = TESR_TerrainParallaxExtraData.x;
        const float shadowsIntensity = TESR_TerrainParallaxExtraData.z;
        // ----------------------------------------------------------------

        float q = 1.0 - distanceWS / maxDistance;           // quality LERP
        if (q <= 0.0) return 1.0;

        //----------------- build the UV–space shadow ray -----------------
        float2 rayDirUV =
            float2( dot(lightWS, T),
                    dot(lightWS, B) * signTB ) * 0.10;      // 0.10 == old scale
        //-----------------------------------------------------------------

        // reference height ­at starting point
        float   weightsTmp[7];
        float sh0 = getTerrainHeight(coords, dxUV, dyUV, q,
                                    texCount, tex, blends, weightsTmp);

        float4 m = rcp(float4(1,2,3,4));                    // {1,½,⅓,¼}
        float4 sh;

        sh.x = getTerrainHeight(coords + rayDirUV * m.x, dxUV, dyUV,
                                q, texCount, tex, blends, weightsTmp);

        if (q > 0.25)
            sh.y = getTerrainHeight(coords + rayDirUV * m.y, dxUV, dyUV,
                                    q, texCount, tex, blends, weightsTmp);
        if (q > 0.50)
            sh.z = getTerrainHeight(coords + rayDirUV * m.z, dxUV, dyUV,
                                    q, texCount, tex, blends, weightsTmp);
        if (q > 0.75)
            sh.w = getTerrainHeight(coords + rayDirUV * m.w, dxUV, dyUV,
                                    q, texCount, tex, blends, weightsTmp);

        float occ = saturate(dot(max(0, sh - sh0), 1.0) * shadowsIntensity);
        return 1.0 - occ * q;
    }
#else
    float getParallaxShadowMultipler(float distance, float2 coords, float2 dx, float2 dy, float3 lightTS, sampler2D heightMap) {
        float maxDistance = 2048;
        float shadowsIntensity = 2;
        
        float quality = 1.0 - distance / maxDistance;
        
        if (quality > 0.0)
        {
            float sh0 = tex2Dgrad(heightMap, coords, dx, dy).r;

            const float2 rayDir = lightTS.xy * 0.04;
            float4 multipliers = rcp((float4(1, 2, 3, 4)));

            float4 sh = tex2Dgrad(heightMap, coords + rayDir * multipliers.x, dx, dy).r;
            if (quality > 0.25)
                sh.y = tex2Dgrad(heightMap, coords + rayDir * multipliers.y, dx, dy).r;
            if (quality > 0.5)
                sh.z = tex2Dgrad(heightMap, coords + rayDir * multipliers.z, dx, dy).r;
            if (quality > 0.75)
                sh.w = tex2Dgrad(heightMap, coords + rayDir * multipliers.w, dx, dy).r;
            
            return 1.0 - saturate(dot(max(0, sh - sh0), 1.0) * shadowsIntensity) * quality;
        }
        
        return 1.0;
    }
    float getParallaxShadowMultiplerWS(
        float  distanceWS,
        float2 coords,                 // uv after parallax search
        float2 dxUV, float2 dyUV,      // gradients  (still needed for tex2Dgrad)
        float3 lightWS,                // unit, world space
        float3  T,  float3  B,         // world-space tangent & bitangent
        float   signTB,                //  +1 or –1  (handedness you stored in w)
        sampler2D heightMap
    )
    {
        const float maxDistance      = 2048.0;
        const float shadowsIntensity = 2.0;

        float q = 1.0 - distanceWS / maxDistance;
        if (q <= 0.0) return 1.0;

        // ---------- UV-space ray from TBN -------------------------------
        float2 rayDirUV = float2( dot(lightWS,  T),
                                dot(lightWS,  B) * signTB ) * 0.04;
        // ----------------------------------------------------------------

        float sh0 = tex2Dgrad(heightMap, coords, dxUV, dyUV).r;

        float4 m = rcp(float4(1,2,3,4));
        float4 sh;
        sh.x = tex2Dgrad(heightMap, coords + rayDirUV*m.x, dxUV, dyUV).r;
        if (q>0.25) sh.y = tex2Dgrad(heightMap, coords + rayDirUV*m.y, dxUV, dyUV).r;
        if (q>0.50) sh.z = tex2Dgrad(heightMap, coords + rayDirUV*m.z, dxUV, dyUV).r;
        if (q>0.75) sh.w = tex2Dgrad(heightMap, coords + rayDirUV*m.w, dxUV, dyUV).r;

        float occ = saturate(dot(max(0, sh - sh0), 1.0) * shadowsIntensity);
        return 1.0 - occ * q;
    }
#endif
