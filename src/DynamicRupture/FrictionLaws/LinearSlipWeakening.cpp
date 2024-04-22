#include "LinearSlipWeakening.h"
namespace seissol::dr::friction_law {

void NoSpecialization::resampleSlipRate(
    real (&resampledSlipRate)[dr::misc::numPaddedPoints],
    real const (&slipRateMagnitude)[dr::misc::numPaddedPoints]) {
  dynamicRupture::kernel::resampleParameter resampleKrnl;
  resampleKrnl.resample = init::resample::Values;
  resampleKrnl.originalQ = slipRateMagnitude;
  resampleKrnl.resampledQ = resampledSlipRate;
  resampleKrnl.execute();
}
void BiMaterialFault::copyLtsTreeToLocal(seissol::initializer::Layer& layerData,
                                         seissol::initializer::DynamicRupture const* const dynRup,
                                         real fullUpdateTime) {
  auto* concreteLts =
      dynamic_cast<seissol::initializer::LTSLinearSlipWeakeningBimaterial const* const>(dynRup);
  regularisedStrength = layerData.var(concreteLts->regularisedStrength);
}

#pragma omp declare simd
real BiMaterialFault::strengthHook(real faultStrength,
                                   real localSlipRate,
                                   real deltaT,
                                   unsigned int ltsFace,
                                   unsigned int pointIndex) {
  // modify strength according to Prakash-Clifton
  // see e.g.: Pelties - Verification of an ADER-DG method for complex dynamic rupture problems
  const real expterm =
      std::exp(-(std::max(static_cast<real>(0.0), localSlipRate) + drParameters->vStar) * deltaT /
               drParameters->prakashLength);
  const real newStrength =
      regularisedStrength[ltsFace][pointIndex] * expterm + faultStrength * (1.0 - expterm);
  regularisedStrength[ltsFace][pointIndex] = newStrength;
  return newStrength;
}

#pragma omp declare simd
real TPApprox::stateVariableHook(real localAccumulatedSlip,
                                 real localDc,
                                 unsigned int ltsFace,
                                 unsigned int pointIndex) {
  const real factor = (1.0 + std::fabs(localAccumulatedSlip) / localDc);
  return 1.0 - pow(factor, drParameters->tpProxyExponent);
}

} // namespace seissol::dr::friction_law
