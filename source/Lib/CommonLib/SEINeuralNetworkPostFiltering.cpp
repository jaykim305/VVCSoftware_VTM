/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2023, ITU/ISO/IEC
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

/** \file    SEINeuralNetworkPostFiltering.cpp
   \brief    SEI NN post filtering (application) class
*/

#include "CommonLib/CommonDef.h"
#include <cmath>
#if JVET_AE0126_NNPF_EDITORIAL_CHANGES
#include "CommonLib/SEI.h"
#endif
#include "SEINeuralNetworkPostFiltering.h"


SEINeuralNetworkPostFiltering::SEINeuralNetworkPostFiltering()
: m_picList()
{

}

void SEINeuralNetworkPostFiltering::setPicActivatedNnpfc(Picture* picture)
{
  SEIMessages seiList = getSeisByType(picture->SEIs, SEI::PayloadType::NEURAL_NETWORK_POST_FILTER_ACTIVATION);
  std::map<uint32_t, bool> tmpIsNnpfActivatedForPic;
  for (auto sei : seiList)
  {
    auto nnpfa = (SEINeuralNetworkPostFilterActivation*) sei;
    auto nnpfc = getNnpfcWithGivenId(m_clvsNnpfcSEIs, nnpfa->m_targetId);
    CHECK(nullptr == nnpfc, "There must be a NNPFC with nnpfc_id equal to nnpfa_target_id");
    if (nnpfa->m_cancelFlag)
    {
      tmpIsNnpfActivatedForPic[nnpfa->m_targetId] = false;
      m_isNnpfActiveForCLVS[nnpfa->m_targetId]    = false;
    }
    else
    {
      if (nnpfa->m_persistenceFlag)
      {
        m_isNnpfActiveForCLVS[nnpfa->m_targetId] = true;
      }
      else
      {
        tmpIsNnpfActivatedForPic[nnpfa->m_targetId] = true;
#if JVET_AE0048_IMPLICIT_PERSISTENCE_CANCEL
        m_isNnpfActiveForCLVS[nnpfa->m_targetId] = false;  // Cancel the persistence of an NNPFA SEI message with any subsequent NNPFA SEI message with the same nnpfa_target_id
#endif
      }
      CHECK((uint32_t)nnpfa->m_outputFlag.size() > nnpfc->m_numInpPicsInOutputTensor, "The value of nnpfa_num_output_entries shall be in the range of 0 to NumInpPicsInOutputTensor");
    }
  }

  std::map<uint32_t, bool> isNnpfActivactedForPic = m_isNnpfActiveForCLVS;
  for (auto it: tmpIsNnpfActivatedForPic)
  {
    isNnpfActivactedForPic.insert_or_assign(it.first, it.second);
  }
  
  for (auto it : isNnpfActivactedForPic)
  {
    if (it.second)
    {
      auto nnpfc = getNnpfcWithGivenId(m_clvsNnpfcSEIs, it.first);
      picture->m_nnpfcActivated.push_back(new SEINeuralNetworkPostFilterCharacteristics(*nnpfc));
    }
  }
}

