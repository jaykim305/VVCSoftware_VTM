/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2020, ITU/ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __SEI__
#define __SEI__

#pragma once
#include <list>
#include <vector>
#include <cstring>

#include "CommonDef.h"
#include "libmd5/MD5.h"

//! \ingroup CommonLib
//! \{
class SPS;

/**
 * Abstract class representing an SEI message with lightweight RTTI.
 */
class SEI
{
public:
  enum PayloadType
  {
    BUFFERING_PERIOD                     = 0,
    PICTURE_TIMING                       = 1,
#if HEVC_SEI
    PAN_SCAN_RECT                        = 2,
#endif
    FILLER_PAYLOAD                       = 3,
#if HEVC_SEI || JVET_P0337_PORTING_SEI
    USER_DATA_REGISTERED_ITU_T_T35       = 4,
    USER_DATA_UNREGISTERED               = 5,
#if !JVET_P0337_PORTING_SEI
    RECOVERY_POINT                       = 6,
    SCENE_INFO                           = 9,
    FULL_FRAME_SNAPSHOT                  = 15,
    PROGRESSIVE_REFINEMENT_SEGMENT_START = 16,
    PROGRESSIVE_REFINEMENT_SEGMENT_END   = 17,
#endif
    FILM_GRAIN_CHARACTERISTICS           = 19,
#if !JVET_P0337_PORTING_SEI
    POST_FILTER_HINT                     = 22,
    TONE_MAPPING_INFO                    = 23,
#endif
    FRAME_PACKING                        = 45,
#if !JVET_P0337_PORTING_SEI
    DISPLAY_ORIENTATION                  = 47,
    GREEN_METADATA                       = 56,
    SOP_DESCRIPTION                      = 128,
    ACTIVE_PARAMETER_SETS                = 129,
#endif
#endif
    DECODING_UNIT_INFO                   = 130,
#if HEVC_SEI
    TEMPORAL_LEVEL0_INDEX                = 131,
#endif
    DECODED_PICTURE_HASH                 = 132,
#if HEVC_SEI || JVET_P0337_PORTING_SEI
#if !JVET_P0337_PORTING_SEI
    SCALABLE_NESTING                     = 133,
    REGION_REFRESH_INFO                  = 134,
    NO_DISPLAY                           = 135,
    TIME_CODE                            = 136,
#endif
    MASTERING_DISPLAY_COLOUR_VOLUME      = 137,
#if !JVET_P0337_PORTING_SEI
    SEGM_RECT_FRAME_PACKING              = 138,
    TEMP_MOTION_CONSTRAINED_TILE_SETS    = 139,
    CHROMA_RESAMPLING_FILTER_HINT        = 140,
    KNEE_FUNCTION_INFO                   = 141,
    COLOUR_REMAPPING_INFO                = 142,
#endif
#endif
    DEPENDENT_RAP_INDICATION             = 145,
#if JVET_P0462_SEI360
    EQUIRECTANGULAR_PROJECTION           = 150,
    SPHERE_ROTATION                      = 154,
    REGION_WISE_PACKING                  = 155,
    OMNI_VIEWPORT                        = 156,
#endif
#if JVET_P0597_GCMP_SEI
    GENERALIZED_CUBEMAP_PROJECTION       = 153,
#endif
#if HEVC_SEI && !JVET_P0337_PORTING_SEI
#if U0033_ALTERNATIVE_TRANSFER_CHARACTERISTICS_SEI
    ALTERNATIVE_TRANSFER_CHARACTERISTICS = 182,
#endif
#endif
    FRAME_FIELD_INFO                     = 168,
#if JVET_P0984_SEI_SUBPIC_LEVEL
    SUBPICTURE_LEVEL_INFO                = 203,
#endif
#if JVET_P0450_SEI_SARI
    SAMPLE_ASPECT_RATIO_INFO             = 204,
#endif
#if JVET_P0337_PORTING_SEI
    CONTENT_LIGHT_LEVEL_INFO             = 144,
    ALTERNATIVE_TRANSFER_CHARACTERISTICS = 147,
    AMBIENT_VIEWING_ENVIRONMENT          = 148,
    CONTENT_COLOUR_VOLUME                = 149,
#endif
  };

  SEI() {}
  virtual ~SEI() {}

  static const char *getSEIMessageString(SEI::PayloadType payloadType);

  virtual PayloadType payloadType() const = 0;
};


#if JVET_P0462_SEI360
class SEIEquirectangularProjection : public SEI
{
public:
  PayloadType payloadType() const { return EQUIRECTANGULAR_PROJECTION; }

  SEIEquirectangularProjection()  {}
  virtual ~SEIEquirectangularProjection() {}

  bool    m_erpCancelFlag;
  bool    m_erpPersistenceFlag;
  bool    m_erpGuardBandFlag;
  uint8_t m_erpGuardBandType;
  uint8_t m_erpLeftGuardBandWidth;
  uint8_t m_erpRightGuardBandWidth;
};

