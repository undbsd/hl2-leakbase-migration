#include "BaseVSShader.h"
#include "convar.h"
#include "sdk_lightmappedgeneric_helper.h"

static LightmappedGeneric_Vars_t s_info;
static LightmappedGeneric_Vars_t s_info_editor;

BEGIN_VS_SHADER( SDK_WorldVertexTransition, "Help for SDK_WorldVertexTransition" )
    BEGIN_SHADER_PARAMS
        SHADER_PARAM( ALBEDO, SHADER_PARAM_TYPE_TEXTURE, "shadertest/BaseTexture", "albedo (Base texture with no baked lighting)" )
        SHADER_PARAM( SELFILLUMTINT, SHADER_PARAM_TYPE_COLOR, "[1 1 1]", "Self-illumination tint" )
        SHADER_PARAM( DETAIL, SHADER_PARAM_TYPE_TEXTURE, "shadertest/detail", "detail texture" )
        SHADER_PARAM( DETAILFRAME, SHADER_PARAM_TYPE_INTEGER, "0", "frame number for $detail" )
        SHADER_PARAM( DETAILSCALE, SHADER_PARAM_TYPE_FLOAT, "4", "scale of the detail texture" )

        // detail (multi-) texturing
        SHADER_PARAM( DETAILBLENDMODE, SHADER_PARAM_TYPE_INTEGER, "0", "mode for combining detail texture with base. 0=normal, 1= additive, 2=alpha blend detail over base, 3=crossfade" )
        SHADER_PARAM( DETAILBLENDFACTOR, SHADER_PARAM_TYPE_FLOAT, "1", "blend amount for detail texture." )
        SHADER_PARAM( DETAILTINT, SHADER_PARAM_TYPE_COLOR, "[1 1 1]", "detail texture tint" )

        SHADER_PARAM( ENVMAP, SHADER_PARAM_TYPE_TEXTURE, "shadertest/shadertest_env", "envmap" )
        SHADER_PARAM( ENVMAPFRAME, SHADER_PARAM_TYPE_INTEGER, "", "" )
        SHADER_PARAM( ENVMAPMASK, SHADER_PARAM_TYPE_TEXTURE, "shadertest/shadertest_envmask", "envmap mask" )
        SHADER_PARAM( ENVMAPMASKFRAME, SHADER_PARAM_TYPE_INTEGER, "", "" )
        SHADER_PARAM( ENVMAPMASKTRANSFORM, SHADER_PARAM_TYPE_MATRIX, "center .5 .5 scale 1 1 rotate 0 translate 0 0", "$envmapmask texcoord transform" )
        SHADER_PARAM( ENVMAPTINT, SHADER_PARAM_TYPE_COLOR, "[1 1 1]", "envmap tint" )
        SHADER_PARAM( BUMPMAP, SHADER_PARAM_TYPE_TEXTURE, "models/shadertest/shader1_normal", "bump map" )
        SHADER_PARAM( BUMPFRAME, SHADER_PARAM_TYPE_INTEGER, "0", "frame number for $bumpmap" )
        SHADER_PARAM( BUMPTRANSFORM, SHADER_PARAM_TYPE_MATRIX, "center .5 .5 scale 1 1 rotate 0 translate 0 0", "$bumpmap texcoord transform" )
        SHADER_PARAM( ENVMAPCONTRAST, SHADER_PARAM_TYPE_FLOAT, "0.0", "contrast 0 == normal 1 == color*color" )
        SHADER_PARAM( ENVMAPSATURATION, SHADER_PARAM_TYPE_FLOAT, "1.0", "saturation 0 == greyscale 1 == normal" )
        SHADER_PARAM( FRESNELREFLECTION, SHADER_PARAM_TYPE_FLOAT, "1.0", "1.0 == mirror, 0.0 == water" )
        SHADER_PARAM( NODIFFUSEBUMPLIGHTING, SHADER_PARAM_TYPE_INTEGER, "0", "0 == Use diffuse bump lighting, 1 = No diffuse bump lighting" )
        SHADER_PARAM( BUMPMAP2, SHADER_PARAM_TYPE_TEXTURE, "models/shadertest/shader3_normal", "bump map" )
        SHADER_PARAM( BUMPFRAME2, SHADER_PARAM_TYPE_INTEGER, "0", "frame number for $bumpmap" )
        SHADER_PARAM( BUMPTRANSFORM2, SHADER_PARAM_TYPE_MATRIX, "center .5 .5 scale 1 1 rotate 0 translate 0 0", "$bumpmap texcoord transform" )
        SHADER_PARAM( BUMPMASK, SHADER_PARAM_TYPE_TEXTURE, "models/shadertest/shader1_normal", "bump map" )
        SHADER_PARAM( BASETEXTURE2, SHADER_PARAM_TYPE_TEXTURE, "shadertest/detail", "detail texture" )
        SHADER_PARAM( FRAME2, SHADER_PARAM_TYPE_INTEGER, "0", "frame number for $basetexture2" )
        SHADER_PARAM( BASETEXTURENOENVMAP, SHADER_PARAM_TYPE_BOOL, "0", "" )
        SHADER_PARAM( BASETEXTURE2NOENVMAP, SHADER_PARAM_TYPE_BOOL, "0", "" )
        SHADER_PARAM( DETAIL_ALPHA_MASK_BASE_TEXTURE, SHADER_PARAM_TYPE_BOOL, "0", "If this is 1, then when detail alpha=0, no base texture is blended and when detail alpha=1, you get detail*base*lightmap" )
        SHADER_PARAM( LIGHTWARPTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "light munging lookup texture" )
        SHADER_PARAM( BLENDMODULATETEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "texture to use r/g channels for blend range for" )
        SHADER_PARAM( BLENDMASKTRANSFORM, SHADER_PARAM_TYPE_MATRIX, "center .5 .5 scale 1 1 rotate 0 translate 0 0", "$blendmodulatetexture texcoord transform" )
        SHADER_PARAM( MASKEDBLENDING, SHADER_PARAM_TYPE_INTEGER, "0", "blend using texture with no vertex alpha. For using texture blending on non-displacements" )
        SHADER_PARAM( SSBUMP, SHADER_PARAM_TYPE_INTEGER, "0", "whether or not to use alternate bumpmap format with height" )
        SHADER_PARAM( SEAMLESS_SCALE, SHADER_PARAM_TYPE_FLOAT, "0", "Scale factor for 'seamless' texture mapping. 0 means to use ordinary mapping" )

        SHADER_PARAM( ENVMAPORIGIN, SHADER_PARAM_TYPE_VEC3, "[0 0 0]", "Envmap parallax correction origin" )
        SHADER_PARAM( ENVMAPPARALLAX, SHADER_PARAM_TYPE_MATRIX, "[1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1]", "Envmap parallax correction matrix" )
    END_SHADER_PARAMS

    void SetupVars( LightmappedGeneric_Vars_t &info )
    {
        info.m_nBaseTexture = BASETEXTURE;
        info.m_nBaseTextureFrame = FRAME;
        info.m_nBaseTextureTransform = BASETEXTURETRANSFORM;
        info.m_nAlbedo = ALBEDO;
        info.m_nSelfIllumTint = SELFILLUMTINT;

        info.m_nDetail = DETAIL;
        info.m_nDetailFrame = DETAILFRAME;
        info.m_nDetailScale = DETAILSCALE;
        info.m_nDetailTextureCombineMode = DETAILBLENDMODE;
        info.m_nDetailTextureBlendFactor = DETAILBLENDFACTOR;
        info.m_nDetailTint = DETAILTINT;

        info.m_nEnvmap = ENVMAP;
        info.m_nEnvmapFrame = ENVMAPFRAME;
        info.m_nEnvmapMask = ENVMAPMASK;
        info.m_nEnvmapMaskFrame = ENVMAPMASKFRAME;
        info.m_nEnvmapMaskTransform = ENVMAPMASKTRANSFORM;
        info.m_nEnvmapTint = ENVMAPTINT;
        info.m_nBumpmap = BUMPMAP;
        info.m_nBumpFrame = BUMPFRAME;
        info.m_nBumpTransform = BUMPTRANSFORM;
        info.m_nEnvmapContrast = ENVMAPCONTRAST;
        info.m_nEnvmapSaturation = ENVMAPSATURATION;
        info.m_nFresnelReflection = FRESNELREFLECTION;
        info.m_nNoDiffuseBumpLighting = NODIFFUSEBUMPLIGHTING;
        info.m_nBumpmap2 = BUMPMAP2;
        info.m_nBumpFrame2 = BUMPFRAME2;
        info.m_nBaseTexture2 = BASETEXTURE2;
        info.m_nBaseTexture2Frame = FRAME2;
        info.m_nBumpTransform2 = BUMPTRANSFORM2;
        info.m_nBumpMask = BUMPMASK;
        info.m_nBaseTextureNoEnvmap = BASETEXTURENOENVMAP;
        info.m_nBaseTexture2NoEnvmap = BASETEXTURE2NOENVMAP;
        info.m_nDetailAlphaMaskBaseTexture = DETAIL_ALPHA_MASK_BASE_TEXTURE;
        info.m_nFlashlightTexture = FLASHLIGHTTEXTURE;
        info.m_nFlashlightTextureFrame = FLASHLIGHTTEXTUREFRAME;
        info.m_nLightWarpTexture = LIGHTWARPTEXTURE;
        info.m_nBlendModulateTexture = BLENDMODULATETEXTURE;
        info.m_nBlendMaskTransform = BLENDMASKTRANSFORM;
        info.m_nMaskedBlending = MASKEDBLENDING;
        info.m_nSelfShadowedBumpFlag = SSBUMP;
        info.m_nSeamlessMappingScale = SEAMLESS_SCALE;
        info.m_nAlphaTestReference = -1;

        info.m_nEnvMapOrigin = ENVMAPORIGIN;
        info.m_nEnvMapParallax = ENVMAPPARALLAX;
    }

    // Yoinked� from DownFall
    // All Hammer editions for 2013 engine branch (including H++)
    // have a hack that basically inverts the alpha values virtually
    // making a workaround for older WorldVertexTransition shaders...
    void SetupVarsEditor( LightmappedGeneric_Vars_t &info )
    {
        SetupVars( info );
        V_swap( info.m_nBaseTexture, info.m_nBaseTexture2 );
        V_swap( info.m_nBaseTextureFrame, info.m_nBaseTexture2Frame );
        V_swap( info.m_nBaseTextureNoEnvmap, info.m_nBaseTexture2NoEnvmap );
        V_swap( info.m_nBumpmap, info.m_nBumpmap2 );
        V_swap( info.m_nBumpFrame, info.m_nBumpFrame2 );
        V_swap( info.m_nBumpTransform, info.m_nBumpTransform2 );
    }

    SHADER_FALLBACK
    {
        if( !g_pHardwareConfig->SupportsShaderModel_3_0() )
            return "WorldVertexTransition";
        return 0;
    }

    SHADER_INIT_PARAMS()
    {
        SetupVars( s_info );
        SetupVarsEditor( s_info_editor );
        InitParamsLightmappedGeneric( this, params, pMaterialName, s_info );
    }

    SHADER_INIT
    {
        SetupVars( s_info );
        SetupVarsEditor( s_info_editor );
        InitLightmappedGeneric( this, params, s_info );
    }

    SHADER_DRAW
    {
        DrawLightmappedGeneric( this, params, pShaderAPI, pShaderShadow, UsingEditor( params ) ? s_info_editor : s_info, pContextDataPtr );
    }
END_SHADER