void SEINeuralNetworkPostFiltering::filterPictures(PicList& picList)
{
  m_picList = PicVector(picList.begin(), picList.end());
  for (Picture* currCodedPic: m_picList)
  {
    const NalUnitType picType = currCodedPic->getPictureType();

    if (picType == NAL_UNIT_CODED_SLICE_IDR_N_LP || picType == NAL_UNIT_CODED_SLICE_IDR_W_RADL
        || picType == NAL_UNIT_CODED_SLICE_CRA || picType == NAL_UNIT_CODED_SLICE_GDR)
    {
      m_clvsNnpfcSEIs = getSeisByType(currCodedPic->SEIs, SEI::PayloadType::NEURAL_NETWORK_POST_FILTER_CHARACTERISTICS);

#if JVET_AE0142_NNPF_CONSTRAINT_BUGFIXES
      for (std::list<SEI*>::iterator it = m_clvsNnpfcSEIs.begin(); it != m_clvsNnpfcSEIs.end(); it++)
      {
        SEINeuralNetworkPostFilterCharacteristics *nnpfcSEI = (SEINeuralNetworkPostFilterCharacteristics*)(*it);
        for (std::list<SEI*>::iterator it2 = it; it2 != m_clvsNnpfcSEIs.end(); it2++)
        {
          SEINeuralNetworkPostFilterCharacteristics *nnpfcSEI2 = (SEINeuralNetworkPostFilterCharacteristics*)(*it2);

          CHECK(nnpfcSEI->m_id == nnpfcSEI2->m_id && nnpfcSEI->m_purpose != nnpfcSEI2->m_purpose, "All NNPFC SEI messages with a particular value of nnpfc_id within a CLVS shall have the same value of nnpfc_purpose.");
        }
      }
#endif

      m_isNnpfActiveForCLVS.clear();
    }

    setPicActivatedNnpfc(currCodedPic);
    if (currCodedPic->m_nnpfcActivated.empty())
    {
      continue;
    }

    const SPS *sps = currCodedPic->slices[0]->getSPS();
    const PPS* pps = currCodedPic->slices[0]->getPPS();
    const int subWidthC  = SPS::getWinUnitX(sps->getChromaFormatIdc());
    const int subHeightC = SPS::getWinUnitY(sps->getChromaFormatIdc());

    uint32_t sourceWidth = pps->getPicWidthInLumaSamples()
                           - subWidthC
                               * (pps->getConformanceWindow().getWindowLeftOffset()
                                  + pps->getConformanceWindow().getWindowRightOffset());
    uint32_t sourceHeight = pps->getPicHeightInLumaSamples()
                            - subHeightC
                                * (pps->getConformanceWindow().getWindowTopOffset()
                                   + pps->getConformanceWindow().getWindowBottomOffset());

    uint32_t croppedWidth;
    uint32_t croppedHeight;

    auto superResolutionNnpfc = getSuperResolutionNnpfc(currCodedPic->m_nnpfcActivated);

    if (nullptr != superResolutionNnpfc)
    {
      croppedWidth  = superResolutionNnpfc->m_picWidthInLumaSamples;
      croppedHeight = superResolutionNnpfc->m_picHeightInLumaSamples;

#if JVET_AE0126_NNPF_EDITORIAL_CHANGES
      int outputPicWidth  = (int)ceil(((double) croppedWidth * (superResolutionNnpfc->m_picWidthNumeratorMinus1 + 1)) / (superResolutionNnpfc->m_picWidthDenominatorMinus1 + 1));
      int outputPicHeight = (int)ceil(((double) croppedHeight * (superResolutionNnpfc->m_picHeightNumeratorMinus1 + 1)) / (superResolutionNnpfc->m_picHeightDenominatorMinus1 + 1));

      CHECK(outputPicWidth == croppedWidth && outputPicHeight == croppedHeight, "When resolutionResamplingFlag is equal to 1, either nnpfcOutputPicWidth is not equal to CroppedWidth or nnpfcOutputPicHeight is not equal to CroppedHeight.");
#endif

    }
    else
    {
      croppedWidth = pps->getPicWidthInLumaSamples()
                     - subWidthC
                         * (pps->getConformanceWindow().getWindowLeftOffset()
                            + pps->getConformanceWindow().getWindowRightOffset());
      croppedHeight = pps->getPicHeightInLumaSamples()
                      - subHeightC
                          * (pps->getConformanceWindow().getWindowTopOffset()
                             + pps->getConformanceWindow().getWindowBottomOffset());
    }

    for (auto sei : currCodedPic->m_nnpfcActivated)
    {
      auto currNnpfc = (SEINeuralNetworkPostFilterCharacteristics*) sei;
      checkInputPics(currCodedPic, currNnpfc, sourceWidth, sourceHeight, croppedWidth, croppedHeight);
    }
  }
}