class SEISphereRotation : public SEI
{
public:
  PayloadType payloadType() const { return SPHERE_ROTATION; }

  SEISphereRotation()  {}
  virtual ~SEISphereRotation() {}

  bool  m_sphereRotationCancelFlag;
  bool  m_sphereRotationPersistenceFlag;
  int   m_sphereRotationYaw;
  int   m_sphereRotationPitch;
  int   m_sphereRotationRoll;
};

class SEIOmniViewport : public SEI
{
public:
  PayloadType payloadType() const { return OMNI_VIEWPORT; }

  SEIOmniViewport() {}
  virtual ~SEIOmniViewport() {}

  struct OmniViewport
  {
    int      azimuthCentre;
    int      elevationCentre;
    int      tiltCentre;
    uint32_t horRange;
    uint32_t verRange;
  };

  uint32_t m_omniViewportId;
  bool     m_omniViewportCancelFlag;
  bool     m_omniViewportPersistenceFlag;
  uint8_t  m_omniViewportCntMinus1;
  std::vector<OmniViewport> m_omniViewportRegions;  
};

class SEIRegionWisePacking : public SEI
{
public:
  PayloadType payloadType() const { return REGION_WISE_PACKING; }
  SEIRegionWisePacking() {}
  virtual ~SEIRegionWisePacking() {}
  bool                  m_rwpCancelFlag;
  bool                  m_rwpPersistenceFlag;
  bool                  m_constituentPictureMatchingFlag;
  int                   m_numPackedRegions;
  int                   m_projPictureWidth;
  int                   m_projPictureHeight;
  int                   m_packedPictureWidth;
  int                   m_packedPictureHeight;
  std::vector<uint8_t>  m_rwpTransformType;
  std::vector<bool>     m_rwpGuardBandFlag;
  std::vector<uint32_t> m_projRegionWidth;
  std::vector<uint32_t> m_projRegionHeight;
  std::vector<uint32_t> m_rwpProjRegionTop;
  std::vector<uint32_t> m_projRegionLeft;
  std::vector<uint16_t> m_packedRegionWidth;
  std::vector<uint16_t> m_packedRegionHeight;
  std::vector<uint16_t> m_packedRegionTop;
  std::vector<uint16_t> m_packedRegionLeft;
  std::vector<uint8_t>  m_rwpLeftGuardBandWidth;
  std::vector<uint8_t>  m_rwpRightGuardBandWidth;
  std::vector<uint8_t>  m_rwpTopGuardBandHeight;
  std::vector<uint8_t>  m_rwpBottomGuardBandHeight;
  std::vector<bool>     m_rwpGuardBandNotUsedForPredFlag;
  std::vector<uint8_t>  m_rwpGuardBandType;
};
#endif

#if JVET_P0597_GCMP_SEI
class SEIGeneralizedCubemapProjection : public SEI
{
public:
  PayloadType payloadType() const { return GENERALIZED_CUBEMAP_PROJECTION; }

  SEIGeneralizedCubemapProjection()  {}
  virtual ~SEIGeneralizedCubemapProjection() {}

  bool                 m_gcmpCancelFlag;
  bool                 m_gcmpPersistenceFlag;
  uint8_t              m_gcmpPackingType;
  uint8_t              m_gcmpMappingFunctionType;
  std::vector<uint8_t> m_gcmpFaceIndex;
  std::vector<uint8_t> m_gcmpFaceRotation;
  std::vector<uint8_t> m_gcmpFunctionCoeffU;
  std::vector<bool>    m_gcmpFunctionUAffectedByVFlag;
  std::vector<uint8_t> m_gcmpFunctionCoeffV;
  std::vector<bool>    m_gcmpFunctionVAffectedByUFlag;
  bool                 m_gcmpGuardBandFlag;
  bool                 m_gcmpGuardBandBoundaryType;
  uint8_t              m_gcmpGuardBandSamplesMinus1;
};
#endif

#if JVET_P0450_SEI_SARI
class SEISampleAspectRatioInfo : public SEI
{
public:
  PayloadType payloadType() const { return SAMPLE_ASPECT_RATIO_INFO; }
  SEISampleAspectRatioInfo() {}
  virtual ~SEISampleAspectRatioInfo() {}
  bool                  m_sariCancelFlag;
  bool                  m_sariPersistenceFlag;
  int                   m_sariAspectRatioIdc;
  int                   m_sariSarWidth;
  int                   m_sariSarHeight;
};
#endif

#if HEVC_SEI || JVET_P0337_PORTING_SEI
static const uint32_t ISO_IEC_11578_LEN=16;

class SEIuserDataUnregistered : public SEI
{
public:
  PayloadType payloadType() const { return USER_DATA_UNREGISTERED; }

