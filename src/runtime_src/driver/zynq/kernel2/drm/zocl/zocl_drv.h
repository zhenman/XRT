/*
 * A GEM style (optionally CMA backed) device manager for ZynQ based
 * OpenCL accelerators.
 *
 * Copyright (C) 2016 Xilinx, Inc. All rights reserved.
 *
 * Authors:
 *    Sonal Santan <sonal.santan@xilinx.com>
 *    Umang Parekh <umang.parekh@xilinx.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _ZOCL_DRV_H_
#define _ZOCL_DRV_H_
#include <drm/drmP.h>
#include <drm/drm_gem.h>
#include <drm/drm_mm.h>
#include <drm/drm_gem_cma_helper.h>
#include <linux/version.h>
#include "zocl_ioctl.h"
#include "xclbin.h"

#define zocl_err(dev, fmt, args...)     \
  dev_err(dev, "%s: "fmt, __func__, ##args)

#define CLEAR(x)			\
	memset(&x, 0, sizeof(x))

struct drm_zocl_exec_metadata {
  enum drm_zocl_execbuf_state state;
  unsigned int                index;
};

struct drm_zocl_bo {
	union {
		struct drm_gem_cma_object       cma_base;
		struct {
			struct drm_gem_object         gem_base;
			struct page                 **pages;
			struct sg_table              *sgt;
			void                         *vmapping;
			uint64_t                      uaddr;
		};
	};
  struct drm_zocl_exec_metadata  metadata;
	uint32_t                       flags;
};

struct zocl_mem_topology {
	//TODO : check the first 4 entries - remove unneccessary ones.
	u32             			bank_count;
	struct mem_data 		 *m_data;
	u32             			m_data_length; /* length of the mem_data section */
	u64             			bank_size; /* in KB. Currently only fixed sizes are supported. */
	u64                 	size;
	struct mem_topology  *topology;
};

struct zocl_connectivity {
	u64                   size;
	struct connectivity  *connections;
};

struct zocl_layout {
	u64                   size;
	struct ip_layout     *layout;
};

struct zocl_debug_layout {
	u64                     size;
	struct debug_ip_layout *layout;
};

struct drm_zocl_dev {
	struct drm_device       *ddev;
	struct fpga_manager     *fpga_mgr;
	struct iommu_domain     *domain;
	void __iomem            *regs;
	phys_addr_t              res_start;
	resource_size_t          res_len;
	unsigned int             irq;
  struct sched_exec_core  *exec;

  struct zocl_mem_topology topology;
  struct zocl_layout       layout;
  struct zocl_debug_layout debug_layout;
  struct zocl_connectivity connectivity;
  u64                      unique_id_last_bitstream;
	//xuid_t									 xclbin_id;
};

static inline struct drm_gem_object *
zocl_gem_object_lookup(struct drm_device *dev,
							         struct drm_file   *filp,
							         u32                handle)
{
	return drm_gem_object_lookup(filp, handle);
}

static inline struct
drm_zocl_bo *to_zocl_bo(struct drm_gem_object *bo)
{
	return (struct drm_zocl_bo *) bo;
}

static inline bool
zocl_bo_userptr(const struct drm_zocl_bo *bo)
{
    return (bo->flags & DRM_ZOCL_BO_FLAGS_USERPTR);
}

static inline bool
zocl_bo_execbuf(const struct drm_zocl_bo *bo)
{
  return (bo->flags & DRM_ZOCL_BO_FLAGS_EXECBUF);
}


int zocl_create_bo_ioctl(struct drm_device *dev, void *data,
    struct drm_file *filp);
int zocl_userptr_bo_ioctl(struct drm_device *dev, void *data,
    struct drm_file *filp);
int zocl_sync_bo_ioctl(struct drm_device *dev, void *data,
    struct drm_file *filp);
int zocl_map_bo_ioctl(struct drm_device *dev, void *data,
    struct drm_file *filp);
int zocl_info_bo_ioctl(struct drm_device *dev, void *data,
    struct drm_file *filp);
int zocl_pwrite_bo_ioctl(struct drm_device *dev, void *data,
    struct drm_file *filp);
int zocl_pread_bo_ioctl(struct drm_device *dev, void *data,
    struct drm_file *filp);
int zocl_execbuf_ioctl(struct drm_device *dev, void *data,
    struct drm_file *filp);
int zocl_read_axlf_ioctl(struct drm_device *dev, void *data,
		struct drm_file *filp);
void zocl_describe(const struct drm_zocl_bo *obj);

void zocl_free_userptr_bo(struct drm_gem_object *obj);
int zocl_iommu_map_bo(struct drm_device *dev, struct drm_zocl_bo *bo);
int zocl_iommu_unmap_bo(struct drm_device *dev, struct drm_zocl_bo *bo);
#if defined(XCLBIN_DOWNLOAD)
int zocl_pcap_download_ioctl(struct drm_device *dev, void *data,
                             struct drm_file *filp);
#endif

int zocl_init_sysfs(struct device *dev);
void zocl_fini_sysfs(struct device *dev);

void zocl_free_sections(struct drm_zocl_dev *zdev);

void zocl_free_bo(struct drm_gem_object *obj);
#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 13, 0)
static inline void drm_free_large(void *ptr)
{
	kvfree(ptr);
}

static inline void *drm_malloc_ab(size_t nmemb, size_t size)
{
	return kvmalloc_array(nmemb, sizeof(struct page *), GFP_KERNEL);
}
#endif

#endif