void SEINeuralNetworkPostFiltering::checkInputPics(
  Picture* currCodedPic, const SEINeuralNetworkPostFilterCharacteristics* currNnpfc,
  uint32_t sourceWidth, uint32_t sourceHeight, uint32_t croppedWidth, uint32_t croppedHeight)
{
  uint32_t numInputPics = currNnpfc->m_numberInputDecodedPicturesMinus1 + 1;

  std::vector<uint32_t> inputPicPoc(numInputPics, 0);
  uint32_t currPicPOC = currCodedPic->getPOC();
  inputPicPoc[0] = currPicPOC;

  if (numInputPics > 1)
  {
    bool fpCurrPicArrangementTypeIsFive = false;
    bool fpCurrPicFrameIsFrame0Flag     = false;
    if ((currNnpfc->m_purpose & NNPC_PurposeType::FRAME_RATE_UPSAMPLING) != 0)
    {
      const SEIMessages currPicFramePacking = getSeisByType(currCodedPic->SEIs, SEI::PayloadType::FRAME_PACKING);
      if (!currPicFramePacking.empty())
      {
        const SEIFramePacking* seiFramePacking = (SEIFramePacking*) *(currPicFramePacking.begin());
        fpCurrPicArrangementTypeIsFive = seiFramePacking->m_arrangementType == 5;
        fpCurrPicFrameIsFrame0Flag = seiFramePacking->m_currentFrameIsFrame0Flag;
      }
    }

    for (int i = 1; i < numInputPics; i++)
    {
      inputPicPoc[i]    = inputPicPoc[i - 1];
      Picture* inputPic = m_picList[i - 1];
      if ((currNnpfc->m_purpose & NNPC_PurposeType::FRAME_RATE_UPSAMPLING) != 0 && fpCurrPicArrangementTypeIsFive)
      {
        SEIMessages inputPicFramePacking = getSeisByType(inputPic->SEIs, SEI::PayloadType::FRAME_PACKING);
        if (!inputPicFramePacking.empty())
        {
          const SEIFramePacking* seiFramePacking = (SEIFramePacking*) *(inputPicFramePacking.begin());
          CHECK((seiFramePacking->m_arrangementType != 5
                 || seiFramePacking->m_currentFrameIsFrame0Flag != fpCurrPicFrameIsFrame0Flag),
                "If currCodedPic is associated with a frame packing arrangement SEI message with fp_arrangement_type equal to 5, "
                "inputPicPoc[i] must have the same value for fp_arrangement_type and fp_current_frame_is_frame0_flag");
        }
      }

      if (sourceWidth != croppedWidth || sourceHeight != croppedHeight)
      {
        bool inputPicUsesNnpfResolutionAdaptation = false;
        for (auto sei : inputPic->m_nnpfcActivated)
        {
          const auto *inputNnpfc = (SEINeuralNetworkPostFilterCharacteristics*)sei;
          if ((inputNnpfc->m_purpose & NNPC_PurposeType::RESOLUTION_UPSAMPLING) != 0)
          {
            CHECK(inputNnpfc->m_picWidthInLumaSamples != croppedWidth || inputNnpfc->m_picHeightInLumaSamples != croppedHeight, "Input picture shall have a super resolution NNPF activated");
            inputPicUsesNnpfResolutionAdaptation = true;
            break;
          }
        }
        CHECK(!inputPicUsesNnpfResolutionAdaptation, "Input picture does not uses a super resolution NNPF");
      }
    }
  }

  bool fpCurrPicArrangementTypeIsTemporalInterleave = false;
  bool fpCurrPicFrameIsFrame0Flag                   = false;
  const SEIMessages currPicFramePacking = getSeisByType(currCodedPic->SEIs, SEI::PayloadType::FRAME_PACKING);
  if (!currPicFramePacking.empty())
  {
    const SEIFramePacking* seiFramePacking = (SEIFramePacking*) *(currPicFramePacking.begin());
    fpCurrPicArrangementTypeIsTemporalInterleave = seiFramePacking->m_arrangementType == 5;
    fpCurrPicFrameIsFrame0Flag = seiFramePacking->m_currentFrameIsFrame0Flag;
  }

  SEIMessages nnpfaList = getSeisByType(currCodedPic->SEIs, SEI::PayloadType::NEURAL_NETWORK_POST_FILTER_ACTIVATION);
  SEINeuralNetworkPostFilterActivation* nnpfa = nullptr;

  // Find nnpfa that activated current nnpfc
  for (auto sei : nnpfaList)
  {
    auto nnpfaCand = (SEINeuralNetworkPostFilterActivation*) sei;
    if (nnpfaCand->m_targetId == currNnpfc->m_id)
    {
      nnpfa = nnpfaCand;
      break;
    }
  }

  if (nnpfa != nullptr)
  {
#if JVET_AE0135_NNPF_PIC_RATE_UPSAMPLING_CONSTRAINT
    int numPicsCurrLayer = 0;
#endif
    Picture *lastPic = nullptr;
    for (auto pic : m_picList)
    {
      if (pic->layerId == currCodedPic->layerId)
      {
#if JVET_AE0135_NNPF_PIC_RATE_UPSAMPLING_CONSTRAINT
        ++numPicsCurrLayer;
#endif
        if (lastPic == nullptr || (pic->getPOC() > lastPic->getPOC()))
        {
          lastPic = pic;
        }
      }
    }

    bool isCurrPicLastInOutputOrder = lastPic != nullptr && currCodedPic == lastPic;

    bool pictureRateUpsamplingFlag = (currNnpfc->m_purpose & FRAME_RATE_UPSAMPLING) != 0;
    int greaterThan0count = 0;
    int numPostRoll = 0;
    if (pictureRateUpsamplingFlag)
    {
      for (int i = 1; i < currNnpfc->m_numberInputDecodedPicturesMinus1; i++)
      {
        if (currNnpfc->m_numberInterpolatedPictures[i] > 0)
        {
          greaterThan0count++;
          numPostRoll = i;
        }
      }
#if JVET_AE0135_NNPF_PIC_RATE_UPSAMPLING_CONSTRAINT
      CHECK( numPicsCurrLayer > numInputPics && greaterThan0count > 1, "Disallow generating NNPF output pictures between any particular pair of consecutive input pictures more than once." );
#endif
    }

    int numInferences;
    if (pictureRateUpsamplingFlag && nnpfa->m_persistenceFlag && greaterThan0count == 1 && isCurrPicLastInOutputOrder)
    {
      numInferences = 1 + numPostRoll;
    }
    else
    {
      numInferences = 1;
    }

    for (int j = 0; j < numInferences; j++)
    {
      std::vector<Picture*> inputPic(numInputPics);
      std::vector<bool> inputPresentFlag(numInputPics);

      if (j > 0)
      {
        for (int k = 0; k <= (j - 1); k++)
        {
          inputPic[k] = currCodedPic;
          inputPresentFlag[k] = false;
        }
      }
      inputPic[j] = currCodedPic;
      inputPresentFlag[j] = true;

      if (numInputPics > 1)
      {
        for (int i = j + 1; i <= (numInputPics - j - 1); i++)
        {
          Picture *prevPic                                   = nullptr;
          Picture *prevPicWithTemporalInterleaveFramePacking = nullptr;

          for (auto pic : m_picList)
          {
            if (pic->layerId == currCodedPic->layerId && pic->getPOC() < inputPic[i - 1]->getPOC())
            {
              if (prevPic == nullptr || pic->getPOC() > prevPic->getPOC())
              {
                prevPic = pic;
              }
              if (prevPicWithTemporalInterleaveFramePacking == nullptr || pic->getPOC() > prevPicWithTemporalInterleaveFramePacking->getPOC())
              {
                SEIMessages picFramePacking = getSeisByType(pic->SEIs, SEI::PayloadType::FRAME_PACKING);
                if (!picFramePacking.empty())
                {
                  const SEIFramePacking* seiFramePacking = (SEIFramePacking*) *(picFramePacking.begin());
                  if (seiFramePacking->m_arrangementType == 5 && seiFramePacking->m_currentFrameIsFrame0Flag == fpCurrPicFrameIsFrame0Flag)
                  {
                    prevPicWithTemporalInterleaveFramePacking = pic;
                  }
                }
              }
            }
          }

          if (pictureRateUpsamplingFlag && fpCurrPicArrangementTypeIsTemporalInterleave && prevPicWithTemporalInterleaveFramePacking != nullptr)
          {
            inputPic[i]         = prevPicWithTemporalInterleaveFramePacking;
            inputPresentFlag[i] = true;
          }
          else if (!pictureRateUpsamplingFlag && prevPic != nullptr)
          {
            inputPic[i]         = prevPicWithTemporalInterleaveFramePacking;
            inputPresentFlag[i] = true;
          }
          else if (!fpCurrPicArrangementTypeIsTemporalInterleave && prevPic != nullptr)
          {
            inputPic[i]         = prevPicWithTemporalInterleaveFramePacking;
            inputPresentFlag[i] = true;
          }
          else
          {
            inputPic[i]         = inputPic[i - 1];
            inputPresentFlag[i] = false;
#if JVET_AE0142_NNPF_CONSTRAINT_BUGFIXES
            CHECK( pictureRateUpsamplingFlag && currNnpfc->m_numberInterpolatedPictures[i - 1] > 0, "It is a requirement of bitstream conformance that when PictureRateUpsamplingFlag is equal to 1, nnpfc_interpolated_pics[i - 1] shall not be greater than 0");
#else
            CHECK(currNnpfc->m_numberInterpolatedPictures[i] > 0, "It is a requirement of bitstream conformance that nnpfc_interpolated_pics[i - 1] shall not be greater than 0");
#endif
          }
        }
      }
    }
  }

}