  SEIuserDataUnregistered()
    : userData(0)
    {}

  virtual ~SEIuserDataUnregistered()
  {
    delete userData;
  }

  uint8_t uuid_iso_iec_11578[ISO_IEC_11578_LEN];
  uint32_t  userDataLength;
  uint8_t *userData;
};
#endif

class SEIDecodedPictureHash : public SEI
{
public:
  PayloadType payloadType() const { return DECODED_PICTURE_HASH; }

  SEIDecodedPictureHash() {}
  virtual ~SEIDecodedPictureHash() {}

  HashType method;

  PictureHash m_pictureHash;
};

class SEIDependentRAPIndication : public SEI
{
public:
  PayloadType payloadType() const { return DEPENDENT_RAP_INDICATION; }
  SEIDependentRAPIndication() { }

  virtual ~SEIDependentRAPIndication() { }
};

#if HEVC_SEI
class SEIActiveParameterSets : public SEI
{
public:
  PayloadType payloadType() const { return ACTIVE_PARAMETER_SETS; }

  SEIActiveParameterSets()
    : m_selfContainedCvsFlag(false)
    , m_noParameterSetUpdateFlag (false)
    , numSpsIdsMinus1        (0)
  {}
  virtual ~SEIActiveParameterSets() {}

  bool m_selfContainedCvsFlag;
  bool m_noParameterSetUpdateFlag;
  int numSpsIdsMinus1;
  std::vector<int> activeSeqParameterSetId;
};
#endif

class SEIBufferingPeriod : public SEI
{
public:
  PayloadType payloadType() const { return BUFFERING_PERIOD; }
  void copyTo (SEIBufferingPeriod& target) const;

  SEIBufferingPeriod()
  : m_bpNalCpbParamsPresentFlag (false)
  , m_bpVclCpbParamsPresentFlag (false)
  , m_initialCpbRemovalDelayLength (0)
  , m_cpbRemovalDelayLength (0)
  , m_dpbOutputDelayLength (0)
#if JVET_P0446_BP_CPB_CNT_FIX
  , m_bpCpbCnt(0)
#endif
  , m_duCpbRemovalDelayIncrementLength (0)
  , m_dpbOutputDelayDuLength (0)
  , m_cpbRemovalDelayDeltasPresentFlag (false)
  , m_numCpbRemovalDelayDeltas (0)
  , m_bpMaxSubLayers (0)
  , m_bpDecodingUnitHrdParamsPresentFlag (false)
  , m_decodingUnitCpbParamsInPicTimingSeiFlag (false)
#if JVET_P0181
    , m_sublayerInitialCpbRemovalDelayPresentFlag(false)
#endif
#if JVET_P0446_CONCATENATION
    , m_additionalConcatenationInfoPresentFlag (false)
    , m_maxInitialRemovalDelayForConcatenation (0)
#endif
#if JVET_P0446_ALT_CPB
    , m_altCpbParamsPresentFlag (false)
    , m_useAltCpbParamsFlag (false)
#endif
  {
    ::memset(m_initialCpbRemovalDelay, 0, sizeof(m_initialCpbRemovalDelay));
    ::memset(m_initialCpbRemovalOffset, 0, sizeof(m_initialCpbRemovalOffset));
    ::memset(m_cpbRemovalDelayDelta, 0, sizeof(m_cpbRemovalDelayDelta));
#if !JVET_P0446_BP_CPB_CNT_FIX
    ::memset(m_bpCpbCnt, 0, sizeof(m_bpCpbCnt));
#endif
  }
  virtual ~SEIBufferingPeriod() {}

  void      setDuCpbRemovalDelayIncrementLength( uint32_t value )        { m_duCpbRemovalDelayIncrementLength = value;        }
  uint32_t  getDuCpbRemovalDelayIncrementLength( ) const                 { return m_duCpbRemovalDelayIncrementLength;         }
  void      setDpbOutputDelayDuLength( uint32_t value )                  { m_dpbOutputDelayDuLength = value;                  }
  uint32_t  getDpbOutputDelayDuLength( ) const                           { return m_dpbOutputDelayDuLength;                   }
  bool m_bpNalCpbParamsPresentFlag;
  bool m_bpVclCpbParamsPresentFlag;
  uint32_t m_initialCpbRemovalDelayLength;
  uint32_t m_cpbRemovalDelayLength;
  uint32_t m_dpbOutputDelayLength;
#if !JVET_P0446_BP_CPB_CNT_FIX
  int      m_bpCpbCnt[MAX_TLAYER];
#else
  int      m_bpCpbCnt;
#endif
  uint32_t m_duCpbRemovalDelayIncrementLength;
  uint32_t m_dpbOutputDelayDuLength;
  uint32_t m_initialCpbRemovalDelay         [MAX_TLAYER][MAX_CPB_CNT][2];
  uint32_t m_initialCpbRemovalOffset        [MAX_TLAYER][MAX_CPB_CNT][2];
  bool m_concatenationFlag;
  uint32_t m_auCpbRemovalDelayDelta;
  bool m_cpbRemovalDelayDeltasPresentFlag;
  int  m_numCpbRemovalDelayDeltas;
  int  m_bpMaxSubLayers;
  uint32_t m_cpbRemovalDelayDelta    [15];
  bool m_bpDecodingUnitHrdParamsPresentFlag;
  bool m_decodingUnitCpbParamsInPicTimingSeiFlag;
#if JVET_P0181
  bool m_sublayerInitialCpbRemovalDelayPresentFlag;
#endif
#if JVET_P0446_CONCATENATION
  bool     m_additionalConcatenationInfoPresentFlag;
  uint32_t m_maxInitialRemovalDelayForConcatenation;
#endif
#if JVET_P0446_ALT_CPB
  bool     m_altCpbParamsPresentFlag;
  bool     m_useAltCpbParamsFlag;
#endif
};

