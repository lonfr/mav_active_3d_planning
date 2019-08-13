#include "active_3d_planning/module/sensor_model/simple_ray_caster.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace active_3d_planning {
namespace sensor_model {

ModuleFactoryRegistry::Registration<SimpleRayCaster>
    SimpleRayCaster::registration("SimpleRayCaster");

SimpleRayCaster::SimpleRayCaster(PlannerI &planner) : CameraModel(planner) {}
void SimpleRayCaster::setupFromParamMap(Module::ParamMap *param_map) {
  CameraModel::setupFromParamMap(param_map);
  setParam<double>(param_map, "ray_step", &p_ray_step_, (double)c_voxel_size_);
  setParam<double>(param_map, "downsampling_factor", &p_downsampling_factor_,
                   1.0);

  // Downsample to voxel size resolution at max range
  c_res_x_ =
      std::min((int)ceil(p_ray_length_ * c_field_of_view_x_ /
                         ((double)c_voxel_size_ * p_downsampling_factor_)),
               p_resolution_x_);
  c_res_y_ =
      std::min((int)ceil(p_ray_length_ * c_field_of_view_y_ /
                         ((double)c_voxel_size_ * p_downsampling_factor_)),
               p_resolution_y_);
}

bool SimpleRayCaster::getVisibleVoxels(std::vector<Eigen::Vector3d> *result,
                                       const Eigen::Vector3d &position,
                                       const Eigen::Quaterniond &orientation) {
  // Naive ray-casting
  Eigen::Vector3d camera_direction;
  Eigen::Vector3d direction;
  Eigen::Vector3d current_position;
  for (int i = 0; i < c_res_x_; ++i) {
    for (int j = 0; j < c_res_y_; ++j) {
      CameraModel::getDirectionVector(&camera_direction,
                                      (double)i / ((double)c_res_x_ - 1.0),
                                      (double)j / ((double)c_res_y_ - 1.0));
      direction = orientation * camera_direction;
      double distance = 0.0;
      while (distance < p_ray_length_) {
        current_position = position + distance * direction;
        distance += p_ray_step_;

        // Check voxel occupied
        double map_distance = 0.0;
        if (voxblox_.getEsdfMapPtr()->getDistanceAtPosition(current_position,
                                                            &map_distance)) {
          if (map_distance < 0.0) {
            break;
          }
        }

        // Add point (duplicates are handled in
        // CameraModel::getVisibleVoxelsFromTrajectory)
        getVoxelCenter(&current_position);
        result->push_back(current_position);
      }
    }
  }
  return true;
}

} // namespace sensor_model
} // namespace active_3d_planning