#ifndef INITIALIZER_BOUNDARY_H_
#define INITIALIZER_BOUNDARY_H_

#include <Initializer/typedefs.hpp>
#include <Initializer/tree/LTSTree.hpp>


#ifndef ACL_DEVICE
# define MEMKIND_BOUNDARY  initializers::AllocationMode::HostOnly
#else
# define MEMKIND_BOUNDARY  initializers::AllocationMode::HostDeviceSplit // initializers::AllocationMode::HostDeviceUnified
#endif // ACL_DEVICE

namespace seissol {
  namespace initializers {
    struct Boundary;
  }
}

struct seissol::initializers::Boundary {
  Variable<BoundaryFaceInformation> faceInformation;
  
  void addTo(LTSTree& tree) {
    LayerMask mask = LayerMask(Ghost);
    tree.addVar(faceInformation, mask, 1, MEMKIND_BOUNDARY);
  }
};
#endif