class SEIPictureTiming : public SEI
{
public:
  PayloadType payloadType() const { return PICTURE_TIMING; }
  void copyTo (SEIPictureTiming& target) const;

  SEIPictureTiming()
  : m_picDpbOutputDelay (0)
  , m_picDpbOutputDuDelay (0)
  , m_numDecodingUnitsMinus1 (0)
  , m_duCommonCpbRemovalDelayFlag (false)
#if JVET_P0446_ALT_CPB
  , m_cpbAltTimingInfoPresentFlag (false)
  , m_cpbDelayOffset (0)
  , m_dpbDelayOffset (0)
#endif
  {
    ::memset(m_ptSubLayerDelaysPresentFlag, 0, sizeof(m_ptSubLayerDelaysPresentFlag));
    ::memset(m_duCommonCpbRemovalDelayMinus1, 0, sizeof(m_duCommonCpbRemovalDelayMinus1));
    ::memset(m_cpbRemovalDelayDeltaEnabledFlag, 0, sizeof(m_cpbRemovalDelayDeltaEnabledFlag));
    ::memset(m_cpbRemovalDelayDeltaIdx, 0, sizeof(m_cpbRemovalDelayDeltaIdx));
    ::memset(m_auCpbRemovalDelay, 0, sizeof(m_auCpbRemovalDelay));
  }
  virtual ~SEIPictureTiming()
  {
  }


  bool  m_ptSubLayerDelaysPresentFlag[MAX_TLAYER];
  bool  m_cpbRemovalDelayDeltaEnabledFlag[MAX_TLAYER];
  uint32_t  m_cpbRemovalDelayDeltaIdx[MAX_TLAYER];
  uint32_t  m_auCpbRemovalDelay[MAX_TLAYER];
  uint32_t  m_picDpbOutputDelay;
  uint32_t  m_picDpbOutputDuDelay;
  uint32_t  m_numDecodingUnitsMinus1;
  bool  m_duCommonCpbRemovalDelayFlag;
  uint32_t  m_duCommonCpbRemovalDelayMinus1[MAX_TLAYER];
  std::vector<uint32_t> m_numNalusInDuMinus1;
  std::vector<uint32_t> m_duCpbRemovalDelayMinus1;
#if JVET_P0446_ALT_CPB
  bool     m_cpbAltTimingInfoPresentFlag;
  std::vector<uint32_t> m_cpbAltInitialCpbRemovalDelayDelta;
  std::vector<uint32_t> m_cpbAltInitialCpbRemovalOffsetDelta;
  uint32_t m_cpbDelayOffset;
  uint32_t m_dpbDelayOffset;
#endif
};

class SEIDecodingUnitInfo : public SEI
{
public:
  PayloadType payloadType() const { return DECODING_UNIT_INFO; }

  SEIDecodingUnitInfo()
    : m_decodingUnitIdx(0)
    , m_dpbOutputDuDelayPresentFlag(false)
    , m_picSptDpbOutputDuDelay(0)
  {
    ::memset(m_duiSubLayerDelaysPresentFlag, 0, sizeof(m_duiSubLayerDelaysPresentFlag));
    ::memset(m_duSptCpbRemovalDelayIncrement, 0, sizeof(m_duSptCpbRemovalDelayIncrement));
  }
  virtual ~SEIDecodingUnitInfo() {}
  int m_decodingUnitIdx;
  bool m_duiSubLayerDelaysPresentFlag[MAX_TLAYER];
  int m_duSptCpbRemovalDelayIncrement[MAX_TLAYER];
  bool m_dpbOutputDuDelayPresentFlag;
  int m_picSptDpbOutputDuDelay;
};


class SEIFrameFieldInfo : public SEI
{
public:
  PayloadType payloadType() const { return FRAME_FIELD_INFO; }

