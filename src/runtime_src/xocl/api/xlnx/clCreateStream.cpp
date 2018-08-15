/**
 * Copyright (C) 2018-2019 Xilinx, Inc
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You may
 * not use this file except in compliance with the License. A copy of the
 * License is located at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <CL/opencl.h>
#include "xocl/core/stream.h"
#include "xocl/core/error.h"
#include "plugin/xdp/profile.h"


// Copyright 2018 Xilinx, Inc. All rights reserved.

namespace xocl {
static void
validOrError(cl_device_id          device_id,
             cl_stream_flags       flags,
	     cl_stream_attributes* attributes,
             cl_int *              errcode_ret)
{
}

static cl_stream 
clCreateStream(cl_device_id           device_id,
	       cl_stream_flags        flags,
	       cl_stream_attributes*  attributes,
	       cl_int*                errcode_ret) 
{
  validOrError(device_id,flags,attributes,errcode_ret);
  return nullptr;
}

} //xocl

CL_API_ENTRY cl_stream CL_API_CALL
clCreateStream(cl_device_id           device_id,
	       cl_stream_flags        flags,
	       cl_stream_attributes*  attributes,
	       cl_int*                errcode_ret) CL_API_SUFFIX__VERSION_1_0
{
  try {
    PROFILE_LOG_FUNCTION_CALL;
    return xocl::clCreateStream
      (device_id,flags,attributes,errcode_ret);
  }
  catch (const xrt::error& ex) {
    xocl::send_exception_message(ex.what());
    xocl::assign(errcode_ret,ex.get_code());
  }
  catch (const std::exception& ex) {
    xocl::send_exception_message(ex.what());
    xocl::assign(errcode_ret,CL_INVALID_VALUE);
  }
  return nullptr;
}

