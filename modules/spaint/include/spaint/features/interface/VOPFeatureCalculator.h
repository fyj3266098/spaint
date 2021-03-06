/**
 * spaint: VOPFeatureCalculator.h
 * Copyright (c) Torr Vision Group, University of Oxford, 2015. All rights reserved.
 */

#ifndef H_SPAINT_VOPFEATURECALCULATOR
#define H_SPAINT_VOPFEATURECALCULATOR

#include "FeatureCalculator.h"

namespace spaint {

/**
 * \brief An instance of this class can be used to calculate VOP feature descriptors for voxels sampled from a scene.
 */
class VOPFeatureCalculator : public FeatureCalculator
{
  //#################### PRIVATE DEBUGGING VARIABLES ####################
private:
  /** The number of milliseconds by which to delay between consecutive frames when debugging (0 = pause). */
  mutable int m_debugDelayMs;

  /** The name of the debugging output window. */
  std::string m_debuggingOutputWindowName;

  //#################### PROTECTED VARIABLES ####################
protected:
  /** The number of bins into which to quantize orientations when aligning voxel patches. */
  size_t m_binCount;

  /** The side length of a VOP patch (must be odd). */
  size_t m_patchSize;

  /** The spacing in the scene between individual pixels in a patch. */
  float m_patchSpacing;

  /** The surface normals at the voxel locations. */
  boost::shared_ptr<ORUtils::MemoryBlock<Vector3f> > m_surfaceNormalsMB;

  /** The x axes of the coordinate systems in the tangent planes to the surfaces at the voxel locations. */
  boost::shared_ptr<ORUtils::MemoryBlock<Vector3f> > m_xAxesMB;

  /** The y axes of the coordinate systems in the tangent planes to the surfaces at the voxel locations. */
  boost::shared_ptr<ORUtils::MemoryBlock<Vector3f> > m_yAxesMB;

  //#################### CONSTRUCTORS ####################
protected:
  /**
   * \brief Constructs a VOP feature calculator.
   *
   * \param maxVoxelLocationCount The maximum number of voxel locations for which we will be calculating features at any one time.
   * \param patchSize             The side length of a VOP patch (must be odd).
   * \param patchSpacing          The spacing in the scene (in voxels) between individual pixels in a patch.
   * \param binCount              The number of bins into which to quantize orientations when aligning voxel patches.
   */
  VOPFeatureCalculator(size_t maxVoxelLocationCount, size_t patchSize, float patchSpacing, size_t binCount);

  //#################### PRIVATE ABSTRACT MEMBER FUNCTIONS ####################
private:
  /**
   * \brief Calculates the surface normals at the voxel locations.
   *
   * \param voxelLocationsMB  A memory block containing the locations of the voxels for which to calculate the surface normals.
   * \param voxelData         The scene's voxel data.
   * \param indexData         The scene's index data.
   * \param featuresMB        A memory block into which to store the calculated feature descriptors (packed sequentially).
   */
  virtual void calculate_surface_normals(const ORUtils::MemoryBlock<Vector3s>& voxelLocationsMB, const SpaintVoxel *voxelData, const ITMVoxelIndex::IndexData *indexData,
                                         ORUtils::MemoryBlock<float>& featuresMB) const = 0;

  /**
   * \brief Converts the VOP patch segments of the feature vectors for the voxels from RGB to CIELab.
   *
   * \param voxelLocationCount  The number of voxel locations for which we are calculating features.
   * \param featuresMB          A memory block into which to store the calculated feature descriptors (packed sequentially).
   */
  virtual void convert_patches_to_lab(int voxelLocationCount, ORUtils::MemoryBlock<float>& featuresMB) const = 0;

  /**
   * \brief Writes the height of each voxel into the corresponding feature vector for use as an extra feature.
   *
   * \param voxelLocationsMB  A memory block containing the locations of the voxels for which to fill in the heights.
   * \param featuresMB        A memory block into which to store the calculated feature descriptors (packed sequentially).
   */
  virtual void fill_in_heights(const ORUtils::MemoryBlock<Vector3s>& voxelLocationsMB, ORUtils::MemoryBlock<float>& featuresMB) const = 0;

  /**
   * \brief Generates coordinate systems in the tangent planes to the surfaces at the voxel locations.
   *
   * \param voxelLocationCount  The number of voxel locations for which we are calculating features.
   */
  virtual void generate_coordinate_systems(int voxelLocationCount) const = 0;

  /**
   * \brief Generates an RGB patch for each voxel by sampling from a regularly-spaced grid around it in its tangent plane.
   *
   * \param voxelLocationsMB  A memory block containing the locations of the voxels for which to generate RGB patches.
   * \param voxelData         The scene's voxel data.
   * \param indexData         The scene's index data.
   * \param featuresMB        A memory block into which to store the calculated feature descriptors (packed sequentially).
   */
  virtual void generate_rgb_patches(const ORUtils::MemoryBlock<Vector3s>& voxelLocationsMB,
                                    const SpaintVoxel *voxelData, const ITMVoxelIndex::IndexData *indexData,
                                    ORUtils::MemoryBlock<float>& featuresMB) const = 0;

  /**
   * \brief Updates the coordinate system for each voxel to align it with the dominant orientation in the voxel's RGB patch.
   *
   * This is intended to achieve at least some degree of rotation invariance.
   *
   * \param voxelLocationCount  The number of voxel locations for which we are calculating features.
   * \param featuresMB          A memory block into which to store the calculated feature descriptors (packed sequentially).
   */
  virtual void update_coordinate_systems(int voxelLocationCount, const ORUtils::MemoryBlock<float>& featuresMB) const = 0;

  //#################### PUBLIC MEMBER FUNCTIONS ####################
public:
  /** Override */
  virtual void calculate_features(const ORUtils::MemoryBlock<Vector3s>& voxelLocationsMB,
                                  const ITMLib::Objects::ITMScene<SpaintVoxel,ITMVoxelIndex> *scene,
                                  ORUtils::MemoryBlock<float>& featuresMB) const;

  /** Override */
  virtual size_t get_feature_count() const;

  //#################### PRIVATE MEMBER FUNCTIONS ####################
private:
  /**
   * \brief Displays the feature descriptors we have calculated for the voxels in a named OpenCV window.
   *
   * Note that we limit the resolution of the window for display purposes, so it is common for
   * some of the feature descriptors not to be shown if there is not enough space.
   *
   * \param featuresMB          A memory block containing the calculated feature descriptors (packed sequentially).
   * \param voxelLocationCount  The number of voxel locations for which we are calculating features.
   * \param windowName          The name of the window in which to show the feature descriptors.
   */
  void display_features(const ORUtils::MemoryBlock<float>& featuresMB, int voxelLocationCount, const std::string& windowName) const;

  /**
   * \brief Sets up a debugging window containing a trackbar that can be used to control the delay between consecutive frames.
   */
  void process_debug_window() const;
};

}

#endif