  SEIFrameFieldInfo()
    : m_fieldPicFlag(false)
    , m_bottomFieldFlag (false)
    , m_pairingIndicatedFlag (false)
    , m_pairedWithNextFieldFlag(false)
    , m_displayFieldsFromFrameFlag(false)
    , m_topFieldFirstFlag(false)
    , m_displayElementalPeriodsMinus1(0)
    , m_sourceScanType(0)
    , m_duplicateFlag(false)
  {}
  virtual ~SEIFrameFieldInfo() {}

  bool m_fieldPicFlag;
  bool m_bottomFieldFlag;
  bool m_pairingIndicatedFlag;
  bool m_pairedWithNextFieldFlag;
  bool m_displayFieldsFromFrameFlag;
  bool m_topFieldFirstFlag;
  int  m_displayElementalPeriodsMinus1;
  int  m_sourceScanType;
  bool m_duplicateFlag;
};

#if HEVC_SEI || JVET_P0337_PORTING_SEI
#if !JVET_P0337_PORTING_SEI
class SEIRecoveryPoint : public SEI
{
public:
  PayloadType payloadType() const { return RECOVERY_POINT; }

  SEIRecoveryPoint() {}
  virtual ~SEIRecoveryPoint() {}

  int  m_recoveryPocCnt;
  bool m_exactMatchingFlag;
  bool m_brokenLinkFlag;
};
#endif

class SEIFramePacking : public SEI
{
public:
  PayloadType payloadType() const { return FRAME_PACKING; }

  SEIFramePacking() {}
  virtual ~SEIFramePacking() {}

  int  m_arrangementId;
  bool m_arrangementCancelFlag;
  int  m_arrangementType;
  bool m_quincunxSamplingFlag;
  int  m_contentInterpretationType;
  bool m_spatialFlippingFlag;
  bool m_frame0FlippedFlag;
  bool m_fieldViewsFlag;
  bool m_currentFrameIsFrame0Flag;
  bool m_frame0SelfContainedFlag;
  bool m_frame1SelfContainedFlag;
  int  m_frame0GridPositionX;
  int  m_frame0GridPositionY;
  int  m_frame1GridPositionX;
  int  m_frame1GridPositionY;
  int  m_arrangementReservedByte;
  bool m_arrangementPersistenceFlag;
  bool m_upsampledAspectRatio;
};

#if !JVET_P0337_PORTING_SEI
class SEISegmentedRectFramePacking : public SEI
{
public:
  PayloadType payloadType() const { return SEGM_RECT_FRAME_PACKING; }

  SEISegmentedRectFramePacking() {}
  virtual ~SEISegmentedRectFramePacking() {}

  bool m_arrangementCancelFlag;
  int  m_contentInterpretationType;
  bool m_arrangementPersistenceFlag;
};

class SEIDisplayOrientation : public SEI
{
public:
  PayloadType payloadType() const { return DISPLAY_ORIENTATION; }

  SEIDisplayOrientation()
    : cancelFlag(true)
    , persistenceFlag(0)
    , extensionFlag(false)
    {}
  virtual ~SEIDisplayOrientation() {}

  bool cancelFlag;
  bool horFlip;
  bool verFlip;

  uint32_t anticlockwiseRotation;
  bool persistenceFlag;
  bool extensionFlag;
};

class SEITemporalLevel0Index : public SEI
{
public:
  PayloadType payloadType() const { return TEMPORAL_LEVEL0_INDEX; }

  SEITemporalLevel0Index()
    : tl0Idx(0)
    , rapIdx(0)
    {}
  virtual ~SEITemporalLevel0Index() {}

  uint32_t tl0Idx;
  uint32_t rapIdx;
};

class SEIGradualDecodingRefreshInfo : public SEI
{
public:
  PayloadType payloadType() const { return REGION_REFRESH_INFO; }

  SEIGradualDecodingRefreshInfo()
    : m_gdrForegroundFlag(0)
  {}
  virtual ~SEIGradualDecodingRefreshInfo() {}

  bool m_gdrForegroundFlag;
};

class SEINoDisplay : public SEI
{
public:
  PayloadType payloadType() const { return NO_DISPLAY; }

  SEINoDisplay()
    : m_noDisplay(false)
  {}
  virtual ~SEINoDisplay() {}

  bool m_noDisplay;
};

class SEISOPDescription : public SEI
{
public:
  PayloadType payloadType() const { return SOP_DESCRIPTION; }

  SEISOPDescription() {}
  virtual ~SEISOPDescription() {}

  uint32_t m_sopSeqParameterSetId;
  uint32_t m_numPicsInSopMinus1;

  uint32_t m_sopDescVclNaluType[MAX_NUM_PICS_IN_SOP];
  uint32_t m_sopDescTemporalId[MAX_NUM_PICS_IN_SOP];
  uint32_t m_sopDescStRpsIdx[MAX_NUM_PICS_IN_SOP];
  int m_sopDescPocDelta[MAX_NUM_PICS_IN_SOP];
};

