#include <active_3d_planning/module/module_factory.h>

// Default types
#include "active_3d_planning/tools/defaults.h"
#include "active_3d_planning/module/trajectory_evaluator.h"
#include "active_3d_planning/module/trajectory_generator.h"

namespace active_3d_planning {

void ModuleFactory::getDefaultType(const std::type_index &module_index,
                                   std::string *type) {
  if (module_index == std::type_index(typeid(defaults::BoundingVolume))) {
    *type = "BoundingVolume";
  } else if (module_index ==
             std::type_index(typeid(defaults::SystemConstraints))) {
    *type = "SystemConstraints";
  } else if (module_index == std::type_index(typeid(EvaluatorUpdater))) {
    *type = "UpdateNothing";
  } else if (module_index == std::type_index(typeid(GeneratorUpdater))) {
    *type = "UpdateNothingGenerator";
  }
}

} // namespace active_3d_planning