class SEIToneMappingInfo : public SEI
{
public:
  PayloadType payloadType() const { return TONE_MAPPING_INFO; }
  SEIToneMappingInfo() {}
  virtual ~SEIToneMappingInfo() {}

  int    m_toneMapId;
  bool   m_toneMapCancelFlag;
  bool   m_toneMapPersistenceFlag;
  int    m_codedDataBitDepth;
  int    m_targetBitDepth;
  int    m_modelId;
  int    m_minValue;
  int    m_maxValue;
  int    m_sigmoidMidpoint;
  int    m_sigmoidWidth;
  std::vector<int> m_startOfCodedInterval;
  int    m_numPivots;
  std::vector<int> m_codedPivotValue;
  std::vector<int> m_targetPivotValue;
  int    m_cameraIsoSpeedIdc;
  int    m_cameraIsoSpeedValue;
  int    m_exposureIndexIdc;
  int    m_exposureIndexValue;
  bool   m_exposureCompensationValueSignFlag;
  int    m_exposureCompensationValueNumerator;
  int    m_exposureCompensationValueDenomIdc;
  int    m_refScreenLuminanceWhite;
  int    m_extendedRangeWhiteLevel;
  int    m_nominalBlackLevelLumaCodeValue;
  int    m_nominalWhiteLevelLumaCodeValue;
  int    m_extendedWhiteLevelLumaCodeValue;
};

class SEIKneeFunctionInfo : public SEI
{
public:
  PayloadType payloadType() const { return KNEE_FUNCTION_INFO; }
  SEIKneeFunctionInfo() {}
  virtual ~SEIKneeFunctionInfo() {}

  int   m_kneeId;
  bool  m_kneeCancelFlag;
  bool  m_kneePersistenceFlag;
  int   m_kneeInputDrange;
  int   m_kneeInputDispLuminance;
  int   m_kneeOutputDrange;
  int   m_kneeOutputDispLuminance;
  int   m_kneeNumKneePointsMinus1;
  std::vector<int> m_kneeInputKneePoint;
  std::vector<int> m_kneeOutputKneePoint;
};

class SEIColourRemappingInfo : public SEI
{
public:

  struct CRIlut
  {
    int codedValue;
    int targetValue;
    bool operator < (const CRIlut& a) const
    {
      return codedValue < a.codedValue;
    }
  };

  PayloadType payloadType() const { return COLOUR_REMAPPING_INFO; }
  SEIColourRemappingInfo() {}
  ~SEIColourRemappingInfo() {}

  void copyFrom( const SEIColourRemappingInfo &seiCriInput)
  {
    (*this) = seiCriInput;
  }

  uint32_t                m_colourRemapId;
  bool                m_colourRemapCancelFlag;
  bool                m_colourRemapPersistenceFlag;
  bool                m_colourRemapVideoSignalInfoPresentFlag;
  bool                m_colourRemapFullRangeFlag;
  int                 m_colourRemapPrimaries;
  int                 m_colourRemapTransferFunction;
  int                 m_colourRemapMatrixCoefficients;
  int                 m_colourRemapInputBitDepth;
  int                 m_colourRemapBitDepth;
  int                 m_preLutNumValMinus1[3];
  std::vector<CRIlut> m_preLut[3];
  bool                m_colourRemapMatrixPresentFlag;
  int                 m_log2MatrixDenom;
  int                 m_colourRemapCoeffs[3][3];
  int                 m_postLutNumValMinus1[3];
  std::vector<CRIlut> m_postLut[3];
};

class SEIChromaResamplingFilterHint : public SEI
{
public:
  PayloadType payloadType() const {return CHROMA_RESAMPLING_FILTER_HINT;}
  SEIChromaResamplingFilterHint() {}
  virtual ~SEIChromaResamplingFilterHint() {}

  int                            m_verChromaFilterIdc;
  int                            m_horChromaFilterIdc;
  bool                           m_verFilteringFieldProcessingFlag;
  int                            m_targetFormatIdc;
  bool                           m_perfectReconstructionFlag;
  std::vector<std::vector<int> > m_verFilterCoeff;
  std::vector<std::vector<int> > m_horFilterCoeff;
};
#endif
class SEIMasteringDisplayColourVolume : public SEI
{
public:
    PayloadType payloadType() const { return MASTERING_DISPLAY_COLOUR_VOLUME; }
    SEIMasteringDisplayColourVolume() {}
    virtual ~SEIMasteringDisplayColourVolume(){}

    SEIMasteringDisplay values;
};
#endif

typedef std::list<SEI*> SEIMessages;

/// output a selection of SEI messages by payload type. Ownership stays in original message list.
SEIMessages getSeisByType(SEIMessages &seiList, SEI::PayloadType seiType);

/// remove a selection of SEI messages by payload type from the original list and return them in a new list.
SEIMessages extractSeisByType(SEIMessages &seiList, SEI::PayloadType seiType);

/// delete list of SEI messages (freeing the referenced objects)
void deleteSEIs (SEIMessages &seiList);

#if HEVC_SEI
class SEIScalableNesting : public SEI
{
public:
  PayloadType payloadType() const { return SCALABLE_NESTING; }

  SEIScalableNesting() {}

  virtual ~SEIScalableNesting()
  {
    deleteSEIs(m_nestedSEIs);
  }

  bool  m_bitStreamSubsetFlag;
  bool  m_nestingOpFlag;
  bool  m_defaultOpFlag;                             //value valid if m_nestingOpFlag != 0
  uint32_t  m_nestingNumOpsMinus1;                       // -"-
  uint32_t  m_nestingMaxTemporalIdPlus1[MAX_TLAYER];     // -"-
  uint32_t  m_nestingOpIdx[MAX_NESTING_NUM_OPS];         // -"-

  bool  m_allLayersFlag;                             //value valid if m_nestingOpFlag == 0
  uint32_t  m_nestingNoOpMaxTemporalIdPlus1;             //value valid if m_nestingOpFlag == 0 and m_allLayersFlag == 0
  uint32_t  m_nestingNumLayersMinus1;                    //value valid if m_nestingOpFlag == 0 and m_allLayersFlag == 0
  uint8_t m_nestingLayerId[MAX_NESTING_NUM_LAYER];     //value valid if m_nestingOpFlag == 0 and m_allLayersFlag == 0. This can e.g. be a static array of 64 uint8_t values

  SEIMessages m_nestedSEIs;
};

class SEITimeCode : public SEI
{
public:
  PayloadType payloadType() const { return TIME_CODE; }
  SEITimeCode() {}
  virtual ~SEITimeCode(){}

  uint32_t numClockTs;
  SEITimeSet timeSetArray[MAX_TIMECODE_SEI_SETS];
};

//definition according to P1005_v1;
class SEITempMotionConstrainedTileSets: public SEI
{
  struct TileSetData
  {
    protected:
      std::vector<int> m_top_left_tile_index;  //[tileSetIdx][tileIdx];
      std::vector<int> m_bottom_right_tile_index;

    public:
      int     m_mcts_id;
      bool    m_display_tile_set_flag;
      bool    m_exact_sample_value_match_flag;
      bool    m_mcts_tier_level_idc_present_flag;
      bool    m_mcts_tier_flag;
      int     m_mcts_level_idc;

      void setNumberOfTileRects(const int number)
      {
        m_top_left_tile_index    .resize(number);
        m_bottom_right_tile_index.resize(number);
      }

      int  getNumberOfTileRects() const
      {
        CHECK(m_top_left_tile_index.size() != m_bottom_right_tile_index.size(), "Inconsistent tile arrangement");
        return int(m_top_left_tile_index.size());
      }

            int &topLeftTileIndex    (const int tileRectIndex)       { return m_top_left_tile_index    [tileRectIndex]; }
            int &bottomRightTileIndex(const int tileRectIndex)       { return m_bottom_right_tile_index[tileRectIndex]; }
      const int &topLeftTileIndex    (const int tileRectIndex) const { return m_top_left_tile_index    [tileRectIndex]; }
      const int &bottomRightTileIndex(const int tileRectIndex) const { return m_bottom_right_tile_index[tileRectIndex]; }
  };

protected:
  std::vector<TileSetData> m_tile_set_data;

public:

  bool    m_mc_all_tiles_exact_sample_value_match_flag;
  bool    m_each_tile_one_tile_set_flag;
  bool    m_limited_tile_set_display_flag;
  bool    m_max_mcs_tier_level_idc_present_flag;
  bool    m_max_mcts_tier_flag;
  int     m_max_mcts_level_idc;

  PayloadType payloadType() const { return TEMP_MOTION_CONSTRAINED_TILE_SETS; }

  void setNumberOfTileSets(const int number)       { m_tile_set_data.resize(number);     }
  int  getNumberOfTileSets()                 const { return int(m_tile_set_data.size()); }

        TileSetData &tileSetData (const int index)       { return m_tile_set_data[index]; }
  const TileSetData &tileSetData (const int index) const { return m_tile_set_data[index]; }

};
#endif

#if ENABLE_TRACING
void xTraceSEIHeader();
void xTraceSEIMessageType( SEI::PayloadType payloadType );
#endif

#if HEVC_SEI || JVET_P0337_PORTING_SEI
#if U0033_ALTERNATIVE_TRANSFER_CHARACTERISTICS_SEI 
class SEIAlternativeTransferCharacteristics : public SEI
{
public:
  PayloadType payloadType() const { return ALTERNATIVE_TRANSFER_CHARACTERISTICS; }

  SEIAlternativeTransferCharacteristics() : m_preferredTransferCharacteristics(18)
  { }

  virtual ~SEIAlternativeTransferCharacteristics() {}

  uint32_t m_preferredTransferCharacteristics;
};
#endif
#if !JVET_P0337_PORTING_SEI
class SEIGreenMetadataInfo : public SEI
{
public:
    PayloadType payloadType() const { return GREEN_METADATA; }
    SEIGreenMetadataInfo() {}

    virtual ~SEIGreenMetadataInfo() {}

    uint32_t m_greenMetadataType;
    uint32_t m_xsdMetricType;
    uint32_t m_xsdMetricValue;
};
#endif
#endif
#if JVET_P0337_PORTING_SEI
class SEIUserDataRegistered : public SEI
{
public:
  PayloadType payloadType() const { return USER_DATA_REGISTERED_ITU_T_T35; }

  SEIUserDataRegistered() {}
  virtual ~SEIUserDataRegistered() {}

  uint16_t m_ituCountryCode;
  std::vector<uint8_t> m_userData;
};

class SEIFilmGrainCharacteristics : public SEI
{
public:
  PayloadType payloadType() const { return FILM_GRAIN_CHARACTERISTICS; }

  SEIFilmGrainCharacteristics() {}
  virtual ~SEIFilmGrainCharacteristics() {}

  bool        m_filmGrainCharacteristicsCancelFlag;
  uint8_t     m_filmGrainModelId;
  bool        m_separateColourDescriptionPresentFlag;
  uint8_t     m_filmGrainBitDepthLumaMinus8;
  uint8_t     m_filmGrainBitDepthChromaMinus8;
  bool        m_filmGrainFullRangeFlag;
  uint8_t     m_filmGrainColourPrimaries;
  uint8_t     m_filmGrainTransferCharacteristics;
  uint8_t     m_filmGrainMatrixCoeffs;
  uint8_t     m_blendingModeId;
  uint8_t     m_log2ScaleFactor;

  struct CompModelIntensityValues
  {
    uint8_t intensityIntervalLowerBound;
    uint8_t intensityIntervalUpperBound;
    std::vector<int> compModelValue;
  };

  struct CompModel
  {
    bool  presentFlag;
    uint8_t numModelValues;
    std::vector<CompModelIntensityValues> intensityValues;
  };

  CompModel m_compModel[MAX_NUM_COMPONENT];
  bool      m_filmGrainCharacteristicsPersistenceFlag;
};

class SEIContentLightLevelInfo : public SEI
{
public:
  PayloadType payloadType() const { return CONTENT_LIGHT_LEVEL_INFO; }
  SEIContentLightLevelInfo() { }

  virtual ~SEIContentLightLevelInfo() { }

  uint32_t m_maxContentLightLevel;
  uint32_t m_maxPicAverageLightLevel;
};

class SEIAmbientViewingEnvironment : public SEI
{
public:
  PayloadType payloadType() const { return AMBIENT_VIEWING_ENVIRONMENT; }
  SEIAmbientViewingEnvironment() { }

  virtual ~SEIAmbientViewingEnvironment() { }

  uint32_t m_ambientIlluminance;
  uint16_t m_ambientLightX;
  uint16_t m_ambientLightY;
};

class SEIContentColourVolume : public SEI
{
public:
  PayloadType payloadType() const { return CONTENT_COLOUR_VOLUME; }
  SEIContentColourVolume() {}
  virtual ~SEIContentColourVolume() {}

  bool      m_ccvCancelFlag;
  bool      m_ccvPersistenceFlag;
  bool      m_ccvPrimariesPresentFlag;
  bool      m_ccvMinLuminanceValuePresentFlag;
  bool      m_ccvMaxLuminanceValuePresentFlag;
  bool      m_ccvAvgLuminanceValuePresentFlag;
  int       m_ccvPrimariesX[MAX_NUM_COMPONENT];
  int       m_ccvPrimariesY[MAX_NUM_COMPONENT];
  uint32_t  m_ccvMinLuminanceValue;
  uint32_t  m_ccvMaxLuminanceValue;
  uint32_t  m_ccvAvgLuminanceValue;
};

#endif
#endif

#if JVET_P0984_SEI_SUBPIC_LEVEL

class SEISubpicureLevelInfo : public SEI
{
public:
  PayloadType payloadType() const { return SUBPICTURE_LEVEL_INFO; }
  SEISubpicureLevelInfo()
  : m_sliSeqParameterSetId(0)
  , m_numRefLevels(0)
  , m_explicitFractionPresentFlag (false)
  {}
  virtual ~SEISubpicureLevelInfo() {}

  int       m_sliSeqParameterSetId;
  int       m_numRefLevels;
  bool      m_explicitFractionPresentFlag;
  std::vector<Level::Name>      m_refLevelIdc;
  std::vector<std::vector<int>> m_refLevelFraction;
};


#endif


//! \}


